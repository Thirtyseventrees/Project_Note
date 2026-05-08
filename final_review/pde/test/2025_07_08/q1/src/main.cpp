#include "q1.hpp"

// Main function.
int
main(int argc, char *argv[])
{
  Utilities::MPI::MPI_InitFinalize mpi_init(argc, argv);

  // Problem parameters from the exercise:
  // alpha = 100, mu = 1, T = 1
  // Taylor-Hood: P2 for velocity, P1 for pressure
  
  const std::string  mesh_file_name  = "../mesh/mesh-pipe.msh";
  const unsigned int degree_velocity = 2;
  const unsigned int degree_pressure = 1;
  const double       T               = 1.0;
  const double       deltat          = 0.01;  // Choose appropriate time step
  const double       alpha           = 100.0;
  const double       mu              = 1.0;

  NavierStokes problem(mesh_file_name,
                       degree_velocity,
                       degree_pressure,
                       T,
                       deltat,
                       alpha,
                       mu);

  problem.run();

  return 0;
}