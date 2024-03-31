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
   initBdd(6, 127, 61);

   /*-------- THIS IS JUST A TEST CODE ---------*/
   BddNode a(bm.getSupport(1));
   BddNode b(bm.getSupport(2));
   BddNode c(bm.getSupport(3));
   BddNode d(bm.getSupport(4));
   BddNode e(bm.getSupport(5));
   BddNode f(bm.getSupport(6));


   BddNode g1 = b & d;

   BddNode g2 = ~c & e;

   BddNode g3 = a & b;
   
   BddNode g4 = c & g1;

   BddNode g5 = g1 | g2;

   BddNode g6 = d | g2;

   BddNode g7 = g3 & g6;

   BddNode g8 = g4 | g7;

   BddNode g9 = g8 & f;

   ofstream fStream;
   fStream.open("p6a.out");
   fStream << "g1" << endl << g1 << endl << endl;
   fStream << "g2" << endl << g2 << endl << endl;
   fStream << "g3" << endl << g3 << endl << endl;
   fStream << "g4" << endl << g4 << endl << endl;
   fStream << "g5" << endl << g5 << endl << endl;
   fStream << "g6" << endl << g6 << endl << endl;
   fStream << "g7" << endl << g7 << endl << endl;
   fStream << "g8" << endl << g8 << endl << endl;
   fStream << "g9" << endl << g9 << endl << endl;
   // BddNode f = ~(a & b);
   // cout << f << endl;

   // BddNode g = c | d;
   // cout << g << endl;

   // BddNode h = ~e;
   // cout << h << endl;

   // BddNode i = f ^ (c | d); // f ^ g;
   // cout << i << endl;

   // BddNode j = ~a | ~b;
   // cout << j << endl;

//    cout << "KK" << endl;
//    BddNode k = ( (a|b) ^ (d&e) );
//    cout << k << endl;
// //   cout << k.getLeftCofactor(1) << endl;
//    cout << k.getLeftCofactor(2) << endl;
// //   cout << k.getRightCofactor(4) << endl;
//    cout << k.getRightCofactor(2) << endl;
//    BddNode l1 = k.exist(2);
//    cout << l1 << endl;
//    BddNode l2 = k.getLeftCofactor(2) | k.getRightCofactor(2);
//    cout << l2 << endl;

   // ofstream ofile("i.dot");
   // i.drawBdd("i", ofile);
   // system("dot -o i.png -Tpng i.dot");

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

//   bm.reset();
   bm.init(nin, h, c);
}

