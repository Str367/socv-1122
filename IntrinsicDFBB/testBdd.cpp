/****************************************************************************
  FileName     [ testBdd.cpp ]
  PackageName  [ ]
  Synopsis     [ Define main() ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "bddNode.h"
#include "fddNode.h"
#include "bddMgr.h"

using namespace std;

/**************************************************************************/
/*                        Define Global BDD Manager                       */
/**************************************************************************/
BddMgr bm;

/**************************************************************************/
/*                    Define Static Function Prototypes                   */
/**************************************************************************/
static void initBdd(size_t nSupports, size_t hashSize, size_t cacheSize);


/**************************************************************************/
/*                             Define main()                              */
/**************************************************************************/
int
main()
{
   initBdd(5, 127, 61);

      /*-------- THIS IS JUST A TEST CODE ---------*/
   BddNode b1(bm.getSupport(1));
   BddNode b2(bm.getSupport(2));
   BddNode b3(bm.getSupport(3));
   BddNode b4(bm.getSupport(4));
   BddNode b5(bm.getSupport(5));
   FddNode f1(bm.getFddSupport(1));
   FddNode f2(bm.getFddSupport(2));
   FddNode f3(bm.getFddSupport(3));
   FddNode f4(bm.getFddSupport(4));
   FddNode f5(bm.getFddSupport(5));

   BddNode f = ~(b1 & b2);
   cout << f << endl;

   BddNode g = b3 | b4;
   cout << g << endl;

   BddNode h = ~b5;
   cout << h << endl;

   BddNode i = f ^ (b3 | b4); // f ^ g;
   cout << i << endl;

   FddNode j = ~f1 | ~f2;
   cout << j << endl;

   cout << "KK" << endl;
   BddNode bk = ( (b1|b2) ^ (b4&b5) );
   FddNode fk = ( (f1|f2) ^ (f4&f5) );
   cout << fk << endl;
//   cout << k.getLeftCofactor(1) << endl;
   cout << fk.getLeftCofactor(2) << endl;
//   cout << k.getRightCofactor(4) << endl;
   cout << fk.getRightCofactor(2) << endl;
   BddNode l1 = bk.exist(2);
   cout << l1 << endl;
   FddNode l2 = fk.getLeftCofactor(2) | fk.getRightCofactor(2);
   cout << l2 << endl;


   FddNode fl(l1);
   if(fl == l2)
      cout << "fl == l2" << endl;

   ofstream ofile("fl.dot");
   fl.drawFdd("fl", ofile);
   system("dot -o fl.png -Tpng fl.dot");

   /*----------- END OF TEST CODE ------------*/
}


/**************************************************************************/
/*                          Define Static Functions                       */
/**************************************************************************/
static void
initBdd(size_t nin, size_t h, size_t c)
{
   BddNode::_debugBddAddr = true;
   BddNode::_debugRefCount = true;
   FddNode::_debugBddAddr = true;
   FddNode::_debugRefCount = true;

//   bm.reset();
   bm.init(nin, h, c);
}

