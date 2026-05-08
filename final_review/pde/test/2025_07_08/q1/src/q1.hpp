#include <deal.II/base/conditional_ostream.h>
#include <deal.II/base/quadrature_lib.h>

#include <deal.II/distributed/fully_distributed_tria.h>

#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_renumbering.h>
#include <deal.II/dofs/dof_tools.h>

#include <deal.II/fe/fe_simplex_p.h>
#include <deal.II/fe/fe_system.h>
#include <deal.II/fe/fe_values.h>
#include <deal.II/fe/fe_values_extractors.h>
#include <deal.II/fe/mapping_fe.h>

#include <deal.II/grid/grid_in.h>
#include <deal.II/grid/grid_tools.h>

#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/solver_gmres.h>
#include <deal.II/lac/trilinos_block_sparse_matrix.h>
#include <deal.II/lac/trilinos_parallel_block_vector.h>
#include <deal.II/lac/trilinos_precondition.h>
#include <deal.II/lac/trilinos_sparse_matrix.h>

#include <deal.II/numerics/data_out.h>
#include <deal.II/numerics/matrix_tools.h>
#include <deal.II/numerics/vector_tools.h>

#include <fstream>
#include <iostream>

using namespace dealii;

// Class implementing a solver for the unsteady Stokes problem with reaction term.
class NavierStokes
{
public:
  // Physical dimension (2D problem as shown in the figure)
  static constexpr unsigned int dim = 2;

  // Function for the Neumann boundary condition psi = -p0 * n
  // This returns dim+1 components (velocity + pressure)
  class FunctionPsi : public Function<dim>
  {
  public:
    FunctionPsi(const double &p0_)
      : Function<dim>(dim + 1)
      , p0(p0_)
    {}

    virtual void
    vector_value(const Point<dim> &/*p*/, Vector<double> &values) const override
    {
      // psi = -p0 * n, but we apply it component-wise in the assembly
      // On Gamma_0: n = (-1, 0), so psi = (p0, 0)
      // On Gamma_1: n = (0, -1), so psi = (0, p0)
      // We handle this in assembly based on boundary_id
      for (unsigned int i = 0; i < dim + 1; ++i)
        values[i] = 0.0;
    }

    virtual double
    value(const Point<dim> &/*p*/, const unsigned int component = 0) const override
    {
      if (component < dim)
        return 0.0;
      else
        return 0.0;
    }

    double
    get_p0() const
    {
      return p0;
    }

  protected:
    const double p0;
  };

  // Function for the inlet pressure p0(x)
  class FunctionP0 : public Function<dim>
  {
  public:
    FunctionP0()
      : Function<dim>(1)
    {}

    virtual double
    value(const Point<dim> &p, const unsigned int /*component*/ = 0) const override
    {
      // p0(x) = 2 on Gamma_0, 1 on Gamma_1
      // Based on the boundary, but we use it point-wise
      // For simplicity, we'll set the value in assembly based on boundary_id
      (void)p;
      return 0.0;
    }
  };

  // Block-diagonal preconditioner
  class PreconditionBlockDiagonal
  {
  public:
    void
    initialize(const TrilinosWrappers::SparseMatrix &velocity_stiffness_,
               const TrilinosWrappers::SparseMatrix &pressure_mass_)
    {
      velocity_stiffness = &velocity_stiffness_;
      pressure_mass      = &pressure_mass_;

      preconditioner_velocity.initialize(velocity_stiffness_);
      preconditioner_pressure.initialize(pressure_mass_);
    }

    void
    vmult(TrilinosWrappers::MPI::BlockVector       &dst,
          const TrilinosWrappers::MPI::BlockVector &src) const
    {
      SolverControl solver_control_velocity(1000, 1e-2 * src.block(0).l2_norm());
      SolverCG<TrilinosWrappers::MPI::Vector> solver_cg_velocity(solver_control_velocity);
      solver_cg_velocity.solve(*velocity_stiffness,
                               dst.block(0),
                               src.block(0),
                               preconditioner_velocity);

      SolverControl solver_control_pressure(1000, 1e-2 * src.block(1).l2_norm());
      SolverCG<TrilinosWrappers::MPI::Vector> solver_cg_pressure(solver_control_pressure);
      solver_cg_pressure.solve(*pressure_mass,
                               dst.block(1),
                               src.block(1),
                               preconditioner_pressure);
    }

  protected:
    const TrilinosWrappers::SparseMatrix *velocity_stiffness;
    TrilinosWrappers::PreconditionILU     preconditioner_velocity;
    const TrilinosWrappers::SparseMatrix *pressure_mass;
    TrilinosWrappers::PreconditionILU     preconditioner_pressure;
  };

