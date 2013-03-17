/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010 by Joel Andersson, Moritz Diehl, K.U.Leuven. All rights reserved.
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef SETNONZEROS_HPP
#define SETNONZEROS_HPP

#include "mx_node.hpp"
#include <map>
#include <stack>

namespace CasADi{

  /** \brief Assign or add entries to a matrix
      \author Joel Andersson
      \date 2013
  */
  template<bool ADD>
  class SetNonzerosBase : public MXNode{
  public:

    /// Constructor
    SetNonzerosBase(const std::vector<int>& nz) : nz_(nz){}

    /// Destructor
    virtual ~SetNonzerosBase() = 0;

    /// Clone function
    virtual SetNonzerosBase* clone() const = 0;
    
    /// Evaluate the function symbolically (MX)
    void evaluateMX(const MXPtrV& input, MXPtrV& output, const MXPtrVV& fwdSeed, MXPtrVV& fwdSens, const MXPtrVV& adjSeed, MXPtrVV& adjSens, bool output_given);

    /** \brief Get the operation */
    virtual int getOp() const{ return ADD ? OP_ADDNONZEROS : OP_SETNONZEROS;}

    /// Operation sequence
    std::vector<int> nz_;
  };


    /** \brief Add the nonzeros of a matrix to another matrix
      \author Joel Andersson
      \date 2013
  */
  template<bool ADD>
  class SetNonzerosVector : public SetNonzerosBase<ADD>{
  public:

    /// Constructor
    SetNonzerosVector(const MX& y, const MX& x, const std::vector<int>& nz);

    /// Clone function
    virtual SetNonzerosVector* clone() const{ return new SetNonzerosVector(*this);}
      
    /// Destructor
    virtual ~SetNonzerosVector(){}

    /// Evaluate the function numerically
    virtual void evaluateD(const DMatrixPtrV& input, DMatrixPtrV& output, const DMatrixPtrVV& fwdSeed, DMatrixPtrVV& fwdSens, const DMatrixPtrVV& adjSeed, DMatrixPtrVV& adjSens);

    /// Evaluate the function symbolically (SX)
    virtual void evaluateSX(const SXMatrixPtrV& input, SXMatrixPtrV& output, const SXMatrixPtrVV& fwdSeed, SXMatrixPtrVV& fwdSens, const SXMatrixPtrVV& adjSeed, SXMatrixPtrVV& adjSens);

    /// Propagate sparsity
    virtual void propagateSparsity(DMatrixPtrV& input, DMatrixPtrV& output, bool fwd);

    /// Evaluate the function (template)
    template<typename T, typename MatV, typename MatVV> 
    void evaluateGen(const MatV& input, MatV& output, const MatVV& fwdSeed, MatVV& fwdSens, const MatVV& adjSeed, MatVV& adjSens);    

    /// Print a part of the expression */
    virtual void printPart(std::ostream &stream, int part) const;
    
    /** \brief Generate code for the operation */
    virtual void generateOperation(std::ostream &stream, const std::vector<std::string>& arg, const std::vector<std::string>& res, CodeGenerator& gen) const;
            
    /// Construct the IMatrix that maps from the iind'th input to the output 
    Matrix<int> mapping(int iind=0) const;
    
    /// Check if the instance is in fact a simple assignment
    bool isAssignment() const;
    
    /// Simplify
    virtual void simplifyMe(MX& ex);

    /// Can the operation be performed inplace (i.e. overwrite the result)
    virtual int numInplace() const{ return 1;}
  };

  // Specialization of the above when nz_ is a Slice
  template<bool ADD>
  class SetNonzerosSlice : public SetNonzerosVector<ADD>{
  public:

    /// Constructor
    SetNonzerosSlice(const MX& y, const MX& x, const std::vector<int>& nz);

    /// Clone function
    virtual SetNonzerosSlice* clone() const{ return new SetNonzerosSlice(*this);}
      
    /// Destructor
    virtual ~SetNonzerosSlice(){}

    /// Propagate sparsity
    //    virtual void propagateSparsity(DMatrixPtrV& input, DMatrixPtrV& output, bool fwd);    

    /// Evaluate the function (template)
    //    template<typename T, typename MatV, typename MatVV> 
    //    void evaluateGen(const MatV& input, MatV& output, const MatVV& fwdSeed, MatVV& fwdSens, const MatVV& adjSeed, MatVV& adjSens);

    /// Evaluate the function numerically
    //    virtual void evaluateD(const DMatrixPtrV& input, DMatrixPtrV& output, const DMatrixPtrVV& fwdSeed, DMatrixPtrVV& fwdSens, const DMatrixPtrVV& adjSeed, DMatrixPtrVV& adjSens);

    /// Evaluate the function symbolically (SX)
    //    virtual void evaluateSX(const SXMatrixPtrV& input, SXMatrixPtrV& output, const SXMatrixPtrVV& fwdSeed, SXMatrixPtrVV& fwdSens, const SXMatrixPtrVV& adjSeed, SXMatrixPtrVV& adjSens);

    /// Print a part of the expression */
    virtual void printPart(std::ostream &stream, int part) const;

    /** \brief Generate code for the operation */
    virtual void generateOperation(std::ostream &stream, const std::vector<std::string>& arg, const std::vector<std::string>& res, CodeGenerator& gen) const;
    
    // Data member
    Slice s_;
  };

  // Specialization of the above when nz_ is a nested Slice
  template<bool ADD>
  class SetNonzerosSlice2 : public SetNonzerosVector<ADD>{
  public:

    /// Constructor
    SetNonzerosSlice2(const MX& y, const MX& x, const std::vector<int>& nz);

    /// Clone function
    virtual SetNonzerosSlice2* clone() const{ return new SetNonzerosSlice2(*this);}
      
    /// Destructor
    virtual ~SetNonzerosSlice2(){}

    /// Propagate sparsity
    //    virtual void propagateSparsity(DMatrixPtrV& input, DMatrixPtrV& output, bool fwd);    

    /// Evaluate the function (template)
    //    template<typename T, typename MatV, typename MatVV> 
    //    void evaluateGen(const MatV& input, MatV& output, const MatVV& fwdSeed, MatVV& fwdSens, const MatVV& adjSeed, MatVV& adjSens);

    /// Evaluate the function numerically
    //    virtual void evaluateD(const DMatrixPtrV& input, DMatrixPtrV& output, const DMatrixPtrVV& fwdSeed, DMatrixPtrVV& fwdSens, const DMatrixPtrVV& adjSeed, DMatrixPtrVV& adjSens);

    /// Evaluate the function symbolically (SX)
    //    virtual void evaluateSX(const SXMatrixPtrV& input, SXMatrixPtrV& output, const SXMatrixPtrVV& fwdSeed, SXMatrixPtrVV& fwdSens, const SXMatrixPtrVV& adjSeed, SXMatrixPtrVV& adjSens);

    /// Print a part of the expression */
    virtual void printPart(std::ostream &stream, int part) const;

    /** \brief Generate code for the operation */
    virtual void generateOperation(std::ostream &stream, const std::vector<std::string>& arg, const std::vector<std::string>& res, CodeGenerator& gen) const;
    
    // Data members
    Slice inner_, outer_;
  };

} // namespace CasADi

#endif // SETNONZEROS_HPP
