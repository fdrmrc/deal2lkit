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

#ifndef d2k_parsed_data_out_h
#define d2k_parsed_data_out_h

#include <deal.II/base/config.h>

#include <deal.II/base/logstream.h>
#include <deal.II/base/parameter_handler.h>

#include <deal.II/dofs/dof_handler.h>

#include <deal.II/grid/tria.h>

#include <deal.II/numerics/data_out.h>

#include <deal2lkit/config.h>
#include <deal2lkit/parameter_acceptor.h>
#include <deal2lkit/utilities.h>

#include <fstream>

D2K_NAMESPACE_OPEN


template <int dim, int spacedim = dim>
class ParsedDataOut : public ParameterAcceptor
{
public:
  /** Optional name for parameter section.
      @p incremental_run_prefix creates a progressive directories/subdirectories
      for every run. For istance if @p incremental_run_prefix = "sol/run"
      the function will create sol/run001 the first time the code is runned,
      sol/run002 the second time, and so on.*/
  ParsedDataOut(const std::string & name                   = "",
                const std::string & output_format          = "vtu",
                const unsigned int &subdivisions           = 1,
                const std::string & incremental_run_prefix = "",
                const std::string & base_name_input        = "solution",
                const std::string & files_to_save          = "",
                const MPI_Comm &    comm                   = MPI_COMM_WORLD);

  /** Initialize the given values for the paramter file. */
  virtual void
  declare_parameters(dealii::ParameterHandler &prm) override;

  /** Prepare names for output directories. */
  virtual void
  parse_parameters_call_back() override;

  /** Prepare to output data on the given file. This will initialize
      the data_out object and a file with a filename that is the
      combination of the @p base_name, the optional @p suffix,
      eventually a processor number and the output suffix.  */
  void
  prepare_data_output(const dealii::DoFHandler<dim, spacedim> &dh,
                      const std::string &                      suffix = "");

  /** Add the given vector to the output file. Prior to calling this
      method, you have to call the prepare_data_output method. The
      string can be a comma separated list of components, or a single
      description. In this latter case, a progressive number per
      component is added in the end. */
  template <typename VECTOR>
  void
  add_data_vector(const VECTOR &data_vector, const std::string &desc);


  /**
   * Wrapper for the corrisponding function in dealii.
   */
  template <typename VECTOR>
  void
  add_data_vector(const VECTOR &                             data_vector,
                  const dealii::DataPostprocessor<spacedim> &postproc);


  /** Actually write the file. Once the data_out has been prepared,
      vectors have been added, the data can be written to a file. This
      is done in this class. At the end of this function call,
      data_out and output_file are in a pristine situation, and the
      process can be started again.
      @p used_files is an optional variable that takes a list of useful files
      (ex. "parameter.prm time.dat") and copies these files
      in the @p incremental_run_prefix of the costructor function.*/
  void
  write_data_and_clear(const dealii::Mapping<dim, spacedim> &mapping =
                         dealii::StaticMappingQ1<dim, spacedim>::mapping);

private:
  /** Initialization flag.*/
  bool initialized;

  /** MPI communicator. */
  const MPI_Comm &comm;

  /** Number of processes. */
  const unsigned int n_mpi_processes;

  /** My mpi process. */
  const unsigned int this_mpi_process;

  /** Folder where solutions are stored. */
  std::string path_solution_dir;

  /** Output format. */
  std::string output_format;

  /** Number of subdivisions. */
  unsigned int subdivisions;

  /** Base name for output files. This base is used to generate all
      filenames. */
  std::string base_name;

  /** name of progressive directories. One for every run.
      For example sol/run will produces sol/run001
      for the first run, sol/run002 for the second, and so on. */
  std::string incremental_run_prefix;

  /**
   * a list of file to save at each run in @ incremental_run_prefix
   * directory. This list should be separated usign '%', e.g.:
   * file1.txt % file2.dat % etc..
   */
  std::string files_to_save;

  /** Solution names. */
  std::string solution_names;

  /** Current output name. When preparing data_out, this name will
      contain the base for the current output. This allows the user to
      use a different output name in different part of the program. */
  std::string current_name;

  /** Output the partitioning of the domain. */
  bool output_partitioning;

  /** Output file. */
  std::ofstream output_file;

  /** Outputs only the data that refers to this process. */
  shared_ptr<dealii::DataOut<dim, spacedim>> data_out;
};


// ============================================================
// Template specializations
// ============================================================

template <int dim, int spacedim>
template <typename VECTOR>
void
ParsedDataOut<dim, spacedim>::add_data_vector(const VECTOR &     data_vector,
                                              const std::string &desc)
{
  AssertThrow(initialized, dealii::ExcNotInitialized());
  dealii::deallog.push("AddingData");
  std::vector<std::string> dd = dealii::Utilities::split_string_list(desc);
  if (data_out->default_suffix() != "")
    {
      if (dd.size() == 1)
        {
          data_out->add_data_vector(data_vector, desc);
        }
      else
        {
          std::vector<std::string>::iterator sit = dd.begin();
          std::vector<int>                   occurrances;

          for (; sit != dd.end(); ++sit)
            occurrances.push_back(std::count(dd.begin(), dd.end(), *sit));

          std::vector<int>::iterator iit = occurrances.begin();
          sit                            = dd.begin();

          std::vector<
            dealii::DataComponentInterpretation::DataComponentInterpretation>
            data_component_interpretation;

          for (; iit != occurrances.end(); ++iit, ++sit)
            {
              if (*iit > 1)
                data_component_interpretation.push_back(
                  dealii::DataComponentInterpretation::
                    component_is_part_of_vector);
              else
                data_component_interpretation.push_back(
                  dealii::DataComponentInterpretation::component_is_scalar);
            }

          data_out->add_data_vector(
            data_vector,
            dd,
            dealii::DataOut<dim, spacedim>::type_dof_data,
            data_component_interpretation);
        }
      dealii::deallog << "Added data: " << desc << std::endl;
    }
  dealii::deallog.pop();
}


template <int dim, int spacedim>
template <typename VECTOR>
void
ParsedDataOut<dim, spacedim>::add_data_vector(
  const VECTOR &                             data_vector,
  const dealii::DataPostprocessor<spacedim> &postproc)
{
  AssertThrow(initialized, dealii::ExcNotInitialized());
  data_out->add_data_vector(data_vector, postproc);
}

D2K_NAMESPACE_CLOSE

#endif