  // Block-triangular preconditioner
  class PreconditionBlockTriangular
  {
  public:
    void
    initialize(const TrilinosWrappers::SparseMatrix &velocity_stiffness_,
               const TrilinosWrappers::SparseMatrix &pressure_mass_,
               const TrilinosWrappers::SparseMatrix &B_)
    {
      velocity_stiffness = &velocity_stiffness_;
      pressure_mass      = &pressure_mass_;
      B                  = &B_;

      preconditioner_velocity.initialize(velocity_stiffness_);
      preconditioner_pressure.initialize(pressure_mass_);
    }

    void
    vmult(TrilinosWrappers::MPI::BlockVector       &dst,
          const TrilinosWrappers::MPI::BlockVector &src) const
    {
      SolverControl solver_control_velocity(1000, 1e-2 * src.block(0).l2_norm());
      SolverCG<TrilinosWrappers::MPI::Vector> solver_cg_velocity(solver_control_velocity);
      solver_cg_velocity.solve(*velocity_stiffness,
                               dst.block(0),
                               src.block(0),
                               preconditioner_velocity);

      tmp.reinit(src.block(1));
      B->vmult(tmp, dst.block(0));
      tmp.sadd(-1.0, src.block(1));

      SolverControl solver_control_pressure(1000, 1e-2 * src.block(1).l2_norm());
      SolverCG<TrilinosWrappers::MPI::Vector> solver_cg_pressure(solver_control_pressure);
      solver_cg_pressure.solve(*pressure_mass,
                               dst.block(1),
                               tmp,
                               preconditioner_pressure);
    }

  protected:
    const TrilinosWrappers::SparseMatrix *velocity_stiffness;
    TrilinosWrappers::PreconditionILU     preconditioner_velocity;
    const TrilinosWrappers::SparseMatrix *pressure_mass;
    TrilinosWrappers::PreconditionILU     preconditioner_pressure;
    const TrilinosWrappers::SparseMatrix *B;
    mutable TrilinosWrappers::MPI::Vector tmp;
  };

  // Constructor
  NavierStokes(const std::string  &mesh_file_name_,
               const unsigned int &degree_velocity_,
               const unsigned int &degree_pressure_,
               const double       &T_,
               const double       &deltat_,
               const double       &alpha_,
               const double       &mu_)
    : mpi_size(Utilities::MPI::n_mpi_processes(MPI_COMM_WORLD))
    , mpi_rank(Utilities::MPI::this_mpi_process(MPI_COMM_WORLD))
    , pcout(std::cout, mpi_rank == 0)
    , mesh_file_name(mesh_file_name_)
    , degree_velocity(degree_velocity_)
    , degree_pressure(degree_pressure_)
    , T(T_)
    , deltat(deltat_)
    , alpha(alpha_)
    , mu(mu_)
    , mesh(MPI_COMM_WORLD)
    , function_psi_0(2.0)  // p0 = 2 on Gamma_0
    , function_psi_1(1.0)  // p0 = 1 on Gamma_1
  {}

  // Setup system
  void
  setup();

  // Assemble system for current time step
  void
  assemble(const double &time);

  // Solve linear system
  void
  solve();

  // Output results
  void
  output(const unsigned int &time_step) const;

  // Run the time-stepping simulation
  void
  run();

protected:
  // MPI parallel
  const unsigned int mpi_size;
  const unsigned int mpi_rank;
  ConditionalOStream pcout;

  // Problem definition
  const std::string mesh_file_name;
  const unsigned int degree_velocity;
  const unsigned int degree_pressure;

  // Time parameters
  const double T;
  const double deltat;

  // Physical parameters
  const double alpha;  // reaction coefficient
  const double mu;     // viscosity

  // Boundary condition functions
  FunctionPsi function_psi_0;
  FunctionPsi function_psi_1;

  // Mesh
  parallel::fullydistributed::Triangulation<dim> mesh;

  // Finite element space
  std::unique_ptr<FiniteElement<dim>> fe;

  // Quadrature formulas
  std::unique_ptr<Quadrature<dim>>     quadrature;
  std::unique_ptr<Quadrature<dim - 1>> quadrature_face;

  // DoF handler
  DoFHandler<dim> dof_handler;

  // DoF index sets
  IndexSet              locally_owned_dofs;
  IndexSet              locally_relevant_dofs;
  std::vector<IndexSet> block_owned_dofs;
  std::vector<IndexSet> block_relevant_dofs;

  // System matrix: (1/dt * M + alpha * M + A)
  TrilinosWrappers::BlockSparseMatrix system_matrix;

  // Pressure mass matrix for preconditioner
  TrilinosWrappers::BlockSparseMatrix pressure_mass;

  // Velocity mass matrix (needed for time stepping)
  TrilinosWrappers::BlockSparseMatrix velocity_mass;

  // Right-hand side vector
  TrilinosWrappers::MPI::BlockVector system_rhs;

  // Solution at current time step (without ghost elements)
  TrilinosWrappers::MPI::BlockVector solution_owned;

  // Solution at current time step (with ghost elements)
  TrilinosWrappers::MPI::BlockVector solution;

  // Solution at previous time step
  TrilinosWrappers::MPI::BlockVector solution_old;
};