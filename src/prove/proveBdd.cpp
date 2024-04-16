/****************************************************************************
  FileName     [ proveBdd.cpp ]
  PackageName  [ prove ]
  Synopsis     [ For BDD-based verification ]
  Author       [ ]
  Copyright    [ Copyright(c) 2023-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include "bddMgrV.h"
#include "gvMsg.h"
// #include "gvNtk.h"
#include <iomanip>
#include <iostream>
#include <vector>

#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

void
BddMgrV::buildPInitialState() {
    // TODO : remember to set _initState
    BddNodeV initState = BddNodeV::_one();
    // Hint : use "cirMgr" to get the network info from the manager
    // set initial state to all zero
    for(unsigned i = 0, n = cirMgr->getNumLATCHs(); i < n; ++i){
        CirRoGate* latchOutput = cirMgr->getRo(i);
        initState &= ~getSupport(latchOutput->getGid());
    }
    _initState = initState;
    // cout << "nums of _reachStates: " << _reachStates.size() << endl;
    _reachStates.clear();
    // cout << "nums of _reachStates: " << _reachStates.size() << endl;
    _reachStates.push_back(_initState);
    // cout << "nums of _reachStates: " << _reachStates.size() << endl;

}

void
BddMgrV::buildPTransRelation() {
    // TODO : remember to set _tr, _tri
    // Hint : use "cirMgr" to get the network info from the manager
    // cout << "buildPTransRelation" << endl;
    _tri = BddNodeV::_one();
    // cout << "latchnums: " << cirMgr->getNumLATCHs() << endl;
    for(unsigned i = 0, n = cirMgr->getNumLATCHs(); i < n; ++i){
        // cout << "i: " << i << endl;
        CirRiGate* Ri = cirMgr->getRi(i);
        BddNodeV delta = getBddNodeV(Ri->getIn0Gate()->getGid());
        if(Ri->getIn0().isInv()) delta = ~delta;    
        // unsigned offset = cirMgr->getNumPIs() + cirMgr->getNumLATCHs() + 1;
        // cout << "offset: " << offset << endl;
        // cout << "Ri->getGid(): " << Ri->getGid() << endl;
        BddNodeV Yns = getBddNodeV(to_string(Ri->getGid()) + "_ns");
        BddNodeV Y = getBddNodeV(Ri->getGid());
        _tri &= ~(Yns ^ delta);
        // cout << "Yns: " << Yns << endl;
        // cout << "Y: " << Y << endl;
        // cout << "delta"<<Ri->getIn0Gate()->getGid()<<": " << delta << endl;
        // cout << "finished" << endl;

    }
    // cout << "_tri: " << _tri << endl;
    _tr = _tri;
    for(unsigned i = 0, n = cirMgr->getNumPIs(); i < n; ++i){
        _tr = _tr.exist(cirMgr->getPi(i)->getGid());
    }
}

void
BddMgrV::buildPImage(int level) {
    // TODO : remember to add _reachStates and set _isFixed
    // Hint : use "cirMgr" to get the network info from the manager
    // note:: _reachStates record the set of reachable states
    _isFixed = false;
    for(int l = 0; l < level && !isPFixed(); ++l){
        BddNodeV S_n_X = getPReachState();
        // if(_reachStates.size() > 1){
        //     S_n_X = restrict(getPReachState(), ~(_reachStates[_reachStates.size()-2]));
        // }
        // cout << "S_n_X" << endl << S_n_X << endl;
        S_n_X = S_n_X & getPTr();
        BddNodeV S_nxt_YX  = S_n_X;
        // ofstream fout1("S_n+1_YX.dot");
        // S_nxt_YX.drawBdd("S_n+1_YX",fout1);
        // fout1.close();
        unsigned numLatchs = cirMgr->getNumLATCHs();
        // ofstream fout2("S_nxt_YX.dot");
        // S_nxt_YX.drawBdd("S_nxt_YX",fout2);
        // fout2.close();
        // cout << "S_nxt_YX" << endl << S_nxt_YX << endl;
        for(unsigned i = 0; i < numLatchs; ++i){
            S_nxt_YX = S_nxt_YX.exist(cirMgr->getRo(i)->getGid());
        }
        // ofstream fout2("S_n+1_Y.dot");
        // S_nxt_YX.drawBdd("S_n+1_Y",fout2);
        // fout2.close();
        // cout << "S_n+1_Y" <<endl<< S_nxt_YX << endl;
        bool isMoved = false;
        // cout << "S_nxt_YX" << endl << S_nxt_YX << endl;
        // S_nxt_YX = S_nxt_YX.nodeMove(getSupport(cirMgr->getRi(i)->getGid()).getLevel(), getSupport(cirMgr->getRo(i)->getGid()).getLevel(), isMoved);
        // ofstream fout("S_n_X.dot");
        // S_n_X.drawBdd("SnX",fout);
        // fout.close();
        unsigned from = cirMgr->getRo(numLatchs-1)->getGid() + cirMgr->getNumLATCHs();
        unsigned to = cirMgr->getRo(numLatchs-1)->getGid();
        S_nxt_YX = S_nxt_YX.nodeMove(from, to, isMoved);
        // cout << "move the node from " << from << " to " << to << endl;
        if(!isMoved){
            cout << "No move: from " << from << " to " << to << endl; 
        }
        // ofstream fout3("R_n+1_X.dot");
        // S_nxt_YX.drawBdd("R_n+1_X",fout3);
        // fout3.close();
        S_nxt_YX |= getPReachState();
        if(S_nxt_YX == getPReachState()){
            _isFixed = true;
            cout << "Fixed point is reached (time : " << _reachStates.size() -1 << ")" << endl;
        }
        else _reachStates.push_back(S_nxt_YX);
        // cout << "after: S_nxt_YX" << endl << S_nxt_YX << endl;
        
        // cout << l << endl;
    }
    

    
}

void
BddMgrV::runPCheckProperty(const string& name, BddNodeV monitor) {
    // TODO : prove the correctness of AG(~monitor)
    // Hint : use "cirMgr" to get the network info from the manager
    BddNodeV target = getPReachState() & monitor;
    unsigned firstCexTimeStamp = _reachStates.size() - 1;
    unsigned upper = cirMgr->getRo(cirMgr->getNumLATCHs()-1)->getGid() + cirMgr->getNumLATCHs();
    unsigned lower = cirMgr->getRo(cirMgr->getNumLATCHs()-1)->getGid();
    ofstream fout("target.dot");
    target.drawBdd("target",fout);
    fout.close();
    if(target == BddNodeV::_zero()){
        if(_isFixed) cout << "Monitor " << name << " is safe." << endl;
        else cout << "Monitor " << name << " is safe up to time " << _reachStates.size() - 1 << "." << endl;
    }
    else{  //cex founded
        cout << "Monitor " << name << " is violated." << endl;
        // cout << "size of _reachStates: " << _reachStates.size() << endl;

        // for(int timestamp = _reachStates.size() - 1; timestamp >= 0; --timestamp){
        //     // cout << "timestamp: " << timestamp << endl;
        //     BddNodeV S = _reachStates[timestamp];
        //     BddNodeV counterEx = S & monitor;
        //     if(counterEx == BddNodeV::_zero()){
        //         firstCexTimeStamp = timestamp;
        //         break;
        //     }
        // }
        // while((_reachStates[firstCexTimeStamp] & monitor) != BddNodeV::_zero()){
        //     --firstCexTimeStamp;
        // }
        // firstCexTimeStamp += 1;
        // cout << "Counterexample found at time " << firstCexTimeStamp << "." << endl;

        // BddNodeV S = target.getCube(), V;
        // bool isMoved;
        // vector<string> output;
        // for(int t = firstCexTimeStamp; t>0; --t){
        // //Step1: Vn(I) = exist Y,X "TRI & Sn+1(X->Y)"
        // S = S.nodeMove(lower, upper, isMoved);
        // V = getPTri() & S;      
        // for(unsigned i = 0, n = cirMgr->getNumLATCHs(); i < n; ++i) {
        //     V = V.exist(cirMgr->getRo(i)->getGid() + cirMgr->getNumLATCHs());
        // }
        // for(unsigned i = 0, n = cirMgr->getNumLATCHs(); i < n; ++i) {
        //     V = V.exist(cirMgr->getRo(i)->getGid());
        // }

        // //Step2: Sn(X) = exist Y,I "TRI & Sn+1(X->Y) & Vn"
        // S = (getPTri() & S & V);
        // for(unsigned i = 0, n = cirMgr->getNumPIs(); i < n; ++i) {
        //     S  = S.exist(cirMgr->getPi(i)->getGid());   
        // }
        // for(unsigned i = 0, n = cirMgr->getNumLATCHs(); i < n; ++i) {
        //     S = S.exist(cirMgr->getRo(i)->getGid() + cirMgr->getNumLATCHs());
        // }

        // //Step3: cout
        // std::stringstream ss;
        // ss << t-1 << ": " << V.getCube().toString();
        // output.push_back(ss.str());
        // }
        // for(int j=output.size()-1; j>=0; --j)
        // cout << output[j] << endl;
        // for(unsigned timestamp = 0; timestamp <= firstCexTimeStamp; ++timestamp){
        //     cout << "timestamp: " << timestamp << endl;
        // // }
        // BddNodeV counterEx_Y = target.getCube(0), counterEx_X, counterEx_I;
        
        // for(unsigned i = 0, n = cirMgr->getNumPIs(); i < n; ++i){
        //     counterEx_Y = counterEx_Y.exist(cirMgr->getPi(i)->getGid());
        // }
        // ofstream fout("counterEx_Y.dot");
        // counterEx_Y.drawBdd("counterEx_Y",fout);
        // fout.close();
        // bool isMoved = false;
        // counterEx_Y = counterEx_Y.nodeMove(lower, upper, isMoved);
        // // ofstream fout1("counterEx_Yafter.dot");
        // // counterEx_Y.drawBdd("counterEx_Yafter",fout1);
        // // fout1.close();

        // vector<BddNodeV> counterEx_Is;
        // counterEx_Is.clear();
        // unsigned t = firstCexTimeStamp;
        // // for(unsigned t = firstCexTimeStamp+1; t > 0; --t){
        //     counterEx_I = counterEx_Y & getPTri();
        //     for(unsigned i = 0, n = cirMgr->getNumLATCHs(); i < n; ++i){
        //         counterEx_I = counterEx_I.exist(cirMgr->getRo(i)->getGid() + cirMgr->getNumLATCHs());
        //     }
        //     for(unsigned i = 0, n = cirMgr->getNumLATCHs(); i < n; ++i){
        //         counterEx_I = counterEx_I.exist(cirMgr->getRo(i)->getGid());
        //     }
        //     ofstream fout2("counterEx_I.dot");
        //     counterEx_I.drawBdd("counterEx_I",fout2);
        //     fout2.close();
        //     counterEx_Is.push_back(counterEx_I);

        //     // cout << "counterEx_ i @ " << t << ":" << endl << counterEx_I << endl;
        //     // std::stringstream ss;
        //     // ss << t << ": " << counterEx_I.getCube(0).toString()[1];
        //     // cout << ss.str() << endl;
        //     cout << counterEx_I.toString() << endl;
        //     counterEx_X = counterEx_Y & counterEx_I;
        //     for(unsigned i = 0, n = cirMgr->getNumPIs(); i < n; ++i){
        //         counterEx_X = counterEx_X.exist(cirMgr->getPi(i)->getGid());
        //     }
        //     cout << "counterEx_X @ " << t << ":" << endl << counterEx_X << endl;
        //     counterEx_Y = counterEx_X.nodeMove(lower, upper, isMoved);
        //     if(!isMoved){
        //         cout << "No move: from " << lower << " to " << upper << endl; 
        //     }
        //     counterEx_Y &= getPTri();
        // // }
        // cout << "counterPis.size(): " << counterEx_Is.size() << endl;
        // ofstream fout("counterEx.dot");
        // counterEx.drawBdd("counterEx",fout);
        // fout.close();

        // ofstream fout1("counterEx_X.dot");
        // counterEx_X.drawBdd("counterEx_X",fout1);
        // fout1.close();
        // BddNodeV counterEx_I = (counterEx & counterEx_X).getCube(0);
        // for(unsigned i = 0; i < cirMgr->getNumLATCHs(); ++i){
        //     counterEx_I = counterEx_I.exist(cirMgr->getRo(i)->getGid());
        // }
        // ofstream fout2("counterEx_I.dot");
        // counterEx_I.drawBdd("counterEx_I",fout2);
        // fout2.close();
        
        // BddNodeV newCounterEx_Y = counterEx_X;
        // bool isMoved = false;
        // newCounterEx_Y = newCounterEx_Y.nodeMove(lower, upper, isMoved);
        // assert(isMoved);
        // newCounterEx_Y &= _tr;
        
        // cout << "Counterexample_X:" << endl << counterEx_X << endl;
        // cout << "Counterexample_I:" << endl << counterEx_I << endl;


        // for(unsigned timestamp = _reachStates.size() - 1; timestamp > 0; --timestamp){
        //     BddNodeV S = _reachStates[timestamp];
        //     BddNodeV S_prev = _reachStates[timestamp - 1];
        //     BddNodeV delta = S & ~S_prev;
        //     BddNodeV delta_monitor = delta & monitor;
        //     if(delta_monitor != BddNodeV::_zero()){
        //         firstCexTimeStamp = timestamp - 1;
        //         break;
        //     }
        // }
    }
}

BddNodeV
BddMgrV::find_ns(BddNodeV cs) {}

BddNodeV
BddMgrV::ns_to_cs(BddNodeV ns) {
    unsigned from = cirMgr->getRo(cirMgr->getNumLATCHs()-1)->getGid() + cirMgr->getNumLATCHs();
    unsigned to = cirMgr->getRo(cirMgr->getNumLATCHs()-1)->getGid();
    bool isMoved = false;
    BddNodeV cs = ns.nodeMove(from, to, isMoved);
    if(!isMoved){
        cout << "No move: from " << from << " to " << to << endl; 
    }
    return cs;
}
