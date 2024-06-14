/****************************************************************************
  FileName     [ fddNode.cpp ]
  PackageName  [ ]
  Synopsis     [ Define BDD Node member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <sstream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include "bddMgr.h"

// Initialize static data members
//
BddMgr* FddNode::_BddMgr = 0;
FddNodeInt* FddNodeInt::_terminal = 0;
FddNode FddNode::_one;
FddNode FddNode::_zero;
bool FddNode::_debugBddAddr = false;
bool FddNode::_debugRefCount = false;

// We check the hash when a new FddNodeInt is possibly being created
FddNode::FddNode(size_t l, size_t r, size_t i, FDD_EDGE_FLAG f)
{
   FddNodeInt* n = _BddMgr->fdduniquify(l, r, i);
   // n should not = 0
   assert(n != 0);
   _node = size_t(n) + f;
   n->incRefCount();
}

// Copy constructor also needs to increase the _refCount
// Need to check if n._node != 0
FddNode::FddNode(const FddNode& n) : _node(n._node)
{
   FddNodeInt* t = getFddNodeInt();
   if (t)
      t->incRefCount();
}

// This function is called only when n is obtained from uniquify()...
// ==> n should not be "0"
FddNode::FddNode(FddNodeInt* n, FDD_EDGE_FLAG f)
{
   assert(n != 0);
   // TODO
   _node = size_t(n) + f;
   n->incRefCount();
}

// Need to check if n._node != 0
FddNode::FddNode(size_t v) : _node(v)
{
   // TODO
   FddNodeInt* n = getFddNodeInt();
   if (n)
      n->incRefCount();
}

FddNode::FddNode(const BddNode& n)
{
   // TODO
   FddNode f = _BddMgr->Bdd2Fdd(n);
   _node = f();
   FddNodeInt* t = getFddNodeInt();
   if(t)
      t->incRefCount();
}

// Need to check if _node != 0
FddNode::~FddNode()
{
   // TODO
   FddNodeInt* n = getFddNodeInt();
   if (n)
      n->decRefCount();
}

const FddNode&
FddNode::getLeft() const
{
   assert(getFddNodeInt() != 0);
   return getFddNodeInt()->getLeft();
}

const FddNode&
FddNode::getRight() const
{
   assert(getFddNodeInt() != 0);
   return getFddNodeInt()->getRight();
}

// [Note] i SHOULD NOT < getLevel()
FddNode
FddNode::getLeftCofactor(unsigned i) const
{
   assert(getFddNodeInt() != 0);
   assert(i > 0);
   // TODO
   if (i > getLevel()) return (*this);
   if (i == getLevel())
      // return isNegEdge()? ~getLeft()^getRight() : getLeft()^getRight();
      return getLeft()^getRight();
   FddNode t = getLeft().getLeftCofactor(i);
   FddNode e = getRight().getLeftCofactor(i);
   if (t == e) return isNegEdge()? ~t : t;
   FDD_EDGE_FLAG f = (isNegEdge() ^ t.isNegEdge())?
                      FDD_NEG_EDGE: FDD_POS_EDGE;
   if (t.isNegEdge()) { t = ~t; e = ~e; }
   return FddNode(t(), e(), getLevel(), f);
}

// [Note] i SHOULD NOT < getLevel()
FddNode
FddNode::getRightCofactor(unsigned i) const
{
   assert(getFddNodeInt() != 0);
   assert(i > 0);
   // TODO
   if (i > getLevel()) return (*this);
   if (i == getLevel())
      return isNegEdge()? ~getRight() : getRight();
   FddNode t = getLeft().getRightCofactor(i);
   FddNode e = getRight().getRightCofactor(i);
   if (t == e) return isNegEdge()? ~t : t;
   FDD_EDGE_FLAG f = (isNegEdge() ^ t.isNegEdge())?
                      FDD_NEG_EDGE: FDD_POS_EDGE;
   if (t.isNegEdge()) { t = ~t; e = ~e; }
   return FddNode(t(), e(), getLevel(), f);
}

// [Note] i SHOULD NOT < getLevel()
// FddNode
// FddNode::getDiffCofactor(unsigned i) const
// {
//    assert(getFddNodeInt() != 0);
//    assert(i > 0);
//    // TODO
//    if (i > getLevel()) return (FddNode::_zero);
//    if (i == getLevel())
//       // return isNegEdge()? ~getRight() : getRight();
//       return  getLeft();
//    FddNode t = getLeft().getRightCofactor(i);
//    FddNode e = getRight().getRightCofactor(i);
//    if (t == e) return isNegEdge()? ~t : t;
//    FDD_EDGE_FLAG f = (isNegEdge() ^ t.isNegEdge())?
//                       FDD_NEG_EDGE: FDD_POS_EDGE;
//    if (t.isNegEdge()) { t = ~t; e = ~e; }
//    return FddNode(t(), e(), getLevel(), f);
// }

unsigned
FddNode::getLevel() const
{
   return getFddNodeInt()->getLevel();
}

unsigned
FddNode::getRefCount() const
{
   return getFddNodeInt()->getRefCount();
}

// Note: FddNode a = b;
// ==> a's original FddNodeInt's reference count should --
//     a's new FddNodeInt's reference count should ++
FddNode&
FddNode::operator = (const FddNode& n)
{
   // TODO
   FddNodeInt* t = getFddNodeInt();
   if (t)
      t->decRefCount();
   _node = n._node;
   t = getFddNodeInt();
   if (t)
      t->incRefCount();
   return (*this);
}

FddNode
FddNode::operator ~ () const
{
   // TODO
   return _BddMgr->fddNot((*this));
}

FddNode
FddNode::operator & (const FddNode& n) const
{
   // TODO
   FddNode tmp = _BddMgr->fddOr(~(*this),~n);
   return ~tmp;
   // return _BddMgr->fddAnd((*this), n);
   // return _BddMgr->ite((*this), n, FddNode::_zero);
}

FddNode&
FddNode::operator &= (const FddNode& n)
{
   // TODO
   (*this) = (*this) & n;
   return (*this);
}

FddNode
FddNode::operator | (const FddNode& n) const
{
   // TODO
   return _BddMgr->fddOr((*this), n);
   // return _BddMgr->ite((*this), FddNode::_one, n);
}

FddNode&
FddNode::operator |= (const FddNode& n)
{
   // TODO
   (*this) = (*this) | n;
   return (*this);
}

FddNode
FddNode::operator ^ (const FddNode& n) const
{
   // TODO
   // cout<<"xor"<<endl;
   return _BddMgr->fddXor((*this), n);
   // return _BddMgr->fddXor((*this), n);
   // return _BddMgr->ite((*this), ~n, n);
}

FddNode&
FddNode::operator ^= (const FddNode& n)
{
   // TODO
   (*this) = (*this) ^ n;
   return (*this);
}

// (*this) < n iff
// (i) level < n.level
// (ii) level == n.level && _node < n._node
bool
FddNode::operator < (const FddNode& n) const
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
FddNode::operator <= (const FddNode& n) const
{
   // TODO
   unsigned l1 = getLevel();
   unsigned l2 = n.getLevel();
   return ((l1 < l2) ||
          ((l1 == l2) && (_node <= n._node)));
}

bool
FddNode::isTerminal() const
{
   return (getFddNodeInt() == FddNodeInt::_terminal);
}

// [BONUS TODO]... starting
//
FddNode
FddNode::exist(unsigned l) const
{
   if (l == 0) return (*this);

   map<size_t, size_t> existMap;
   return existRecur(l, existMap);
}

FddNode
FddNode::existRecur(unsigned l, map<size_t, size_t>& existMap) const
{
   if (isTerminal()) return (*this);

   unsigned thisLevel = getLevel();
   if (l > thisLevel) return (*this);

   map<size_t, size_t>::iterator mi = existMap.find(_node);
   if (mi != existMap.end()) return (*mi).second;

   FddNode left = getLeftCofactor(thisLevel);
   FddNode right = getRightCofactor(thisLevel);
   if (l == thisLevel) {
      FddNode res = left | right;
      existMap[_node] = res();
      return res;
   }

   // l must < getLevel()
   bool isNegEdge = false;
   FddNode t = left.existRecur(l, existMap);
   FddNode e = right.existRecur(l, existMap);
   if (t == e) {
      existMap[_node] = t();
      return t;
   }
   if (t.isNegEdge()) {
      t = ~t; e = ~e; isNegEdge = true;
   }
   FddNode res(_BddMgr->fdduniquify(t(), e(), thisLevel));
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
// FddNode
// FddNode::nodeMove(unsigned fromLevel, unsigned toLevel, bool& isMoved) const
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

// FddNode
// FddNode::nodeMoveRecur
// (unsigned fromLevel, unsigned toLevel, map<size_t, size_t>& moveMap) const
// {
//    unsigned thisLevel = getLevel();
//    assert(thisLevel >= fromLevel);

//    map<size_t, size_t>::iterator mi = moveMap.find(_node);
//    if (mi != moveMap.end()) return (*mi).second;

//    FddNode left = getLeft();
//    FddNode right = getRight();

//    if (!left.isTerminal())
//       left = left.nodeMoveRecur(fromLevel, toLevel, moveMap);
//    if (!right.isTerminal())
//       right = right.nodeMoveRecur(fromLevel, toLevel, moveMap);

//    FddNodeInt *n
//    = _BddMgr->fdduniquify(left(), right(), thisLevel - fromLevel + toLevel);
//    FddNode ret = FddNode(size_t(n));
//    if (isNegEdge()) ret = ~ret;

//    moveMap[_node] = ret();
//    return ret;
// }

// // Check if there is any FddNode in the cone of level [bLevel, eLevel]
// // return true if any
// //
// bool
// FddNode::containNode(unsigned bLevel, unsigned eLevel) const
// {
//    bool res = containNodeRecur(bLevel, eLevel);
//    unsetVisitedRecur();
//    return res;
// }

// bool
// FddNode::containNodeRecur(unsigned bLevel, unsigned eLevel) const
// {
//    FddNodeInt* n = getFddNodeInt();
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
// FddNode::countCube() const
// {  
//    map<size_t, size_t> numCubeMap;
//    return countCubeRecur(false, numCubeMap);
// }  

// size_t
// FddNode::countCubeRecur(bool phase, map<size_t, size_t>& numCubeMap) const
// {
//    if (isTerminal())
//       return ((phase ^ isNegEdge())? 0 : 1); 

//    map<size_t, size_t>::iterator mi = numCubeMap.find(_node);
//    if (mi != numCubeMap.end()) return (*mi).second;

//    unsigned numCube = 0;
//    FddNode left = getLeft();
//    numCube += left.countCubeRecur(phase ^ isNegEdge(), numCubeMap);
//    FddNode right = getRight();
//    numCube += right.countCubeRecur(phase ^ isNegEdge(), numCubeMap);

//    numCubeMap[_node] = numCube;
//    return numCube;
// }

// FddNode
// FddNode::getCube(size_t ith) const
// {  
//    ith %= countCube();
//    FddNode res = FddNode::_one;
//    size_t idx = 0;
//    getCubeRecur(false, idx, ith, res);
//    return res;
// }
   
// // return true if the target-th cube is met
// bool
// FddNode::getCubeRecur
// (bool phase, size_t& ith, size_t target, FddNode& res) const
// {
//    if (isTerminal()) {
//       if (!(phase ^ isNegEdge())) {
//          if (ith == target) return true;
//          ++ith;
//       }
//       return false;
//    }

//    FddNode old = res;
//    FddNode left = getLeft();
//    res = old & _BddMgr->getFddSupport(getLevel());
//    if (left.getCubeRecur(phase ^ isNegEdge(), ith, target, res))
//       return true;
//    FddNode right = getRight();
//    res = old & ~(_BddMgr->getFddSupport(getLevel()));
//    if (right.getCubeRecur(phase ^ isNegEdge(), ith, target, res))
//       return true;

//    return false;
// }

// vector<FddNode>
// FddNode::getAllCubes() const
// {
//    vector<FddNode> allCubes;
//    FddNode cube = FddNode::_one;
//    getAllCubesRecur(false, cube, allCubes);
//    return allCubes;
// }

// void
// FddNode::getAllCubesRecur
// (bool phase, FddNode& cube, vector<FddNode>& allCubes) const
// {
//    if (isTerminal()) {
//       if (!(phase ^ isNegEdge()))
//          allCubes.push_back(cube);
//       return;
//    }

//    FddNode old = cube;
//    FddNode left = getLeft();
//    cube = old & _BddMgr->getFddSupport(getLevel());
//    left.getAllCubesRecur(phase ^ isNegEdge(), cube, allCubes);
//    FddNode right = getRight();
//    cube = old & ~(_BddMgr->getFddSupport(getLevel()));
//    right.getAllCubesRecur(phase ^ isNegEdge(), cube, allCubes);
// }

// Assume this FddNode is a cube
string
FddNode::toString() const
{
   string str;
   toStringRecur(false, str);
   return str;
}

// return true if a cube is found
bool
FddNode::toStringRecur(bool phase, string& str) const
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
operator << (ostream& os, const FddNode& n)
{
   size_t nNodes = 0;
   n.print(os, 0, nNodes);
   n.unsetVisitedRecur();
   os << endl << endl << "==> Total #FddNodes : " << nNodes << endl;
   return os;
}

void
FddNode::print(ostream& os, size_t indent, size_t& nNodes) const
{
   for (size_t i = 0; i < indent; ++i)
      os << ' ';
   FddNodeInt* n = getFddNodeInt();
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
FddNode::unsetVisitedRecur() const
{
   FddNodeInt* n = getFddNodeInt();
   if (!n->isVisited()) return;
   n->unsetVisited();
   if (!isTerminal()) {
      n->getLeft().unsetVisitedRecur();
      n->getRight().unsetVisitedRecur();
   }
}

void
FddNode::drawFdd(const string& name, ofstream& ofile) const
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
FddNode::drawFddRecur(ofstream& ofile) const
{
   // TODO
   FddNodeInt* n = getFddNodeInt();
   if (n->isVisited())
      return;
   n->setVisited();
   if(isTerminal())
      return;
   FddNode left = getLeft();
   FddNode right = getRight();

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
FddNode::getLabel() const
{
   if (isTerminal())
      return "One";

   ostringstream str;
   str << getFddNodeInt();

   return str.str();
}

