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

#ifndef _d2k_parsed_preconditioner_h
#define _d2k_parsed_preconditioner_h

#include <deal2lkit/config.h>
#include <deal.II/base/config.h>
#include <deal.II/lac/trilinos_precondition.h>
#include <deal2lkit/parsed_finite_element.h>
#include <deal2lkit/parameter_acceptor.h>
#include <deal2lkit/utilities.h>

using namespace dealii;


D2K_NAMESPACE_OPEN

/**
 * A solver selector which uses parameter files to choose between
 * different options. This object is a LinearOperator which can be
 * called in place of the inverse of another LinearOperator.
 *
 * Example usage is the following:
 *
 * @code
 * ParsedAMGPreconditioner<VEC> Ainv;
 * ParameterAcceptor::initialize(...);
 *
 * Ainv.op   = linear_operator<VEC>(A);
 * Ainv.prec = linear_operator<VEC>(sompreconditioners);
 *
 * x = Ainv*b;
 *
 * @endcode
 */
class ParsedAMGPreconditioner : public ParameterAcceptor, public TrilinosWrappers::PreconditionAMG
{
public:
  /**
   * Constructor. Build the inverse of an Operator using a parameter
   * file. A section name can be specified, the solver type, the
   * maximum number of iterations, and the reduction to reach
   * convergence. If you know in advance the operators this object
   * will need, you can also supply them here. They default to the
   * identity, and you can assign them later by setting op and prec.
   */
  ParsedAMGPreconditioner(const std::string &name = "",
                          const bool &elliptic = true,
                          const bool &higher_order_elements = false,
                          const unsigned int &n_cycles = 1,
                          const bool &w_cycle = false,
                          const double &aggregation_threshold = 1e-4,
                          const std::string &var_const_modes = "none",
                          const unsigned int &smoother_sweeps = 2,
                          const unsigned int &smoother_overlap = 0,
                          const bool &output_details = false,
                          const std::string &smoother_type = "Chebyshev",
                          const std::string &coarse_type = "Amesos-KLU"
                         );

  /**
   * Declare solver type and solver options.
   */
  virtual void declare_parameters(ParameterHandler &prm);

  // /**
  //  * Parse solver type and solver options.
  //  */
  // virtual void parse_parameters(ParameterHandler &prm);

  // /**
  //  * Initialize internal variables.
  //  */
  // virtual void parse_parameters_call_back();

  /**
   * TODO
   */
  template<int dim, int spacedim=dim, typename Matrix>
  void initialize_preconditioner(const ParsedFiniteElement<dim, spacedim > &fe,
                                 const DoFHandler<dim, spacedim > &dh,
                                 const Matrix &matrix);

  using TrilinosWrappers::PreconditionAMG::initialize;

private:
  /**
   * Determines whether the AMG preconditioner should be optimized for elliptic problems (ML option smoothed aggregation SA, using a Chebyshev smoother) or for non-elliptic problems (ML option non- symmetric smoothed aggregation NSSA, smoother is SSOR with underrelaxation).
   */
  bool elliptic;

  /**
   * Determines whether the matrix that the preconditioner is built upon is generated from linear or higher-order elements.
   */
  bool higher_order_elements;

  /**
   * Defines how many multigrid cycles should be performed by the preconditioner.
   */
  unsigned int n_cycles;

  /**
   * Defines whether a w-cycle should be used instead of the standard setting of a v-cycle.
   */
  bool w_cycle;

  /**
   * This threshold tells the AMG setup how the coarsening should be performed. In the AMG used by ML, all points that strongly couple with the tentative coarse-level point form one aggregate. The term strong coupling is controlled by the variable aggregation_threshold, meaning that all elements that are not smaller than aggregation_threshold times the diagonal element do couple strongly.
   */
  double aggregation_threshold;

