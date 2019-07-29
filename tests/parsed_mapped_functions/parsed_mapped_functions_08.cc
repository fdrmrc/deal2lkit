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

// testing default constructor


#include <deal2lkit/parameter_acceptor.h>
#include <deal2lkit/parsed_mapped_functions.h>
#include <deal2lkit/utilities.h>

#include "../tests.h"


using namespace deal2lkit;


int
main()
{
  initlog();
  ParsedMappedFunctions<3> pmf("Mapped functions", 4);

  dealii::ParameterAcceptor::initialize(
    SOURCE_DIR "/parameters/parsed_mapped_functions_08.prm",
    "used_parameters.prm");
  ParameterAcceptor::prm.log_parameters(deallog);

  Point<3> p(2, 3, 5);

  std::vector<unsigned int> ids = pmf.get_mapped_ids();
  for (unsigned int i = 0; i < ids.size(); ++i)
    {
      deallog << "Component mask id " << ids[i] << " "
              << pmf.get_mapped_mask(ids[i]) << std::endl;
      deallog << "Parsed Function on id: " << ids[i] << " "
              << (*pmf.get_mapped_function(ids[i])).value(p) << std::endl;
    }
}
