/****************************************************************************
  FileName     [ FddNodeV.h ]
  PackageName  [ ]
  Synopsis     [ Define basic BDD Node data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef FDD_NODEV_H
#define FDD_NODEV_H

#include <vector>
#include <map>
#include <iostream>

using namespace std;

#define BDD_EDGE_BITS      2
//#define BDD_NODE_PTR_MASK  ((UINT_MAX >> BDD_EDGE_BITS) << BDD_EDGE_BITS)
#define BDD_NODE_PTR_MASK  ((~(size_t(0)) >> BDD_EDGE_BITS) << BDD_EDGE_BITS)

class BddMgrV;
class FddNodeVInt;
class BddNodeV;

enum FDD_EDGE_FLAG
{
   FDD_POS_EDGE = 0,
   FDD_NEG_EDGE = 1,

   FDD_EDGE_DUMMY  // dummy end
};

class FddNodeV
{
public:
   static FddNodeV          _one;
   static FddNodeV          _zero;
   static bool             _debugBddAddr;
   static bool             _debugRefCount;

   // no node association yet
   FddNodeV() : _node(0) {}
   // We check the hash when a new node is possibly being created
   FddNodeV(size_t l, size_t r, size_t i, FDD_EDGE_FLAG f = FDD_POS_EDGE);
   // Copy constructor also needs to increase the _refCount
   FddNodeV(const FddNodeV& n);
   // n must have been uniquified...
   FddNodeV(FddNodeVInt* n, FDD_EDGE_FLAG f = FDD_POS_EDGE);
   // 
   FddNodeV(size_t v);
   // transform bdd to fdd
   FddNodeV(const BddNodeV& n);
   // Destructor is the only place to decrease _refCount
   ~FddNodeV();

   // Basic access functions
   const FddNodeV& getLeft() const;
   const FddNodeV& getRight() const;
   FddNodeV getLeftCofactor(unsigned i) const;
   FddNodeV getRightCofactor(unsigned i) const;
   // FddNodeV getDiffCofactor(unsigned i) const;
   unsigned getLevel() const;
   unsigned getRefCount() const;
   bool isNegEdge() const { return (_node & FDD_NEG_EDGE); }
   bool isPosEdge() const { return !isNegEdge(); }

   // Operators overloading
   size_t operator () () const { return _node; }
   // FddNodeV operator ~ () const { return (_node ^ BDD_NEG_EDGE); }
   FddNodeV operator ~ () const;
   FddNodeV& operator = (const FddNodeV& n);
   FddNodeV operator & (const FddNodeV& n) const;
   FddNodeV& operator &= (const FddNodeV& n);
   FddNodeV operator | (const FddNodeV& n) const;
   FddNodeV& operator |= (const FddNodeV& n);
   FddNodeV operator ^ (const FddNodeV& n) const;
   FddNodeV& operator ^= (const FddNodeV& n);
   bool operator == (const FddNodeV& n) const { return (_node == n._node); }
   bool operator != (const FddNodeV& n) const { return (_node != n._node); }
   bool operator < (const FddNodeV& n) const;
   bool operator <= (const FddNodeV& n) const;
   bool operator > (const FddNodeV& n) const { return !((*this) <= n); }
   bool operator >= (const FddNodeV& n) const { return !((*this) < n); }

   // Other BDD operations
   FddNodeV exist(unsigned l) const;
   FddNodeV nodeMove(unsigned fLevel, unsigned tLevel, bool& isMoved) const;
   size_t countCube() const;
   FddNodeV getCube(size_t ith=0) const;
   vector<FddNodeV> getAllCubes() const;
   string toString() const;


   friend ostream& operator << (ostream& os, const FddNodeV& n);

   // For BDD drawing
   void drawFdd(const string&, ofstream&) const;
   string getLabel() const;

   // Static functions
   static void setBddMgrV(BddMgrV* m) { _BddMgrV = m; }

   //test
   void print(){cout<<_node<<endl;}
   
private:
   size_t                  _node;

   // Static data mebers
   static BddMgrV*          _BddMgrV;

   // Private functions
   FddNodeVInt* getFddNodeVInt() const {
      return (FddNodeVInt*)(_node & BDD_NODE_PTR_MASK); }
   bool isTerminal() const;
   void print(ostream&, size_t, size_t&) const;
   void unsetVisitedRecur() const;
   void drawFddRecur(ofstream&) const;
   // comment out for SoCV BDD
   FddNodeV existRecur(unsigned l, map<size_t, size_t>&) const;
   FddNodeV nodeMoveRecur(unsigned f, unsigned t, map<size_t, size_t>&) const;
   bool containNode(unsigned b, unsigned e) const;
   bool containNodeRecur(unsigned b, unsigned e) const;
   size_t countCubeRecur(bool phase, map<size_t, size_t>& numCubeMap) const;
   bool getCubeRecur(bool p, size_t& ith, size_t target, FddNodeV& res) const;
   void getAllCubesRecur(bool p, FddNodeV& c, vector<FddNodeV>& aCubes) const;
   bool toStringRecur(bool p, string& str) const;
};

// Private class
class FddNodeVInt
{
   friend class FddNodeV;
   friend class BddMgrV;

   // For const 1 and const 0
   FddNodeVInt() : _level(0), _refCount(0), _visited(0) {}

   // Don't initialize _refCount here...
   // FddNodeV() will call incRefCount() or decRefCount() instead...
   FddNodeVInt(size_t l, size_t r, unsigned ll)
   : _left(l), _right(r), _level(ll), _refCount(0), _visited(0) {}

   const FddNodeV& getLeft() const { return _left; }
   const FddNodeV& getRight() const { return _right; }
   unsigned getLevel() const { return _level; }
   unsigned getRefCount() const { return _refCount; }
   void incRefCount() { ++_refCount; }
   void decRefCount() { --_refCount; }
   bool isVisited() const { return (_visited == 1); }
   void setVisited() { _visited = 1; }
   void unsetVisited() { _visited = 0; }

   FddNodeV              _left;
   FddNodeV              _right;
   unsigned             _level    : 16;
   unsigned             _refCount : 15;
   unsigned             _visited  : 1;

   static FddNodeVInt*   _terminal;
};

#endif // FDD_NODE_H