  /**
   * Specifies the constant modes (near null space) of the matrix. This parameter tells AMG whether we work on a scalar equation (where the near null space only consists of ones, and default value is OK) or on a vector-valued equation. For vector-valued equation problem with n_component, the provided constant_modes should fulfill the following requirements:
  @code{cpp}
  n_component.size() == n_component
  n_component[*].size() == n_dof_local or n_component[*].size() == n_dof_global
  n_component[ic][id] == "idth DoF is corresponding to component ic
  @endcode
   */


  /**
   * TODO
   */
  std::string var_const_modes;

  /**
   * Determines how many sweeps of the smoother should be performed. When the flag elliptic is set to true, i.e., for elliptic or almost elliptic problems, the polynomial degree of the Chebyshev smoother is set to smoother_sweeps. The term sweeps refers to the number of matrix-vector products performed in the Chebyshev case. In the non-elliptic case, smoother_sweeps sets the number of SSOR relaxation sweeps for post-smoothing to be performed.
   */
  unsigned int smoother_sweeps;

  /**
   * Determines the overlap in the SSOR/Chebyshev error smoother when run in parallel.
   */
  unsigned int smoother_overlap;

  /**
   * If this flag is set to true, then internal information from the ML preconditioner is printed to screen. This can be useful when debugging the preconditioner.
   */
  bool output_details;

  /**
   * Determines which smoother to use for the AMG cycle. Possibilities for smoother_type are the following:
   * "Aztec", "IFPACK", "Jacobi", "ML symmetric Gauss-Seidel", "symmetric Gauss-Seidel", "ML Gauss-Seidel", "Gauss-Seidel", "block Gauss-Seidel", "symmetric block Gauss-Seidel", "Chebyshev", "MLS", "Hiptmair", "Amesos-KLU", "Amesos-Superlu", "Amesos-UMFPACK", "Amesos-Superludist", "Amesos-MUMPS", "user-defined", "SuperLU", "IFPACK-Chebyshev", "self", "do-nothing", "IC", "ICT", "ILU", "ILUT", "Block Chebyshev", "IFPACK-Block Chebyshev"
   */
  std::string smoother_type;

  /**
   * Determines which solver to use on the coarsest level. The same settings as for the smoother type are possible.
   */
  std::string coarse_type;
};


class ParsedJacobiPreconditioner : public ParameterAcceptor, public TrilinosWrappers::PreconditionJacobi
{
public:
  /**
   * Constructor. Build the inverse of an Operator using a parameter
   * file. A section name can be specified, the solver type, the
   * maximum number of iterations, and the reduction to reach
   * convergence. If you know in advance the operators this object
   * will need, you can also supply them here. They default to the
   * identity, and you can assign them later by setting op and prec.
   */
  ParsedJacobiPreconditioner(const std::string &name = "",
                             const double &omega = 1,
                             const double &min_diagonal = 0,
                             const unsigned int &n_sweeps = 1
                            );

  /**
   * Declare solver type and solver options.
   */
  virtual void declare_parameters(ParameterHandler &prm);

  // /**
  //  * Parse solver type and solver options.
  //  */
  // virtual void parse_parameters(ParameterHandler &prm);

  // /**
  //  * Initialize internal variables.
  //  */
  // virtual void parse_parameters_call_back();

  /**
   * TODO
   */
  template<int dim, int spacedim=dim, typename Matrix>
  void initialize_preconditioner(const ParsedFiniteElement<dim, spacedim > &fe,
                                 const DoFHandler<dim, spacedim > &dh,
                                 const Matrix &matrix);

  using TrilinosWrappers::PreconditionJacobi::initialize;

private:

  /**
   * This specifies the relaxation parameter in the Jacobi preconditioner.
   */
  double omega;

  /**
   * This specifies the minimum value the diagonal elements should have. This might be necessary when the Jacobi preconditioner is used on matrices with zero diagonal elements. In that case, a straight- forward application would not be possible since we would divide by zero.
   */
  double min_diagonal;

  /**
   * Sets how many times the given operation should be applied during the vmult() operation.
   */
  unsigned int n_sweeps;
};

D2K_NAMESPACE_CLOSE


#endif

