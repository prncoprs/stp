/********************************************************************
 * AUTHORS: Vijay Ganesh, Trevor Hansen
 *
 * BEGIN DATE: November, 2005
 *
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
********************************************************************/

#ifndef BITBLASTNEW_H
#define BITBLASTNEW_H

#include "stp/STPManager/STPManager.h"
#include "stp/Simplifier/constantBitP/MultiplicationStats.h"
#include <cassert>
#include <cmath>
#include <list>
#include <map>

namespace simplifier
{
namespace constantBitP
{
class ConstantBitPropagation;
class FixedBits;
}
}

namespace stp
{

using std::list;
using std::set;
using simplifier::constantBitP::MultiplicationStats;

class Simplifier;
class ASTNode;
typedef vector<ASTNode> ASTVec;

template <class BBNode, class BBNodeManagerT> class BitBlaster;

template <class BBNode, class BBNodeManagerT> class BitBlaster // not copyable
{
  BBNode BBTrue, BBFalse;

  // Memo table for bit blasted terms.  If a node has already been
  // bitblasted, it is mapped to a vector of Boolean formulas for
  // the
  std::map<ASTNode, vector<BBNode>> BBTermMemo;

  // Memo table for bit blasted formulas.  If a node has already
  // been bitblasted, it is mapped to a node representing the
  // bitblasted equivalent
  std::map<ASTNode, BBNode> BBFormMemo;

  // Get vector of Boolean formulas for sum of two
  // vectors of Boolean formulas
  void BBPlus2(vector<BBNode>& sum, const vector<BBNode>& y, BBNode cin);

  // Increment
  vector<BBNode> BBInc(const vector<BBNode>& x);

  // Add one bit to a vector of bits.
  vector<BBNode> BBAddOneBit(const vector<BBNode>& x, BBNode cin);

  // Bitwise complement
  vector<BBNode> BBNeg(const vector<BBNode>& x);

  // Unary minus
  vector<BBNode> BBUminus(const vector<BBNode>& x);

  // Multiply.
  vector<BBNode> BBMult(const vector<BBNode>& x, const vector<BBNode>& y,
                        set<BBNode>& support, const ASTNode& n);
  void mult_allPairs(const vector<BBNode>& x, const vector<BBNode>& y,
                     set<BBNode>& support, vector<list<BBNode>>& products);
  void mult_Booth(const vector<BBNode>& x_i, const vector<BBNode>& y_i,
                  set<BBNode>& support, const stp::ASTNode& xN,
                  const stp::ASTNode& yN, vector<list<BBNode>>& products,
                  const ASTNode& n);
  vector<BBNode> mult_normal(const vector<BBNode>& x, const vector<BBNode>& y,
                             set<BBNode>& support, const ASTNode& n);

  vector<BBNode> batcher(const vector<BBNode>& in);
  vector<BBNode> mergeSorted(const vector<BBNode>& in1,
                             const vector<BBNode>& in2);
  vector<BBNode> compareOddEven(const vector<BBNode>& in);

  void setColumnsToZero(vector<list<BBNode>>& products, set<BBNode>& support,
                        const ASTNode& n);

  void sortingNetworkAdd(set<BBNode>& support, list<BBNode>& current,
                         vector<BBNode>& currentSorted,
                         vector<BBNode>& priorSorted);

  vector<BBNode> v6(vector<list<BBNode>>& products, set<BBNode>& support,
                    const ASTNode& n);
  vector<BBNode> v7(vector<list<BBNode>>& products, set<BBNode>& support,
                    const ASTNode& n);
  vector<BBNode> v8(vector<list<BBNode>>& products, set<BBNode>& support,
                    const ASTNode& n);
  vector<BBNode> v9(vector<list<BBNode>>& products, set<BBNode>& support,
                    const ASTNode& n);
  vector<BBNode> v13(vector<list<BBNode>>& products, set<BBNode>& support,
                     const ASTNode& n);

  vector<BBNode> multWithBounds(const ASTNode& n,
                                vector<list<BBNode>>& products,
                                set<BBNode>& toConjoinToTop);
  bool statsFound(const ASTNode& n);

  void mult_BubbleSorterWithBounds(set<BBNode>& support,
                                   list<BBNode>& currentColumn,
                                   vector<BBNode>& currentSorted,
                                   vector<BBNode>& priorSorted,
                                   const int minTrue = 0,
                                   const int maxTrue = ((unsigned)~0) >> 1);

  void buildAdditionNetworkResult(list<BBNode>& from, list<BBNode>& to,
                                  set<BBNode>& support, const bool top,
                                  const bool empty);
  vector<BBNode> buildAdditionNetworkResult(vector<list<BBNode>>& products,
                                            set<BBNode>& support,
                                            const ASTNode& n);

  vector<BBNode> BBAndBit(const vector<BBNode>& y, BBNode b);

  MultiplicationStats* getMS(const ASTNode& n, int& highestZero);

  /////////// The end of the multiplication stuff..

  void checkFixed(const vector<BBNode>& v, const ASTNode& n);

  // AND each bit of vector y with single bit b and return the result.
  // (used in BBMult)
  // vector<BBNode> BBAndBit(const vector<BBNode>& y, ASTNode b);

  // Returns vector<BBNode> for result - y.  This destroys "result".
  void BBSub(vector<BBNode>& result, const vector<BBNode>& y,
             set<BBNode>& support);

  // build ITE's (ITE cond then[i] else[i]) for each i.
  vector<BBNode> BBITE(const BBNode& cond, const vector<BBNode>& thn,
                       const vector<BBNode>& els);

  // Build a vector of zeros.
  vector<BBNode> BBfill(unsigned int width, BBNode fillval);

  // build an EQ formula
  BBNode BBEQ(const vector<BBNode>& left, const vector<BBNode>& right);

  // This implements a variant of binary long division.
  // q and r are "out" parameters.  rwidth puts a bound on the
  // recursion depth.   Unsigned only, for now.

  void BBDivMod(const vector<BBNode>& y, const vector<BBNode>& x,
                vector<BBNode>& q, vector<BBNode>& r, unsigned int rwidth,
                set<BBNode>& support);

  // Return formula for majority function of three formulas.
  BBNode Majority(const BBNode& a, const BBNode& b, const BBNode& c);

  // Internal bit blasting routines.
  BBNode BBBVLE(const vector<BBNode>& x, const vector<BBNode>& y,
                bool is_signed, bool is_bvlt = false);
  BBNode BBBVLE_variant1(const vector<BBNode>& x, const vector<BBNode>& y,
                         bool is_signed, bool is_bvlt = false);
  BBNode BBBVLE_variant2(const vector<BBNode>& x, const vector<BBNode>& y,
                         bool is_signed, bool is_bvlt = false);

  // Return bit-blasted form for BVLE, BVGE, BVGT, SBLE, etc.
  BBNode BBcompare(const ASTNode& form, set<BBNode>& support);

  void BBLShift(vector<BBNode>& x, unsigned int shift);
  void BBRShift(vector<BBNode>& x, unsigned int shift);

  // no copy, no assign.
  BitBlaster& operator=(const BitBlaster& other);
  BitBlaster(const BitBlaster& other);

  // Checks for constants.
  void commonCheck(const ASTNode& n);
  void check(const BBNode& x, const ASTNode& n);
  void check(const vector<BBNode>& x, const ASTNode& n);

  bool update(const ASTNode& n, const int i,
              simplifier::constantBitP::FixedBits* b, BBNode& bb,
              set<BBNode>& support);
  void updateTerm(const ASTNode& n, vector<BBNode>& bb, set<BBNode>& support);
  void updateForm(const ASTNode& n, BBNode& bb, set<BBNode>& support);

  const BBNode BBForm(const ASTNode& form, set<BBNode>& support);

  bool isConstant(const vector<BBNode>& v);
  ASTNode getConstant(const vector<BBNode>& v, const ASTNode& n);

  // Nodes in this set can be replaced by their constant values, without being
  // conjoined to the top..
  ASTNodeSet fixedFromBottom;

  UserDefinedFlags* uf;
  NodeFactory* ASTNF;
  Simplifier* simp;
  BBNodeManagerT* nf;

  ASTNodeSet booth_recoded; // Nodes that have been recoded.

public:
  simplifier::constantBitP::ConstantBitPropagation* cb;

  // Bit blast a bitvector term.  The term must have a kind for a
  // bitvector term.  Result is a ref to a vector of formula nodes
  // representing the boolean formula.
  const vector<BBNode> BBTerm(const ASTNode& term, set<BBNode>& support);
  typename std::map<ASTNode, vector<BBNode>>::iterator
  simplify_during_bb(ASTNode& term, std::set<BBNode>& support);

  BitBlaster(BBNodeManagerT* bnm, Simplifier* _simp, NodeFactory* astNodeF,
             UserDefinedFlags* _uf,
             simplifier::constantBitP::ConstantBitPropagation* cb_ = NULL)
      : uf(_uf)
  {
    nf = bnm;
    cb = cb_;
    BBTrue = nf->getTrue();
    BBFalse = nf->getFalse();
    simp = _simp;
    ASTNF = astNodeF;
  }

  void ClearAllTables()
  {
    BBTermMemo.clear();
    BBFormMemo.clear();
  }

  ~BitBlaster() { ClearAllTables(); }

  // Bitblast a formula
  const BBNode BBForm(const ASTNode& form);

  void getConsts(const ASTNode& n, ASTNodeMap& fromTo, ASTNodeMap& equivs);
};

} // end of namespace

#endif
