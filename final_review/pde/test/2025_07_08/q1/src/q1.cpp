#include "q1.hpp"

void
NavierStokes::setup()
{
  // Create the mesh
  {
    pcout << "Initializing the mesh" << std::endl;

    Triangulation<dim> mesh_serial;

    GridIn<dim> grid_in;
    grid_in.attach_triangulation(mesh_serial);

    std::ifstream grid_in_file(mesh_file_name);
    grid_in.read_msh(grid_in_file);

    GridTools::partition_triangulation(mpi_size, mesh_serial);
    const auto construction_data = TriangulationDescription::Utilities::
      create_description_from_triangulation(mesh_serial, MPI_COMM_WORLD);
    mesh.create_triangulation(construction_data);

    pcout << "  Number of elements = " << mesh.n_global_active_cells()
          << std::endl;
  }

  pcout << "-----------------------------------------------" << std::endl;

  // Initialize the finite element space
  {
    pcout << "Initializing the finite element space" << std::endl;

    const FE_SimplexP<dim> fe_scalar_velocity(degree_velocity);
    const FE_SimplexP<dim> fe_scalar_pressure(degree_pressure);
    fe = std::make_unique<FESystem<dim>>(fe_scalar_velocity,
                                         dim,
                                         fe_scalar_pressure,
                                         1);

    pcout << "  Velocity degree = " << fe_scalar_velocity.degree << std::endl;
    pcout << "  Pressure degree = " << fe_scalar_pressure.degree << std::endl;
    pcout << "  DoFs per cell   = " << fe->dofs_per_cell << std::endl;

    quadrature      = std::make_unique<QGaussSimplex<dim>>(fe->degree + 1);
    quadrature_face = std::make_unique<QGaussSimplex<dim - 1>>(fe->degree + 1);

    pcout << "  Quadrature points per cell = " << quadrature->size() << std::endl;
  }

  pcout << "-----------------------------------------------" << std::endl;

  // Initialize the DoF handler
  {
    pcout << "Initializing the DoF handler" << std::endl;

    dof_handler.reinit(mesh);
    dof_handler.distribute_dofs(*fe);

    // Reorder DoFs: velocity first, then pressure
    std::vector<unsigned int> block_component(dim + 1, 0);
    block_component[dim] = 1;
    DoFRenumbering::component_wise(dof_handler, block_component);

    locally_owned_dofs    = dof_handler.locally_owned_dofs();
    locally_relevant_dofs = DoFTools::extract_locally_relevant_dofs(dof_handler);

    std::vector<types::global_dof_index> dofs_per_block =
      DoFTools::count_dofs_per_fe_block(dof_handler, block_component);
    const unsigned int n_u = dofs_per_block[0];
    const unsigned int n_p = dofs_per_block[1];

    block_owned_dofs.resize(2);
    block_relevant_dofs.resize(2);
    block_owned_dofs[0]    = locally_owned_dofs.get_view(0, n_u);
    block_owned_dofs[1]    = locally_owned_dofs.get_view(n_u, n_u + n_p);
    block_relevant_dofs[0] = locally_relevant_dofs.get_view(0, n_u);
    block_relevant_dofs[1] = locally_relevant_dofs.get_view(n_u, n_u + n_p);

    pcout << "  Number of DoFs:" << std::endl;
    pcout << "    velocity = " << n_u << std::endl;
    pcout << "    pressure = " << n_p << std::endl;
    pcout << "    total    = " << n_u + n_p << std::endl;
  }

  pcout << "-----------------------------------------------" << std::endl;

  // Initialize the linear system
  {
    pcout << "Initializing the linear system" << std::endl;

    // Coupling table for system matrix (no p-p coupling)
    Table<2, DoFTools::Coupling> coupling(dim + 1, dim + 1);
    for (unsigned int c = 0; c < dim + 1; ++c)
      for (unsigned int d = 0; d < dim + 1; ++d)
        {
          if (c == dim && d == dim)
            coupling[c][d] = DoFTools::none;
          else
            coupling[c][d] = DoFTools::always;
        }

    TrilinosWrappers::BlockSparsityPattern sparsity(block_owned_dofs, MPI_COMM_WORLD);
    DoFTools::make_sparsity_pattern(dof_handler, coupling, sparsity);
    sparsity.compress();

    // Sparsity pattern for pressure mass matrix
    Table<2, DoFTools::Coupling> coupling_pressure(dim + 1, dim + 1);
    for (unsigned int c = 0; c < dim + 1; ++c)
      for (unsigned int d = 0; d < dim + 1; ++d)
        {
          if (c == dim && d == dim)
            coupling_pressure[c][d] = DoFTools::always;
          else
            coupling_pressure[c][d] = DoFTools::none;
        }

    TrilinosWrappers::BlockSparsityPattern sparsity_pressure_mass(block_owned_dofs,
                                                                   MPI_COMM_WORLD);
    DoFTools::make_sparsity_pattern(dof_handler, coupling_pressure, sparsity_pressure_mass);
    sparsity_pressure_mass.compress();

    // Sparsity pattern for velocity mass matrix
    Table<2, DoFTools::Coupling> coupling_velocity(dim + 1, dim + 1);
    for (unsigned int c = 0; c < dim + 1; ++c)
      for (unsigned int d = 0; d < dim + 1; ++d)
        {
          if (c < dim && d < dim)
            coupling_velocity[c][d] = DoFTools::always;
          else
            coupling_velocity[c][d] = DoFTools::none;
        }

    TrilinosWrappers::BlockSparsityPattern sparsity_velocity_mass(block_owned_dofs,
                                                                   MPI_COMM_WORLD);
    DoFTools::make_sparsity_pattern(dof_handler, coupling_velocity, sparsity_velocity_mass);
    sparsity_velocity_mass.compress();

    pcout << "  Initializing matrices" << std::endl;
    system_matrix.reinit(sparsity);
    pressure_mass.reinit(sparsity_pressure_mass);
    velocity_mass.reinit(sparsity_velocity_mass);

    pcout << "  Initializing vectors" << std::endl;
    system_rhs.reinit(block_owned_dofs, MPI_COMM_WORLD);
    solution_owned.reinit(block_owned_dofs, MPI_COMM_WORLD);
    solution.reinit(block_owned_dofs, block_relevant_dofs, MPI_COMM_WORLD);
    solution_old.reinit(block_owned_dofs, block_relevant_dofs, MPI_COMM_WORLD);
  }
}

