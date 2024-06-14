/****************************************************************************
  FileName     [ fddNode.h ]
  PackageName  [ ]
  Synopsis     [ Define basic BDD Node data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef FDD_NODE_H
#define FDD_NODE_H

#include <vector>
#include <map>
#include <iostream>

using namespace std;

#define BDD_EDGE_BITS      2
//#define BDD_NODE_PTR_MASK  ((UINT_MAX >> BDD_EDGE_BITS) << BDD_EDGE_BITS)
#define BDD_NODE_PTR_MASK  ((~(size_t(0)) >> BDD_EDGE_BITS) << BDD_EDGE_BITS)

class BddMgr;
class FddNodeInt;
class BddNode;

enum FDD_EDGE_FLAG
{
   FDD_POS_EDGE = 0,
   FDD_NEG_EDGE = 1,

   FDD_EDGE_DUMMY  // dummy end
};

class FddNode
{
public:
   static FddNode          _one;
   static FddNode          _zero;
   static bool             _debugBddAddr;
   static bool             _debugRefCount;

   // no node association yet
   FddNode() : _node(0) {}
   // We check the hash when a new node is possibly being created
   FddNode(size_t l, size_t r, size_t i, FDD_EDGE_FLAG f = FDD_POS_EDGE);
   // Copy constructor also needs to increase the _refCount
   FddNode(const FddNode& n);
   // n must have been uniquified...
   FddNode(FddNodeInt* n, FDD_EDGE_FLAG f = FDD_POS_EDGE);
   // 
   FddNode(size_t v);
   // transform bdd to fdd
   FddNode(const BddNode& n);
   // Destructor is the only place to decrease _refCount
   ~FddNode();

   // Basic access functions
   const FddNode& getLeft() const;
   const FddNode& getRight() const;
   FddNode getLeftCofactor(unsigned i) const;
   FddNode getRightCofactor(unsigned i) const;
   // FddNode getDiffCofactor(unsigned i) const;
   unsigned getLevel() const;
   unsigned getRefCount() const;
   bool isNegEdge() const { return (_node & FDD_NEG_EDGE); }
   bool isPosEdge() const { return !isNegEdge(); }

   // Operators overloading
   size_t operator () () const { return _node; }
   // FddNode operator ~ () const { return (_node ^ BDD_NEG_EDGE); }
   FddNode operator ~ () const;
   FddNode& operator = (const FddNode& n);
   FddNode operator & (const FddNode& n) const;
   FddNode& operator &= (const FddNode& n);
   FddNode operator | (const FddNode& n) const;
   FddNode& operator |= (const FddNode& n);
   FddNode operator ^ (const FddNode& n) const;
   FddNode& operator ^= (const FddNode& n);
   bool operator == (const FddNode& n) const { return (_node == n._node); }
   bool operator != (const FddNode& n) const { return (_node != n._node); }
   bool operator < (const FddNode& n) const;
   bool operator <= (const FddNode& n) const;
   bool operator > (const FddNode& n) const { return !((*this) <= n); }
   bool operator >= (const FddNode& n) const { return !((*this) < n); }

   // Other BDD operations
   FddNode exist(unsigned l) const;
   // FddNode nodeMove(unsigned fLevel, unsigned tLevel, bool& isMoved) const;
   // size_t countCube() const;
   // FddNode getCube(size_t ith=0) const;
   // vector<FddNode> getAllCubes() const;
   string toString() const;



   friend ostream& operator << (ostream& os, const FddNode& n);

   // For BDD drawing
   void drawFdd(const string&, ofstream&) const;
   string getLabel() const;

   // Static functions
   static void setBddMgr(BddMgr* m) { _BddMgr = m; }

   //test
   void print(){cout<<_node<<endl;}
   
private:
   size_t                  _node;

   // Static data mebers
   static BddMgr*          _BddMgr;

   // Private functions
   FddNodeInt* getFddNodeInt() const {
      return (FddNodeInt*)(_node & BDD_NODE_PTR_MASK); }
   bool isTerminal() const;
   void print(ostream&, size_t, size_t&) const;
   void unsetVisitedRecur() const;
   void drawFddRecur(ofstream&) const;
   // comment out for SoCV BDD
   FddNode existRecur(unsigned l, map<size_t, size_t>&) const;
   // FddNode nodeMoveRecur(unsigned f, unsigned t, map<size_t, size_t>&) const;
   // bool containNode(unsigned b, unsigned e) const;
   // bool containNodeRecur(unsigned b, unsigned e) const;
   // size_t countCubeRecur(bool phase, map<size_t, size_t>& numCubeMap) const;
   // bool getCubeRecur(bool p, size_t& ith, size_t target, FddNode& res) const;
   // void getAllCubesRecur(bool p, FddNode& c, vector<FddNode>& aCubes) const;
   bool toStringRecur(bool p, string& str) const;
};

// Private class
class FddNodeInt
{
   friend class FddNode;
   friend class BddMgr;

   // For const 1 and const 0
   FddNodeInt() : _level(0), _refCount(0), _visited(0) {}

   // Don't initialize _refCount here...
   // FddNode() will call incRefCount() or decRefCount() instead...
   FddNodeInt(size_t l, size_t r, unsigned ll)
   : _left(l), _right(r), _level(ll), _refCount(0), _visited(0) {}

   const FddNode& getLeft() const { return _left; }
   const FddNode& getRight() const { return _right; }
   unsigned getLevel() const { return _level; }
   unsigned getRefCount() const { return _refCount; }
   void incRefCount() { ++_refCount; }
   void decRefCount() { --_refCount; }
   bool isVisited() const { return (_visited == 1); }
   void setVisited() { _visited = 1; }
   void unsetVisited() { _visited = 0; }

   FddNode              _left;
   FddNode              _right;
   unsigned             _level    : 16;
   unsigned             _refCount : 15;
   unsigned             _visited  : 1;

   static FddNodeInt*   _terminal;
};

#endif // FDD_NODE_H
