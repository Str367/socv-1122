/****************************************************************************
  FileName     [ cirBdd.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define BDD manager functions ]
  Author       [ Design Verification Lab ]
  Copyright    [ Copyright(c) 2023-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include "bddMgrV.h"   // MODIFICATION FOR SoCV BDD
#include "bddNodeV.h"  // MODIFICATION FOR SoCV BDD
#include "cirGate.h"
#include "cirMgr.h"
#include "gvMsg.h"
#include "util.h"
#include <string>

extern BddMgrV* bddMgrV;  // MODIFICATION FOR SoCV BDD

const bool
CirMgr::setBddOrder(const bool& file) {
    unsigned supportSize = getNumPIs() + 2 * getNumLATCHs();
    unsigned bddspsize = bddMgrV->getNumSupports();
    if (supportSize >= bddMgrV->getNumSupports()) {
        gvMsg(GV_MSG_ERR) << "BDD Support Size is Smaller Than Current Design Required !!" << endl;
        return false;
    }
    // build support
    unsigned supportId = 1;
    // build PI (primary input)
    for (unsigned i = 0, n = getNumPIs(); i < n; ++i) {
        CirPiGate* gate = (file) ? getPi(i) : getPi(n - i - 1);
        bddMgrV->addBddNodeV(gate->getGid(), bddMgrV->getSupport(supportId)());
        bddMgrV->addFddNodeV(gate->getGid(), bddMgrV->getSupport(supportId)());
        ++supportId;
    }
    // build FF_CS (X: current state)
    for (unsigned i = 0, n = getNumLATCHs(); i < n; ++i) {
        CirRoGate* gate = (file) ? getRo(i) : getRo(n - i - 1);
        bddMgrV->addBddNodeV(gate->getGid(), bddMgrV->getSupport(supportId)());
        bddMgrV->addFddNodeV(gate->getGid(), bddMgrV->getSupport(supportId)());
        ++supportId;
    }
    // build FF_NS (Y: next state)
    // here we only create "CS_name + _ns" for y_i
    for (unsigned i = 0, n = getNumLATCHs(); i < n; ++i) {
        CirRiGate* gate = (file) ? getRi(i) : getRi(n - i - 1);
        bddMgrV->addBddNodeV(gate->getName(), bddMgrV->getSupport(supportId)());
        bddMgrV->addFddNodeV(gate->getName(), bddMgrV->getSupport(supportId)());
        ++supportId;
    }
    // Constants (const0 node, id=0)
    bddMgrV->addBddNodeV(_const0->getGid(), BddNodeV::_zero());
    bddMgrV->addFddNodeV(_const0->getGid(), FddNodeV::_zero());
    ++supportId;

    return true;
}

void CirMgr::buildNtkBdd() {
    // TODO: build BDD for ntk here
    // Perform DFS traversal from DFF inputs, inout, and output gates.
    // Collect ordered nets to a GVNetVec
    // Construct BDDs in the DFS order

    for(unsigned i = 0,n = getNumPOs(); i < n; ++i){
        // cout << "po" << getPo(i)->getGid() << endl;
        buildBdd(getPo(i));
    }
    for(unsigned i = 0,n = getNumPIs(); i < n; ++i){
        // cout << "pi" << getPi(i)->getGid() << endl;
        buildBdd(getPi(i));
    }
    for(unsigned i = 0,n = getNumLATCHs(); i < n; ++i){
        // cout << "ro" << getRo(i)->getGid() << endl;
        buildBdd(getRo(i));
        // cout << "ri" << getRi(i)->getGid() << endl;
        buildBdd(getRi(i));
    }
}

void CirMgr::buildBdd(CirGate* gate) {
    GateList orderedGates;
    clearList(orderedGates);
    CirGate::setGlobalRef();
    gate->genDfsList(orderedGates);
    assert(orderedGates.size() <= getNumTots());

    // TODO: build BDD for the specified net here
    for(unsigned i = 0, n = orderedGates.size(); i<n; ++i){
        CirGate* g = orderedGates[i];
        // cout << g->getGid() << " " << g->getType() << endl;
        if(g->getType() == AIG_GATE){
        // if(g->getType() == AIG_GATE){
            // cout<<(g->getGid())<<endl;
            // BddNodeV res = bddMgrV -> getBddNodeV(g->getGid());
            BddNodeV l = bddMgrV -> getBddNodeV(g->getIn0Gate()->getGid());
            // if(i<4)cout << "l" << l << endl;
            if(g->getIn0().isInv())
                l = ~l;
            BddNodeV r = bddMgrV -> getBddNodeV(g->getIn1Gate()->getGid());
            // if(i<4)cout << "r" << r << endl;
            if(g->getIn1().isInv())
                r = ~r;
            BddNodeV res = l & r;
            
            bddMgrV -> addBddNodeV((g->getGid()), res());
            // string name = to_string(g->getGid());
            // // cout<<name<<endl;
            // bddMgrV -> addBddNodeV(name, res());
        }
    }
    if(gate->getType() == PO_GATE){
        BddNodeV res = bddMgrV -> getBddNodeV(gate->getGid());
        BddNodeV l = bddMgrV -> getBddNodeV(gate->getIn0Gate()->getGid());
        if(gate->getIn0().isInv())
            l = ~l;
        res = l;
        bddMgrV -> addBddNodeV((gate->getGid()), res());
    }
    if(gate->getType() == RI_GATE){
        BddNodeV res = bddMgrV -> getBddNodeV(gate->getGid());
        BddNodeV l = bddMgrV -> getBddNodeV(gate->getIn0Gate()->getGid());
        if(gate->getIn0().isInv())
            l = ~l;
        res = l;
        bddMgrV -> addBddNodeV((gate->getGid()), res());
    }
}


//for fdd implementation

// void CirMgr::buildNtkFdd() {
//     // TODO: build BDD for ntk here
//     // Perform DFS traversal from DFF inputs, inout, and output gates.
//     // Collect ordered nets to a GVNetVec
//     // Construct BDDs in the DFS order

//     for(unsigned i = 0,n = getNumPOs(); i < n; ++i){
//         // cout << "po" << getPo(i)->getGid() << endl;
//         buildFdd(getPo(i));
//     }
//     for(unsigned i = 0,n = getNumPIs(); i < n; ++i){
//         // cout << "pi" << getPi(i)->getGid() << endl;
//         buildFdd(getPi(i));
//     }
//     for(unsigned i = 0,n = getNumLATCHs(); i < n; ++i){
//         // cout << "ro" << getRo(i)->getGid() << endl;
//         buildFdd(getRo(i));
//         // cout << "ri" << getRi(i)->getGid() << endl;
//         buildFdd(getRi(i));
//     }
// }

// void CirMgr::buildFdd(CirGate* gate) {
//     GateList orderedGates;
//     clearList(orderedGates);
//     CirGate::setGlobalRef();
//     gate->genDfsList(orderedGates);
//     assert(orderedGates.size() <= getNumTots());

//     // TODO: build BDD for the specified net here
//     for(unsigned i = 0, n = orderedGates.size(); i<n; ++i){
//         cout << i << endl;
//         CirGate* g = orderedGates[i];
//         // cout << g->getGid() << " " << g->getType() << endl;
//         if(g->getType() == AIG_GATE){
//             cout << "aig" << endl;
//         // if(g->getType() == AIG_GATE){
//             // cout<<(g->getGid())<<endl;
//             // BddNodeV res = bddMgrV -> getBddNodeV(g->getGid());
//             FddNodeV l = bddMgrV -> getFddNodeV(g->getIn0Gate()->getGid());
//             cout << "l" << l << endl;
//             if(g->getIn0().isInv())
//                 l = ~l;
//             FddNodeV r = bddMgrV -> getFddNodeV(g->getIn1Gate()->getGid());
//             cout << "r" << r << endl;
//             if(g->getIn1().isInv())
//                 r = ~r;
//             FddNodeV res = l & r;
//             cout << "res" << res() << endl;
//             bddMgrV -> addFddNodeV((g->getGid()), res());
//             // string name = to_string(g->getGid());
//             // // cout<<name<<endl;
//             // bddMgrV -> addBddNodeV(name, res());
//         }
//     }
//     if(gate->getType() == PO_GATE){
//         cout << "po" << endl;
//         FddNodeV res = bddMgrV -> getFddNodeV(gate->getGid());
//         FddNodeV l = bddMgrV -> getFddNodeV(gate->getIn0Gate()->getGid());
//         if(gate->getIn0().isInv())
//             l = ~l;
//         res = l;
//         bddMgrV -> addFddNodeV((gate->getGid()), res());
//     }
//     if(gate->getType() == RI_GATE){
//         cout << "ri" << endl;
//         FddNodeV res = bddMgrV -> getFddNodeV(gate->getGid());
//         FddNodeV l = bddMgrV -> getFddNodeV(gate->getIn0Gate()->getGid());
//         if(gate->getIn0().isInv())
//             l = ~l;
//         res = l;
//         bddMgrV -> addFddNodeV((gate->getGid()), res());
//     }
//     if(gate->getType() == RO_GATE) cout << "ro" << endl;
// }