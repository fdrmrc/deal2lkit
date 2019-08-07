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


#include <deal.II/base/utilities.h>

#include <deal.II/grid/grid_out.h>

#include <deal2lkit/parsed_grid_generator.h>
#include <deal2lkit/utilities.h>

#include "../tests.h"


using namespace deal2lkit;

template <int dim, int spacedim>
void
test(ParsedGridGenerator<dim, spacedim> &pgg)
{
  auto    tria = pgg.serial();
  GridOut go;
  go.write_msh(*tria, deallog.get_file_stream());
}

int
main()
{
  initlog();
  ParsedGridGenerator<2, 2> a("Read");

  ParameterHandler prm;
  ParameterAcceptor::declare_all_parameters(prm);
  prm.parse_input_from_string(""
                              "subsection Read\n"
                              "  set Grid to generate = file \n"
                              "  set Input grid file name = " SOURCE_DIR
                              "/grids/mesh_22.msh\n"
                              "end\n");

  ParameterAcceptor::parse_all_parameters(prm);

  test(a);
}