void
NavierStokes::assemble(const double &time)
{
  pcout << "  Assembling the system" << std::endl;

  const unsigned int dofs_per_cell = fe->dofs_per_cell;
  const unsigned int n_q           = quadrature->size();
  const unsigned int n_q_face      = quadrature_face->size();

  FEValues<dim> fe_values(*fe,
                          *quadrature,
                          update_values | update_gradients |
                            update_quadrature_points | update_JxW_values);

  FEFaceValues<dim> fe_face_values(*fe,
                                   *quadrature_face,
                                   update_values | update_normal_vectors |
                                     update_JxW_values);

  FullMatrix<double> cell_matrix(dofs_per_cell, dofs_per_cell);
  FullMatrix<double> cell_pressure_mass_matrix(dofs_per_cell, dofs_per_cell);
  FullMatrix<double> cell_velocity_mass_matrix(dofs_per_cell, dofs_per_cell);
  Vector<double>     cell_rhs(dofs_per_cell);

  std::vector<types::global_dof_index> dof_indices(dofs_per_cell);

  system_matrix = 0.0;
  system_rhs    = 0.0;
  pressure_mass = 0.0;
  velocity_mass = 0.0;

  FEValuesExtractors::Vector velocity(0);
  FEValuesExtractors::Scalar pressure(dim);

  // For evaluating the old solution
  std::vector<Tensor<1, dim>> solution_old_velocity_values(n_q);

  for (const auto &cell : dof_handler.active_cell_iterators())
    {
      if (!cell->is_locally_owned())
        continue;

      fe_values.reinit(cell);

      cell_matrix               = 0.0;
      cell_rhs                  = 0.0;
      cell_pressure_mass_matrix = 0.0;
      cell_velocity_mass_matrix = 0.0;

      // Get old solution values at quadrature points
      fe_values[velocity].get_function_values(solution_old,
                                              solution_old_velocity_values);

      for (unsigned int q = 0; q < n_q; ++q)
        {
          for (unsigned int i = 0; i < dofs_per_cell; ++i)
            {
              for (unsigned int j = 0; j < dofs_per_cell; ++j)
                {
                  // Time derivative term: (1/dt) * (u, v)
                  cell_matrix(i, j) +=
                    (1.0 / deltat) *
                    scalar_product(fe_values[velocity].value(i, q),
                                   fe_values[velocity].value(j, q)) *
                    fe_values.JxW(q);

                  // Reaction term: alpha * (u, v)
                  cell_matrix(i, j) +=
                    alpha *
                    scalar_product(fe_values[velocity].value(i, q),
                                   fe_values[velocity].value(j, q)) *
                    fe_values.JxW(q);

                  // Viscosity term: mu * (grad u, grad v)
                  cell_matrix(i, j) +=
                    mu *
                    scalar_product(fe_values[velocity].gradient(i, q),
                                   fe_values[velocity].gradient(j, q)) *
                    fe_values.JxW(q);

                  // Pressure term in momentum equation: -(p, div v)
                  cell_matrix(i, j) -=
                    fe_values[velocity].divergence(i, q) *
                    fe_values[pressure].value(j, q) *
                    fe_values.JxW(q);

                  // Pressure term in continuity equation: -(div u, q)
                  cell_matrix(i, j) -=
                    fe_values[velocity].divergence(j, q) *
                    fe_values[pressure].value(i, q) *
                    fe_values.JxW(q);

                  // Pressure mass matrix (for preconditioner)
                  cell_pressure_mass_matrix(i, j) +=
                    fe_values[pressure].value(i, q) *
                    fe_values[pressure].value(j, q) / mu *
                    fe_values.JxW(q);

                  // Velocity mass matrix
                  cell_velocity_mass_matrix(i, j) +=
                    scalar_product(fe_values[velocity].value(i, q),
                                   fe_values[velocity].value(j, q)) *
                    fe_values.JxW(q);
                }

              // RHS: (1/dt) * (u^n, v)
              cell_rhs(i) +=
                (1.0 / deltat) *
                scalar_product(solution_old_velocity_values[q],
                               fe_values[velocity].value(i, q)) *
                fe_values.JxW(q);
            }
        }

      // Neumann boundary conditions: psi = -p0 * n
      if (cell->at_boundary())
        {
          for (unsigned int f = 0; f < cell->n_faces(); ++f)
            {
              if (cell->face(f)->at_boundary())
                {
                  const unsigned int boundary_id = cell->face(f)->boundary_id();

                  // Gamma_0 (tag 0): p0 = 2
                  // Gamma_1 (tag 1): p0 = 1
                  // These are Neumann boundaries
                  if (boundary_id == 0 || boundary_id == 1)
                    {
                      fe_face_values.reinit(cell, f);

                      double p0_value = (boundary_id == 0) ? 2.0 : 1.0;

                      for (unsigned int q = 0; q < n_q_face; ++q)
                        {
                          // psi = -p0 * n, so the boundary integral is:
                          // integral(psi . v) = integral(-p0 * n . v)
                          for (unsigned int i = 0; i < dofs_per_cell; ++i)
                            {
                              cell_rhs(i) +=
                                -p0_value *
                                scalar_product(fe_face_values.normal_vector(q),
                                               fe_face_values[velocity].value(i, q)) *
                                fe_face_values.JxW(q);
                            }
                        }
                    }
                }
            }
        }

      cell->get_dof_indices(dof_indices);

      system_matrix.add(dof_indices, cell_matrix);
      system_rhs.add(dof_indices, cell_rhs);
      pressure_mass.add(dof_indices, cell_pressure_mass_matrix);
      velocity_mass.add(dof_indices, cell_velocity_mass_matrix);
    }

  system_matrix.compress(VectorOperation::add);
  system_rhs.compress(VectorOperation::add);
  pressure_mass.compress(VectorOperation::add);
  velocity_mass.compress(VectorOperation::add);

  // Dirichlet boundary conditions: u = 0 on Gamma_D = Gamma_2 U Gamma_3
  {
    std::map<types::global_dof_index, double>           boundary_values;
    std::map<types::boundary_id, const Function<dim> *> boundary_functions;

    // Velocity mask (only apply to velocity components)
    ComponentMask velocity_mask(dim + 1, true);
    velocity_mask.set(dim, false);

    Functions::ZeroFunction<dim> zero_function(dim + 1);

    // Gamma_2 (tag 2) and Gamma_3 (tag 3) have homogeneous Dirichlet BC
    boundary_functions[2] = &zero_function;
    boundary_functions[3] = &zero_function;

    VectorTools::interpolate_boundary_values(dof_handler,
                                             boundary_functions,
                                             boundary_values,
                                             velocity_mask);

    MatrixTools::apply_boundary_values(
      boundary_values, system_matrix, solution_owned, system_rhs, false);
  }
}

