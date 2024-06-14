/****************************************************************************
  FileName     [ bddMgr.cpp ]
  PackageName  [ ]
  Synopsis     [ BDD Manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iomanip>
#include <cassert>
#include "bddNode.h"
// #include "fddNode.h"
#include "bddMgr.h"

using namespace std;

//----------------------------------------------------------------------
//    Global variables
//----------------------------------------------------------------------
BddMgr* bddMgr = new BddMgr;

//----------------------------------------------------------------------
//    External functions
//----------------------------------------------------------------------
bool myStr2Int(const string&, int&);

//----------------------------------------------------------------------
//    static functions
//----------------------------------------------------------------------
static void swapBddNode(BddNode& f, BddNode& g)
{
   BddNode tmp = f; f = g; g = tmp;
}

//----------------------------------------------------------------------
//    helper functions
//----------------------------------------------------------------------
BddNode getBddNode(const string& bddName)
{
   int id;
   if (myStr2Int(bddName, id))  // bddName is an ID
      return bddMgr->getBddNode(id);
   else // bddNameis a name
      return bddMgr->getBddNode(bddName);
}

//----------------------------------------------------------------------
//    class BddMgr
//----------------------------------------------------------------------
// _level = 0 ==> const 1 & const 0
// _level = 1 ~ nin ==> supports
//
void
BddMgr::init(size_t nin, size_t h, size_t c)
{
   reset();
   _uniqueTable.init(h);
   _fdduniqueTable.init(h);
   _computedTable.init(c);

   // This must be called first
   BddNode::setBddMgr(this);
   FddNode::setBddMgr(this);
   BddNodeInt::_terminal = uniquify(0, 0, 0);
   FddNodeInt::_terminal = fdduniquify(0, 0, 0);
   BddNode::_one = BddNode(BddNodeInt::_terminal, BDD_POS_EDGE);
   BddNode::_zero = BddNode(BddNodeInt::_terminal, BDD_NEG_EDGE);
   FddNode::_one = FddNode(FddNodeInt::_terminal, FDD_POS_EDGE);
   FddNode::_zero = FddNode(FddNodeInt::_terminal, FDD_NEG_EDGE);


   _supports.reserve(nin+1);
   _fddsupports.reserve(nin+1);
   _supports.push_back(BddNode::_one);
   _fddsupports.push_back(FddNode::_one);
   for (size_t i = 1; i <= nin; ++i){
      _supports.push_back(BddNode(BddNode::_one(), BddNode::_zero(), i));
      _fddsupports.push_back(FddNode(FddNode::_one(), FddNode::_zero(), i));
   }
}

// Called by the CIRSETVar command
void
BddMgr::restart()
{
   size_t nin = _supports.size() - 1;
   size_t h   = _uniqueTable.numBuckets();
   size_t c   = _computedTable.size();

   init(nin, h, c);
}

// This is a private function called by init() and restart()
void
BddMgr::reset()
{
   // TODO
   _supports.clear();
   _fddsupports.clear();
   _bddArr.clear();
   _fddArr.clear();
   _bddMap.clear();
   _fddMap.clear();
   BddHash::iterator bi = _uniqueTable.begin();
   for (; bi != _uniqueTable.end(); ++bi)
      delete (*bi).second;
   _uniqueTable.reset();
   _fdduniqueTable.reset();
   _computedTable.reset();
}

// [Note] Remeber to check "isNegEdge" when return BddNode!!!!!
//
BddNode
BddMgr::ite(BddNode f, BddNode g, BddNode h)
{
   bool isNegEdge = false;  // should only be flipped by "standardize()"

#define DO_STD_ITE 1  // NOTE: make it '0' if you haven't done standardize()!!
   standardize(f, g, h, isNegEdge);

   BddNode ret;

   // check terminal cases
   if (checkIteTerminal(f, g, h, ret)) {
      if (isNegEdge) ret = ~ret;
      return ret;  // no need to update tables
   }

   // check computed table
   // TODO: based on your definition of BddCacheKey,
   //       instantiate a BddCacheKey k (i.e. pass in proper data members)
   // BddCacheKey k;  // Change this line!!
   BddCacheKey k(f(), g(), h());
   size_t ret_t;
   if (_computedTable.read(k, ret_t)) {
      if (isNegEdge) ret_t = ret_t ^ BDD_NEG_EDGE;
      return ret_t;
   }

   // check top varaible
   unsigned v = f.getLevel();
   if (g.getLevel() > v)
      v = g.getLevel();
   if (h.getLevel() > v)
      v = h.getLevel();

   // recursion
   BddNode fl = f.getLeftCofactor(v),
           gl = g.getLeftCofactor(v),
           hl = h.getLeftCofactor(v);
   BddNode t = ite(fl, gl, hl);

   BddNode fr = f.getRightCofactor(v),
           gr = g.getRightCofactor(v),
           hr = h.getRightCofactor(v);
   BddNode e = ite(fr, gr, hr);

   // get result
   if (t == e) {
      // update computed table
      _computedTable.write(k, t());
      if (isNegEdge) t = ~t;
      return t;
   }

   // move bubble if necessary... ==> update isNedEdge
   bool moveBubble = t.isNegEdge();
#if DO_STD_ITE
   assert(!moveBubble);
#else
   if (moveBubble) {
      t = ~t; e = ~e;
   }
#endif

   // check unique table
   BddNodeInt* ni = uniquify(t(), e(), v);
   ret_t = size_t(ni);
#if !(DO_STD_ITE)
   if (moveBubble) ret_t = ret_t ^ BDD_NEG_EDGE;
#endif
   // update computed table
   _computedTable.write(k, ret_t);
   if (isNegEdge)
      ret_t = ret_t ^ BDD_NEG_EDGE;
   return ret_t;
}

void
BddMgr::standardize(BddNode &f, BddNode &g, BddNode &h, bool &isNegEdge)
{
   // TODO
   // (1) Identical/Complement rules
   if (f == g) g = BddNode::_one;
   else if (f == ~g) g = BddNode::_zero;
   else if (f == h) h = BddNode::_zero;
   else if (f == ~h) h = BddNode::_one;

   // (2) Symmetrical rules
   if (g == BddNode::_one) {
      if (f > h) swapBddNode(f, h);
   }
   else if (g == BddNode::_zero) {
      if (f > h) { swapBddNode(f, h); f = ~f; h = ~h; }
   }
   else if (h == BddNode::_one) {
      if (f > g) { swapBddNode(f, g); f = ~f; g = ~g; }
   }
   else if (h == BddNode::_zero) {
      if (f > g) swapBddNode(f, g);
   }
   else if (g == ~h) {
      if (f > g) { swapBddNode(f, g); h = ~g; }
   }

   // (3) Complement edge rules
   // ==> both f and g will be posEdge afterwards
   if (f.isNegEdge()) { swapBddNode(g, h); f = ~f; }
   if (g.isNegEdge()) {
      g = ~g; h = ~h; isNegEdge = !isNegEdge;
   }
}

// Check if triplet (l, r, i) is in _uniqueTable,
// If not, create a new node;
// else, return the hashed one
//
BddNodeInt*
BddMgr::uniquify(size_t l, size_t r, unsigned i)
{
   // TODO
   BddNodeInt* n = 0;
   BddHashKey k(l, r, i);
   if (!_uniqueTable.check(k, n)) {
      n = new BddNodeInt(l, r, i);
      _uniqueTable.forceInsert(k, n);
   }
   return n;
}

// Check if triplet (l, r, i) is in _fdduniqueTable,
// If not, create a new node;
// else, return the hashed one
//
FddNodeInt*
BddMgr::fdduniquify(size_t l, size_t r, unsigned i)
{
   // TODO
   FddNodeInt* n = 0;
   BddHashKey k(l, r, i);
   if (!_fdduniqueTable.check(k, n)) {
      n = new FddNodeInt(l, r, i);
      _fdduniqueTable.forceInsert(k, n);
   }
   return n;
}


// return false if _bddArr[id] has aleady been inserted
bool
BddMgr::addBddNode(unsigned id, size_t n)
{
   if (id >= _bddArr.size()) {
      unsigned origSize = _bddArr.size();
      _bddArr.resize(id+1);
      for(unsigned i = origSize; i < _bddArr.size(); ++i)
        _bddArr[i] = 0;
   } else if (_bddArr[id] != 0)
      return false;
   _bddArr[id] = n;
   return true;
}
// return false if _bddArr[id] has aleady been inserted
bool
BddMgr::addFddNode(unsigned id, size_t n)
{
   if (id >= _fddArr.size()) {
      unsigned origSize = _fddArr.size();
      _fddArr.resize(id+1);
      for(unsigned i = origSize; i < _fddArr.size(); ++i)
        _fddArr[i] = 0;
   } else if (_fddArr[id] != 0)
      return false;
   _fddArr[id] = n;
   return true;
}

// return 0 if not in the map!!
BddNode
BddMgr::getBddNode(unsigned id) const
{
   if (id >= _bddArr.size())
      return size_t(0);
   return _bddArr[id];
}

// return 0 if not in the map!!
FddNode
BddMgr::getFddNode(unsigned id) const
{
   if (id >= _fddArr.size())
      return size_t(0);
   return _fddArr[id];
}

// return false if str is already in the _bddMap!!
bool
BddMgr::addBddNode(const string& str, size_t n)
{
   return _bddMap.insert(BddMapPair(str, n)).second;
}
// return false if str is already in the _fddMap!!
bool
BddMgr::addFddNode(const string& str, size_t n)
{
   return _fddMap.insert(BddMapPair(str, n)).second;
}

void
BddMgr::forceAddBddNode(const string& str, size_t n)
{
   _bddMap[str] = n;
}
void
BddMgr::forceAddFddNode(const string& str, size_t n)
{
   _fddMap[str] = n;
}

// return 0 if not in the map!!
BddNode
BddMgr::getBddNode(const string& name) const
{
   BddMapConstIter bi = _bddMap.find(name);
   if (bi == _bddMap.end()) return size_t(0);
   return (*bi).second;
}
// return 0 if not in the map!!
FddNode
BddMgr::getFddNode(const string& name) const
{
   BddMapConstIter bi = _fddMap.find(name);
   if (bi == _fddMap.end()) return size_t(0);
   return (*bi).second;
}

// return true if terminal case
bool
BddMgr::checkIteTerminal
(const BddNode& f, const BddNode& g, const BddNode& h, BddNode& n)
{
   // TODO
   if (g == h) { n = g; return true; }
   if (f == BddNode::_one) { n = g; return true; }
   if (f == BddNode::_zero) { n = h; return true; }
   if (g == BddNode::_one && h == BddNode::_zero) { n = f; return true; }
   return false;
}

//----------------------------------------------------------------------
//    Application functions
//----------------------------------------------------------------------
// pattern[0 ~ n-1] ==> _support[1 ~ n]
//
// ==> return -1 if pattern is not legal
// ==> return 0/1 for evaluated result
int 
BddMgr::evalCube(const BddNode& node, const string& pattern) const
{
   // TODO
   size_t v = node.getLevel();
   size_t n = pattern.size();
   if (n < v) {
      cerr << "Error: " << pattern << " too short!!" << endl;
      return -1;
   }

   BddNode next = node;
   for (int i = v - 1; i >= 0; --i) {
      char c = pattern[i];
      if (c == '1')
         next = next.getLeftCofactor(i+1);
      else if (c == '0')
         next = next.getRightCofactor(i+1);
      else {
         cerr << "Illegal pattern: " << c << "(" << i << ")" << endl;
         return -1;
      }
   }
   return (next == BddNode::_one)? 1 : 0;
}

bool
BddMgr::drawBdd(const string& name, const string& fileName) const
{
   // TODO
   BddNode node = ::getBddNode(name);
   if (node() == 0) {
      cerr << "Error: \"" << name << "\" is not a legal BDD node!!" << endl;
      return false;
   }

   ofstream ofile(fileName.c_str());
   if (!ofile) {
      cerr << "Error: cannot open file \"" << fileName << "\"!!" << endl;
      return false;
   }

   node.drawBdd(name, ofile);

   return true;
}


FddNode
BddMgr::fddNot(const FddNode& f)
{
   if(f == FddNode::_one) return FddNode::_zero;
   if(f == FddNode::_zero) return FddNode::_one;
   size_t ret;
   unsigned v = f.getLevel();
   FddNode l = f.getLeft();
   FddNode r = f.getRight();

   if(r == FddNode::_one) r = FddNode::_zero;
   else if(r == FddNode::_zero) r = FddNode::_one;
   else r = fddNot(r);

   if(l == FddNode::_zero) return r;

   ret = size_t(fdduniquify(l(), r(), v));
   return ret;
}

FddNode
BddMgr::fddXor(const FddNode& f, const FddNode& g)
{
   // cout<<endl<< f.getLevel() << " " << g.getLevel()<<endl;

   if(f == FddNode::_zero) {
      // cout << "f == 0" << endl;
      return g;
   }
   // cout << "f != 0" << endl;
   if(g == FddNode::_zero) {
      // cout << "g == 0" << endl;
      return f;
   }
   // cout << "g != 0" << endl;
   if(f == FddNode::_one) {
      // cout << "f == 1" << endl;
      return ~g;
   }
   // cout << "f != 1" << endl;
   if(g == FddNode::_one) {
      // cout << "g == 1" << endl;
      return ~f;
   }
   // cout << "g != 1" << endl;

   
   unsigned v = f.getLevel() > g.getLevel() ? f.getLevel() : g.getLevel();
   // cout<<"test"<<endl;
   FddNode fl = (v == f.getLevel())? f.getLeft():FddNode::_zero,
           gl = (v == g.getLevel())? g.getLeft():FddNode::_zero;
   FddNode l = fddXor(fl, gl);
   FddNode fr = f.getRightCofactor(v),
           gr = g.getRightCofactor(v); 
   FddNode r = fddXor(fr, gr);
   if(l == FddNode::_zero) return r;
   // if(l == r) l = FddNode::_one;

   FddNodeInt* ni = fdduniquify(l(), r(), v);
   return size_t(ni);
}

FddNode
BddMgr::fddOr(const FddNode& f, const FddNode& g)
{
   // cout << endl << f.getLevel() << " " << g.getLevel() << endl;
   if(f == FddNode::_one) return FddNode::_one;
   // cout << "f != 1" << endl;
   if(g == FddNode::_one) return FddNode::_one;
   // cout << "g != 1" << endl;
   if(f == FddNode::_zero) return g;
   // cout << "f != 0" << endl;
   if(g == FddNode::_zero) return f;
   // cout << "g != 0" << endl;
   
   unsigned v = f.getLevel() > g.getLevel() ? f.getLevel() : g.getLevel();
   // cout << "testl" << endl;
   FddNode fd = (v == f.getLevel())? f.getLeft():FddNode::_zero,
           gd = (v == g.getLevel())? g.getLeft():FddNode::_zero;

   // cout << "testr" << endl;
   FddNode fn = (v == f.getLevel())? f.getRight():f,
           gn = (v == g.getLevel())? g.getRight():g;

   FddNode fp = fd^fn,
           gp = gd^gn;


   FddNode r = fddOr(fn, gn);

   FddNode p = fddOr(fp, gp);

   FddNode l = fddXor(r,p);


   if(l == FddNode::_zero) return r;

   FddNodeInt* ni = fdduniquify(l(), r(), v);
   return size_t(ni);
}

// FddNode
// BddMgr::fddAnd(const FddNode& f, const FddNode& g)
// {
   
// }

FddNode
BddMgr::Bdd2Fdd(const BddNode& n){

   //edge case
   // cout<<"enter Bdd2Fdd"<<endl;
   // cout<<"n: "<<endl<<n<<endl;
   if(n == BddNode::_zero){
      return FddNode::_zero; 
   }
   if(n == BddNode::_one){
      return FddNode::_one;
   }
   BddNode bl = n.getLeft(),
           br = n.getRight();

   FddNode fl = Bdd2Fdd(bl),
           fr = Bdd2Fdd(br);
   FddNode fd = fl^fr;
   if(n.isNegEdge()) fr = ~fr;
   if(fd == FddNode::_zero) return fr;

   unsigned v = n.getLevel();
   FddNodeInt* ni = fdduniquify(fd(), fr(), v);
   return size_t(ni);
}

BddNode
BddMgr::Fdd2Bdd(const FddNode& n){
   //edge case
   // cout<<"enter Bdd2Fdd"<<endl;
   // cout<<"n: "<<endl<<n<<endl;
   if(n == FddNode::_zero){
      // cout<<"n==0"<<endl;
      return BddNode::_zero; 
   }
   // cout<<"n!=0"<<endl;
   if(n == FddNode::_one){
      // cout<<"n==1"<<endl;
      return BddNode::_one;
   }
   // cout<<"n!=1"<<endl;
   FddNode fd = n.getLeft(),
           fr = n.getRight();
   FddNode fl = fd^fr;
   BddNode bl = Fdd2Bdd(fl),
           br = Fdd2Bdd(fr);

   BddNode v = getSupport(n.getLevel());
   return ite(v, bl, br);

}