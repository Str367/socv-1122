/****************************************************************************
  FileName     [ FddNodeV.cpp ]
  PackageName  [ ]
  Synopsis     [ Define BDD Node member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/


#include <sstream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include "bddMgrV.h"

// Initialize static data members
//
BddMgrV* FddNodeV::_BddMgrV = 0;
FddNodeVInt* FddNodeVInt::_terminal = 0;
FddNodeV FddNodeV::_one;
FddNodeV FddNodeV::_zero;
bool FddNodeV::_debugBddAddr = false;
bool FddNodeV::_debugRefCount = false;

// We check the hash when a new FddNodeVInt is possibly being created
FddNodeV::FddNodeV(size_t l, size_t r, size_t i, FDD_EDGE_FLAG f)
{
   FddNodeVInt* n = _BddMgrV->fdduniquify(l, r, i);
   // n should not = 0
   assert(n != 0);
   _node = size_t(n) + f;
   n->incRefCount();
}

// Copy constructor also needs to increase the _refCount
// Need to check if n._node != 0
FddNodeV::FddNodeV(const FddNodeV& n) : _node(n._node)
{
   FddNodeVInt* t = getFddNodeVInt();
   if (t)
      t->incRefCount();
}

// This function is called only when n is obtained from uniquify()...
// ==> n should not be "0"
FddNodeV::FddNodeV(FddNodeVInt* n, FDD_EDGE_FLAG f)
{
   assert(n != 0);
   // TODO
   _node = size_t(n) + f;
   n->incRefCount();
}

// Need to check if n._node != 0
FddNodeV::FddNodeV(size_t v) : _node(v)
{
   // TODO
   FddNodeVInt* n = getFddNodeVInt();
   if (n)
      n->incRefCount();
}

FddNodeV::FddNodeV(const BddNodeV& n)
{
   FddNodeV f = bddMgrV->Bdd2Fdd(n);
   _node = f();
   FddNodeVInt* t = getFddNodeVInt();
   if (t)
      t->incRefCount();
}

// Need to check if _node != 0
FddNodeV::~FddNodeV()
{
   // TODO
   FddNodeVInt* n = getFddNodeVInt();
   if (n)
      n->decRefCount();
}

const FddNodeV&
FddNodeV::getLeft() const
{
   assert(getFddNodeVInt() != 0);
   return getFddNodeVInt()->getLeft();
}

const FddNodeV&
FddNodeV::getRight() const
{
   assert(getFddNodeVInt() != 0);
   return getFddNodeVInt()->getRight();
}

// [Note] i SHOULD NOT < getLevel()
FddNodeV
FddNodeV::getLeftCofactor(unsigned i) const
{
   assert(getFddNodeVInt() != 0);
   assert(i > 0);
   // TODO
   if (i > getLevel()) return (*this);
   if (i == getLevel())
      // return isNegEdge()? ~getLeft()^getRight() : getLeft()^getRight();
      return getLeft()^getRight();
   FddNodeV t = getLeft().getLeftCofactor(i);
   FddNodeV e = getRight().getLeftCofactor(i);
   if (t == e) return t;
   // if (t == e) return isNegEdge()? ~t : t;
   FDD_EDGE_FLAG f = FDD_POS_EDGE;
   FddNodeV d = t^e;
   // if (t.isNegEdge()) { t = ~t; e = ~e; }
   return FddNodeV(d(), e(), getLevel(), f);
}

// [Note] i SHOULD NOT < getLevel()
FddNodeV
FddNodeV::getRightCofactor(unsigned i) const
{
   assert(getFddNodeVInt() != 0);
   assert(i > 0);
   // TODO
   if (i > getLevel()) return (*this);
   if (i == getLevel())
      // return isNegEdge()? ~getRight() : getRight();
      return getRight();
   FddNodeV d = getLeft(),
            r = getRight(),
            l = d^r;
   FddNodeV t = l.getRightCofactor(i);
   FddNodeV e = r.getRightCofactor(i);
   // if (t == e) return isNegEdge()? ~t : t;
   if (t == e) return t;
   FDD_EDGE_FLAG f = FDD_POS_EDGE;
   // if (t.isNegEdge()) { t = ~t; e = ~e; }
   return FddNodeV(t(), e(), getLevel(), f);
}

// [Note] i SHOULD NOT < getLevel()
// FddNodeV
// FddNodeV::getDiffCofactor(unsigned i) const
// {
//    assert(getFddNodeVInt() != 0);
//    assert(i > 0);
//    // TODO
//    if (i > getLevel()) return (FddNodeV::_zero);
//    if (i == getLevel())
//       // return isNegEdge()? ~getRight() : getRight();
//       return  getLeft();
//    FddNodeV t = getLeft().getRightCofactor(i);
//    FddNodeV e = getRight().getRightCofactor(i);
//    if (t == e) return isNegEdge()? ~t : t;
//    FDD_EDGE_FLAG f = (isNegEdge() ^ t.isNegEdge())?
//                       FDD_NEG_EDGE: FDD_POS_EDGE;
//    if (t.isNegEdge()) { t = ~t; e = ~e; }
//    return FddNodeV(t(), e(), getLevel(), f);
// }

unsigned
FddNodeV::getLevel() const
{
   return getFddNodeVInt()->getLevel();
}

unsigned
FddNodeV::getRefCount() const
{
   return getFddNodeVInt()->getRefCount();
}

// Note: FddNodeV a = b;
// ==> a's original FddNodeVInt's reference count should --
//     a's new FddNodeVInt's reference count should ++
FddNodeV&
FddNodeV::operator = (const FddNodeV& n)
{
   // TODO
   FddNodeVInt* t = getFddNodeVInt();
   if (t)
      t->decRefCount();
   _node = n._node;
   t = getFddNodeVInt();
   if (t)
      t->incRefCount();
   return (*this);
}

FddNodeV
FddNodeV::operator ~ () const
{
   // TODO
   return _BddMgrV->fddNot((*this));
}

FddNodeV
FddNodeV::operator & (const FddNodeV& n) const
{
   // TODO
   FddNodeV tmp = _BddMgrV->fddOr(~(*this),~n);
   return ~tmp;
   // return _BddMgrV->fddAnd((*this), n);
   // return _BddMgrV->ite((*this), n, FddNodeV::_zero);
}

FddNodeV&
FddNodeV::operator &= (const FddNodeV& n)
{
   // TODO
   (*this) = (*this) & n;
   return (*this);
}

FddNodeV
FddNodeV::operator | (const FddNodeV& n) const
{
   // TODO
   return _BddMgrV->fddOr((*this), n);
   // return _BddMgrV->ite((*this), FddNodeV::_one, n);
}

FddNodeV&
FddNodeV::operator |= (const FddNodeV& n)
{
   // TODO
   (*this) = (*this) | n;
   return (*this);
}

FddNodeV
FddNodeV::operator ^ (const FddNodeV& n) const
{
   // TODO
   // cout<<"xor"<<endl;
   return _BddMgrV->fddXor((*this), n);
   // return _BddMgrV->fddXor((*this), n);
   // return _BddMgrV->ite((*this), ~n, n);
}

FddNodeV&
FddNodeV::operator ^= (const FddNodeV& n)
{
   // TODO
   (*this) = (*this) ^ n;
   return (*this);
}

// (*this) < n iff
// (i) level < n.level
// (ii) level == n.level && _node < n._node
bool
FddNodeV::operator < (const FddNodeV& n) const
{
   // TODO
   unsigned l1 = getLevel();
   unsigned l2 = n.getLevel();
   return ((l1 < l2) ||
          ((l1 == l2) && (_node < n._node)));
}

// (*this) < n iff
// (i) level < n.level
// (ii) level == n.level && _node <= n._node
bool
FddNodeV::operator <= (const FddNodeV& n) const
{
   // TODO
   unsigned l1 = getLevel();
   unsigned l2 = n.getLevel();
   return ((l1 < l2) ||
          ((l1 == l2) && (_node <= n._node)));
}

bool
FddNodeV::isTerminal() const
{
   return (getFddNodeVInt() == FddNodeVInt::_terminal);
}

// [BONUS TODO]... starting
//
FddNodeV
FddNodeV::exist(unsigned l) const
{
   if (l == 0) return (*this);

   map<size_t, size_t> existMap;
   return existRecur(l, existMap);
}

FddNodeV
FddNodeV::existRecur(unsigned l, map<size_t, size_t>& existMap) const
{
   if (isTerminal()) return (*this);

   unsigned thisLevel = getLevel();
   if (l > thisLevel) return (*this);

   map<size_t, size_t>::iterator mi = existMap.find(_node);
   if (mi != existMap.end()) return (*mi).second;

   FddNodeV left = getLeftCofactor(thisLevel);
   FddNodeV right = getRightCofactor(thisLevel);
   if (l == thisLevel) {
      FddNodeV res = left | right;
      existMap[_node] = res();
      return res;
   }

   // l must < getLevel()
   bool isNegEdge = false;
   FddNodeV t = left.existRecur(l, existMap);
   FddNodeV e = right.existRecur(l, existMap);
   if (t == e) {
      existMap[_node] = t();
      return t;
   }
   if (t.isNegEdge()) {
      t = ~t; e = ~e; isNegEdge = true;
   }
   FddNodeV res(_BddMgrV->fdduniquify(t(), e(), thisLevel));
   if (isNegEdge) res = ~res;
   existMap[_node] = res();
   return res;
}

// // Move the BDD nodes in the cone >= fromLevel to toLevel.
// // After the move, there will be no BDD nodes between [fromLevel, toLevel).
// // Return the resulted BDD node.
// //
// // [Note] If (fromLevel > toLevel) ==> move down
// //        If (fromLevel < toLevel) ==> move up
// //
// // Before the move, need to make sure:
// // 1. (thisLevel - fromLevel) < abs(fromLevel - toLevel)
// // 2. There is no node < fromLevel (except for the terminal node).
// //
// // [Note] If (move up), there is no node above toLevel in the beginning.
// //
// // If any of the above is violated, isMoved will be set to false,
// // no move will be made, and return (*this).
// //
// FddNodeV
// FddNodeV::nodeMove(unsigned fromLevel, unsigned toLevel, bool& isMoved) const
// {
//    assert(fromLevel > 1);
//    if (int(getLevel() - fromLevel) >= abs(int(fromLevel - toLevel)) ||
//         containNode(fromLevel - 1, 1)) {
//       isMoved = false;
//       return (*this);
//    }

//    isMoved = true;
//    map<size_t, size_t> moveMap;
//    return nodeMoveRecur(fromLevel, toLevel, moveMap);
// }

// FddNodeV
// FddNodeV::nodeMoveRecur
// (unsigned fromLevel, unsigned toLevel, map<size_t, size_t>& moveMap) const
// {
//    unsigned thisLevel = getLevel();
//    assert(thisLevel >= fromLevel);

//    map<size_t, size_t>::iterator mi = moveMap.find(_node);
//    if (mi != moveMap.end()) return (*mi).second;

//    FddNodeV left = getLeft();
//    FddNodeV right = getRight();

//    if (!left.isTerminal())
//       left = left.nodeMoveRecur(fromLevel, toLevel, moveMap);
//    if (!right.isTerminal())
//       right = right.nodeMoveRecur(fromLevel, toLevel, moveMap);

//    FddNodeVInt *n
//    = _BddMgrV->fdduniquify(left(), right(), thisLevel - fromLevel + toLevel);
//    FddNodeV ret = FddNodeV(size_t(n));
//    if (isNegEdge()) ret = ~ret;

//    moveMap[_node] = ret();
//    return ret;
// }

// // Check if there is any FddNodeV in the cone of level [bLevel, eLevel]
// // return true if any
// //
// bool
// FddNodeV::containNode(unsigned bLevel, unsigned eLevel) const
// {
//    bool res = containNodeRecur(bLevel, eLevel);
//    unsetVisitedRecur();
//    return res;
// }

// bool
// FddNodeV::containNodeRecur(unsigned bLevel, unsigned eLevel) const
// {
//    FddNodeVInt* n = getFddNodeVInt();
//    if (n->isVisited())
//       return false;
//    n->setVisited();

//    unsigned thisLevel = getLevel();
//    if (thisLevel < bLevel) return false;
//    if (thisLevel <= eLevel) return true;

//    if (getLeft().containNodeRecur(bLevel, eLevel)) return true;
//    if (getRight().containNodeRecur(bLevel, eLevel)) return true;

//    return false;
// }     
   
// size_t
// FddNodeV::countCube() const
// {  
//    map<size_t, size_t> numCubeMap;
//    return countCubeRecur(false, numCubeMap);
// }  

// size_t
// FddNodeV::countCubeRecur(bool phase, map<size_t, size_t>& numCubeMap) const
// {
//    if (isTerminal())
//       return ((phase ^ isNegEdge())? 0 : 1); 

//    map<size_t, size_t>::iterator mi = numCubeMap.find(_node);
//    if (mi != numCubeMap.end()) return (*mi).second;

//    unsigned numCube = 0;
//    FddNodeV left = getLeft();
//    numCube += left.countCubeRecur(phase ^ isNegEdge(), numCubeMap);
//    FddNodeV right = getRight();
//    numCube += right.countCubeRecur(phase ^ isNegEdge(), numCubeMap);

//    numCubeMap[_node] = numCube;
//    return numCube;
// }

// FddNodeV
// FddNodeV::getCube(size_t ith) const
// {  
//    ith %= countCube();
//    FddNodeV res = FddNodeV::_one;
//    size_t idx = 0;
//    getCubeRecur(false, idx, ith, res);
//    return res;
// }
   
// // return true if the target-th cube is met
// bool
// FddNodeV::getCubeRecur
// (bool phase, size_t& ith, size_t target, FddNodeV& res) const
// {
//    if (isTerminal()) {
//       if (!(phase ^ isNegEdge())) {
//          if (ith == target) return true;
//          ++ith;
//       }
//       return false;
//    }

//    FddNodeV old = res;
//    FddNodeV left = getLeft();
//    res = old & _BddMgrV->getFddSupport(getLevel());
//    if (left.getCubeRecur(phase ^ isNegEdge(), ith, target, res))
//       return true;
//    FddNodeV right = getRight();
//    res = old & ~(_BddMgrV->getFddSupport(getLevel()));
//    if (right.getCubeRecur(phase ^ isNegEdge(), ith, target, res))
//       return true;

//    return false;
// }

// vector<FddNodeV>
// FddNodeV::getAllCubes() const
// {
//    vector<FddNodeV> allCubes;
//    FddNodeV cube = FddNodeV::_one;
//    getAllCubesRecur(false, cube, allCubes);
//    return allCubes;
// }

// void
// FddNodeV::getAllCubesRecur
// (bool phase, FddNodeV& cube, vector<FddNodeV>& allCubes) const
// {
//    if (isTerminal()) {
//       if (!(phase ^ isNegEdge()))
//          allCubes.push_back(cube);
//       return;
//    }

//    FddNodeV old = cube;
//    FddNodeV left = getLeft();
//    cube = old & _BddMgrV->getFddSupport(getLevel());
//    left.getAllCubesRecur(phase ^ isNegEdge(), cube, allCubes);
//    FddNodeV right = getRight();
//    cube = old & ~(_BddMgrV->getFddSupport(getLevel()));
//    right.getAllCubesRecur(phase ^ isNegEdge(), cube, allCubes);
// }

// Assume this FddNodeV is a cube
string
FddNodeV::toString() const
{
   string str;
   toStringRecur(false, str);
   return str;
}

// return true if a cube is found
bool
FddNodeV::toStringRecur(bool phase, string& str) const
{
   if (isTerminal())
      return (!(phase ^ isNegEdge()));

   stringstream sstr;
   if (getLeft().toStringRecur(phase ^ isNegEdge(), str)) {
      sstr << "(" << getLevel() << ") ";
      str += sstr.str();
      return true;
   }
   else if (getRight().toStringRecur(phase ^ isNegEdge(), str)) {
      sstr << "!(" << getLevel() << ") ";
      str += sstr.str();
      return true;
   }
   return false;
}
//
// [BONUS TODO]... ended

ostream&
operator << (ostream& os, const FddNodeV& n)
{
   size_t nNodes = 0;
   n.print(os, 0, nNodes);
   n.unsetVisitedRecur();
   os << endl << endl << "==> Total #FddNodeVs : " << nNodes << endl;
   return os;
}

void
FddNodeV::print(ostream& os, size_t indent, size_t& nNodes) const
{
   for (size_t i = 0; i < indent; ++i)
      os << ' ';
   FddNodeVInt* n = getFddNodeVInt();
   os << '[' << getLevel() << "](" << (isNegEdge()? '-' : '+') << ") ";
   if (_debugBddAddr)
      os << n << " ";
   if (_debugRefCount)
      os << "(" << n->getRefCount() << ")";
   if (n->isVisited()) {
      os << " (*)";
      return;
   }
   else ++nNodes;
   n->setVisited();
   if (!isTerminal()) {
      os << endl;
      n->getLeft().print(os, indent + 2, nNodes);
      os << endl;
      n->getRight().print(os, indent + 2, nNodes);
   }
}

void
FddNodeV::unsetVisitedRecur() const
{
   FddNodeVInt* n = getFddNodeVInt();
   if (!n->isVisited()) return;
   n->unsetVisited();
   if (!isTerminal()) {
      n->getLeft().unsetVisitedRecur();
      n->getRight().unsetVisitedRecur();
   }
}

void
FddNodeV::drawFdd(const string& name, ofstream& ofile) const
{
   // TODO
   ofile << "digraph {" << endl;
   ofile << "   node [shape = plaintext];" << endl;
   ofile << "   ";
   for (unsigned l = getLevel(); l > 0; --l)
      ofile << l << " -> ";
   ofile << "0 [style = invis];" << endl;
   ofile << "   { rank = source; \"" << name << "\"; }" << endl;
   ofile << "   node [shape = ellipse];" << endl;
   ofile << "   \"" << name << "\" -> \"" << getLabel()
         << "\" [color = blue]";
   ofile << (isNegEdge()? " [arrowhead = odot]" : ";") << endl;

   drawFddRecur(ofile);

   ofile << "   { rank = same; 0; \"One\"; }" << endl;
   ofile << "}" << endl;

   unsetVisitedRecur( );
}

void
FddNodeV::drawFddRecur(ofstream& ofile) const
{
   // TODO
   FddNodeVInt* n = getFddNodeVInt();
   if (n->isVisited())
      return;
   n->setVisited();
   if(isTerminal())
      return;
   FddNodeV left = getLeft();
   FddNodeV right = getRight();

   ofile << "   { rank = same; " << getLevel() << "; \"" << getLabel()
         << "\"; }\n";

   ofile << "   \"" << getLabel() << "\" -> \"" << left.getLabel() << "\"";
   ofile << ((left.isNegEdge())? " [arrowhead=odot]" : ";") << endl;

   ofile << "   \"" << getLabel() << "\" -> \"" << right.getLabel()
         << "\"[style = dotted ] [color=red]";
   ofile << ((right.isNegEdge())? " [arrowhead=odot]" : ";") << endl;

   left.drawFddRecur(ofile);
   right.drawFddRecur(ofile);
}

// Don't put this->isNegEdge() on label
string
FddNodeV::getLabel() const
{
   if (isTerminal())
      return "One";

   ostringstream str;
   str << getFddNodeVInt();

   return str.str();
}