void
NavierStokes::solve()
{
  pcout << "  Solving the linear system" << std::endl;

  SolverControl solver_control(2000, 1e-6 * system_rhs.l2_norm());
  SolverGMRES<TrilinosWrappers::MPI::BlockVector> solver(solver_control);

  PreconditionBlockTriangular preconditioner;
  preconditioner.initialize(system_matrix.block(0, 0),
                            pressure_mass.block(1, 1),
                            system_matrix.block(1, 0));

  solver.solve(system_matrix, solution_owned, system_rhs, preconditioner);
  pcout << "    " << solver_control.last_step() << " GMRES iterations" << std::endl;

  solution = solution_owned;
}

void
NavierStokes::output(const unsigned int &time_step) const
{
  DataOut<dim> data_out;

  std::vector<DataComponentInterpretation::DataComponentInterpretation>
    interpretation(dim, DataComponentInterpretation::component_is_part_of_vector);
  interpretation.push_back(DataComponentInterpretation::component_is_scalar);

  std::vector<std::string> names(dim, "velocity");
  names.push_back("pressure");

  data_out.add_data_vector(dof_handler, solution, names, interpretation);

  std::vector<unsigned int> partition_int(mesh.n_active_cells());
  GridTools::get_subdomain_association(mesh, partition_int);
  const Vector<double> partitioning(partition_int.begin(), partition_int.end());
  data_out.add_data_vector(partitioning, "partitioning");

  data_out.build_patches();

  const std::string output_file_name = "output-navier-stokes";
  data_out.write_vtu_with_pvtu_record("./",
                                      output_file_name,
                                      time_step,
                                      MPI_COMM_WORLD);

  pcout << "  Output written to " << output_file_name << std::endl;
}

void
NavierStokes::run()
{
  setup();

  // Initial condition: u = 0
  solution_owned = 0.0;
  solution       = solution_owned;
  solution_old   = solution_owned;

  // Output initial condition
  output(0);

  pcout << "===============================================" << std::endl;

  double       time            = 0.0;
  unsigned int time_step       = 0;

  while (time < T - 0.5 * deltat)
    {
      time += deltat;
      ++time_step;

      pcout << "Time step " << time_step << ", t = " << time << std::endl;

      assemble(time);
      solve();

      // Update old solution
      solution_old = solution;

      output(time_step);
    }

  pcout << "===============================================" << std::endl;
}