//-----------------------------------------------------------
//
//    Copyright (C) 2015 by the deal2lkit authors
//
//    This file is part of the deal2lkit library.
//
//    The deal2lkit library is free software; you can use it, redistribute
//    it, and/or modify it under the terms of the GNU Lesser General
//    Public License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//    The full text of the license can be found in the file LICENSE at
//    the top level of the deal2lkit distribution.
//
//-----------------------------------------------------------


#include <deal.II/base/mpi.h>
#include <deal.II/base/utilities.h>

#include <deal.II/grid/grid_out.h>

#include <deal.II/lac/vector.h>

#include <deal2lkit/parsed_grid_generator.h>
#include <deal2lkit/parsed_grid_refinement.h>
#include <deal2lkit/utilities.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "../tests.h"

using namespace deal2lkit;

template <int dim, int spacedim>
void
test()
{
  ParsedGridGenerator<dim, spacedim> pgg;
  ParsedGridRefinement               pgr;

  dealii::ParameterAcceptor::initialize();

  parallel::distributed::Triangulation<dim, spacedim> *tria =
    pgg.distributed(MPI_COMM_WORLD);

  tria->refine_global(3);

  Vector<float> criteria(tria->n_active_cells());

  for (auto cell : tria->active_cell_iterators())
    criteria[cell->index()] = cell->center().norm();

  pgr.mark_cells(criteria, *tria);

  tria->prepare_coarsening_and_refinement();
  tria->execute_coarsening_and_refinement();

  std::ostringstream filename;
  filename << "/tmp/mesh_" << dim << spacedim << "-mpi-"
           << Utilities::MPI::this_mpi_process(MPI_COMM_WORLD) << ".msh";
  GridOut go;
  go.write_msh(*tria, deallog.get_file_stream());
  std::ofstream ofile(filename.str().c_str());
  go.write_msh(*tria, ofile);

  delete tria;
}

int
main(int argc, char *argv[])
{
  Utilities::MPI::MPI_InitFinalize mpi_initialization(argc, argv, 1);
  MPILogInitAll                    log;

  //  test<1,1>();
  //  test<1,2>();
  test<2, 2>();
  test<2, 3>();
  test<3, 3>();
}
