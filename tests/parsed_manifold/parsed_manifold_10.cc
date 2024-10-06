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

#include <deal.II/opencascade/manifold_lib.h>
#include <deal.II/opencascade/utilities.h>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <GC_MakeCircle.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <deal2lkit/parsed_grid_generator.h>
#include <deal2lkit/utilities.h>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

#include "../tests.h"



using namespace deal2lkit;

using namespace OpenCASCADE;

int
main()
{
  initlog();

  std::string name = "NormalTomeshProjectionManifold";

  const unsigned int dim      = 2;
  const unsigned int spacedim = 3;

  ParsedGridGenerator<dim, spacedim> pgg("Default");

  ParameterHandler prm;
  dealii::ParameterAcceptor::declare_all_parameters(prm);
  std::stringstream input;

  input << "subsection Default" << std::endl
        << "  set Input grid file name = "
        << SOURCE_DIR "/iges_files/wigley.iges" << std::endl
        << "  set Grid to generate = file" << std::endl
        << "  set Optional int 1 = 0 " << std::endl
        << "  set Copy material to manifold ids = true" << std::endl
        << "  set Manifold descriptors = "
        << "0=NormalToMeshProjectionManifold:"
        << SOURCE_DIR "/iges_files/wigley.iges" << std::endl
        << "end" << std::endl;

  prm.parse_input_from_string(input.str().c_str());
  dealii::ParameterAcceptor::parse_all_parameters(prm);

  auto tria = pgg.serial();
  tria->refine_global(1);

  GridOut go;
  go.write_msh(*tria, deallog.get_file_stream());
}
