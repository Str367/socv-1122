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
   // cout << "g1" << endl;
   // cout << g1 << endl;

   BddNode g2 = ~c & e;
   // cout << "g2" << endl;
   // cout << g2 << endl;

   BddNode g3 = a & b;
   // cout << "g3" << endl;
   // cout << g3 << endl;

   BddNode g4 = c & g1;
   // cout << "g4" << endl;
   // cout << g4 << endl;

   BddNode g5 = g1 | g2;
   // cout << "g5" << endl;
   // cout << g5 << endl;

   BddNode g6 = d | g2;
   // cout << "g6" << endl;
   // cout << g6 << endl;

   BddNode g7 = g3 & g6;
   // cout << "g7" << endl;
   // cout << g7 << endl;

   BddNode g8 = g4 | g7;
   // cout << "g8" << endl;
   // cout << g8 << endl;

   BddNode g9 = g8 & g5 & f;
   // cout << "g9" << endl;
   // cout << g9 << endl;

   // g5 == 1: b & d | ~c & e
   BddNode g9_g5 = g9.getLeftCofactor(2).getLeftCofactor(4) | g9.getLeftCofactor(5).getRightCofactor(3);
   // g5 == 0: (~b | ~d) & (c | ~e) = (~b & c) | (~b & ~e) | (~d & c) | (~d & ~e)
   BddNode g9_ng5 = g9.getRightCofactor(2).getLeftCofactor(3) | g9.getRightCofactor(2).getRightCofactor(5) | g9.getRightCofactor(4).getLeftCofactor(3) | g9.getRightCofactor(4).getRightCofactor(5);
   BddNode g9_dg5 = g9_g5 ^ g9_ng5;
   
   cout << "g9_dg5" << endl;
   cout << g9_dg5 << endl;

   // cout << "g9_g5" << endl;
   // cout << g9_g5 << endl;
   // cout << "g9_dg5" << endl;
   // cout << g9_dg5 << endl;

   // cout << "g5.getLeftCofactor(2)" << endl;
   // cout << g5.getLeftCofactor(2) << endl;
   // cout << "g5.getRightCofactor(2)" << endl;
   // cout << g5.getRightCofactor(2) << endl;
   // cout << g5._one << endl;
   // cout << g5._zero << endl;

   ofstream fStream;
   fStream.open("p6c.out");
   fStream << "g9_dg5" << endl << g9_dg5 << endl << endl;
   // fStream << "g1" << endl << g1 << endl << endl;
   // fStream << "g2" << endl << g2 << endl << endl;
   // fStream << "g3" << endl << g3 << endl << endl;
   // fStream << "g4" << endl << g4 << endl << endl;
   // fStream << "g5" << endl << g5 << endl << endl;
   // fStream << "g6" << endl << g6 << endl << endl;
   // fStream << "g7" << endl << g7 << endl << endl;
   // fStream << "g8" << endl << g8 << endl << endl;
   // fStream << "g9" << endl << g9 << endl << endl;
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

   // ofstream ofile("g9.dot");
   // g9.drawBdd("i", ofile);
   // system("dot -o g9.png -Tpng g9.dot");

   // ofstream ofile2("g5.dot");
   // g5.drawBdd("i", ofile2);
   // system("dot -o g5.png -Tpng g5.dot");

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

