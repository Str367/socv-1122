#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "sat.h"
#include <queue>

using namespace std;

class Gate
{
public:
   Gate(unsigned i = 0): _gid(i) {}
   ~Gate() {}

   Var getVar() const { return _var; }
   void setVar(const Var& v) { _var = v; }

private:
   unsigned   _gid;  // for debugging purpose...
   Var        _var;
};

enum Operation{Undefined,Assign, AssignNot, LessThan, Adjacent, AdjacentNot};
unordered_map<string, Operation> opMap = {
   {"Assign", Assign},
   {"AssignNot", AssignNot},
   {"LessThan", LessThan},
   {"Adjacent", Adjacent},
   {"AdjacentNot", AdjacentNot}
};

// 
//[0] PI  1 (a)
//[1] PI  2 (b)
//[2] AIG 4 1 2
//[3] PI  3 (c)
//[4] AIG 5 1 3
//[5] AIG 6 !4 !5
//[6] PO  9 !6
//[7] AIG 7 !2 !3
//[8] AIG 8 !7 1
//[9] PO  10 8
//
vector<vector<Gate *>> seatAssignment;
Var property = var_Undef; //last element is the property
int manCount;

void update_property(Var v, SatSolver& s, bool inverse){
   if(property == var_Undef){
      Var tmp = s.newVar();
      property = s.newVar();
      s.addAigCNF(property,tmp,false,v,inverse);
   }
   else {
      Var v2 = s.newVar();
      s.addAigCNF(v2,property,false,v,inverse);
      property = v2;
   }
}

void
initCondition(SatSolver& s)
{
   // Init gates
   for(int i = 0; i < manCount; ++i) {
      seatAssignment.push_back(vector<Gate *>());
      for(int j = 0; j < manCount; ++j) {
         seatAssignment[i].push_back(new Gate(i * manCount + j));  // note that i is always greater than j
         Var v = s.newVar();
         seatAssignment[i][j]->setVar(v);
      }
   }
   vector<Var> vars;
   for(size_t i = 0; i < manCount; ++i) {
      vars.clear();
      for(size_t j = 0; j < manCount; ++j) {
         Var v = s.newVar();
         if(j == 0){
            s.addAigCNF(v,seatAssignment[i][0]->getVar(),false,seatAssignment[i][1]->getVar(),true);
         }
         else if(j == 1){
            s.addAigCNF(v,seatAssignment[i][0]->getVar(),true,seatAssignment[i][1]->getVar(),false);
         }
         else s.addAigCNF(v,seatAssignment[i][0]->getVar(),true,seatAssignment[i][1]->getVar(),true);



         for(size_t k = 2; k < manCount; ++k) {
            // cout << "i,j,k= " << i << " " << j << " " << k << endl;
            Var v2 = s.newVar();
            if(k == j) s.addAigCNF(v2,v,false,seatAssignment[i][k]->getVar(),false);
            else s.addAigCNF(v2,v,false,seatAssignment[i][k]->getVar(),true);
            v = v2;
         }
         vars.push_back(v);
      }
         // cout<<"Vars.size() = "<<vars.size()<<endl;
      Var v = s.newVar();
      s.addAigCNF(v,vars[0],true,vars[1],true);
      for(size_t j = 2, size = vars.size(); j < size; ++j) {
         Var v2 = s.newVar();
         s.addAigCNF(v2,v,false,vars[j],true);
         v = v2;
      }
      // cout<<"update property"<<endl;
      update_property(v,s,true);
   }


   for(size_t j = 0; j < manCount; ++j) {
      vars.clear();
      for(size_t i = 0; i < manCount; ++i) {
         Var v = s.newVar();
         if(i == 0){
            s.addAigCNF(v,seatAssignment[0][j]->getVar(),false,seatAssignment[1][j]->getVar(),true);
         }
         else if(i == 1){
            s.addAigCNF(v,seatAssignment[0][j]->getVar(),true,seatAssignment[1][j]->getVar(),false);
         }
         else s.addAigCNF(v,seatAssignment[0][j]->getVar(),true,seatAssignment[1][j]->getVar(),true);



         for(size_t k = 2; k < manCount; ++k) {
            Var v2 = s.newVar();
            if(k == i) s.addAigCNF(v2,v,false,seatAssignment[k][j]->getVar(),false);
            else s.addAigCNF(v2,v,false,seatAssignment[k][j]->getVar(),true);
            v = v2;
         }
         vars.push_back(v);
      }
      Var v = s.newVar();
      s.addAigCNF(v,vars[0],true,vars[1],true);
      for(size_t i = 2, size = vars.size(); i < size; ++i) {
         Var v2 = s.newVar();
         s.addAigCNF(v2,v,false,vars[i],true);
         v = v2;
      }
      update_property(v,s,true);
   }

   //test assign 5to5
   // update_property(seatAssignment[4][0]->getVar(),s,true); //ok


   //set the condition that every seat is assigned to one student

   // gates.push_back(new Gate(1));  // gates[0]
   // gates.push_back(new Gate(2));  // gates[1]
   // gates.push_back(new Gate(4));  // gates[2]
   // gates.push_back(new Gate(3));  // gates[3]
   // gates.push_back(new Gate(5));  // gates[4]
   // gates.push_back(new Gate(6));  // gates[5]
   // gates.push_back(new Gate(9));  // gates[6]
   // gates.push_back(new Gate(7));  // gates[7]
   // gates.push_back(new Gate(8));  // gates[8]
   // gates.push_back(new Gate(10)); // gates[9]

   // POs are not needed in this demo example
}

void
assign(int man, int seat, SatSolver& s, bool inverse){
   Var v = seatAssignment[man][seat]->getVar();
   update_property(v,s,inverse);
}

void
assignNot(int man, int seat, SatSolver& s){
   Var v = seatAssignment[man][seat]->getVar();
   update_property(v,s,true);
}

void
lessThan(int manless, int manmore, SatSolver& s){
   vector<Var> vars;
   for(size_t i = 1 ; i<manCount; ++i){
      Var v2 = seatAssignment[manmore][i]->getVar();
      for(size_t j = 0; j<i; ++j){
         Var v = s.newVar();
         Var v1 = seatAssignment[manless][j]->getVar();
         s.addAigCNF(v,v1,false,v2,false);
         vars.push_back(v);
      }
   }
   // cout<<"vars.size() = "<<vars.size()<<endl;
   if(vars.size() == 1){
      update_property(vars[0],s,false);
   }
   else{
      Var v = s.newVar();
      s.addAigCNF(v,vars[0],true,vars[1],true);
      for(size_t i = 2; i<vars.size(); ++i){
         Var v2 = s.newVar();
         s.addAigCNF(v2,v,false,vars[i],true);
         v = v2;
      }
      update_property(v,s,true);
   }
}

void
adjacent(int man1, int man2, SatSolver& s, bool isNot){
   queue<Var> vars;
   for(size_t i = 0; i<manCount-1; ++i){
      Var v1 = s.newVar(), v2 = s.newVar(), v3 = s.newVar();
      s.addAigCNF(v1,seatAssignment[man1][i]->getVar(),false,seatAssignment[man2][i+1]->getVar(),false);
      s.addAigCNF(v2,seatAssignment[man1][i+1]->getVar(),false,seatAssignment[man2][i]->getVar(),false);
      s.addAigCNF(v3,v1,true,v2,true);
      vars.push(v3);
   }
   while(!vars.empty()){
      Var v1 = vars.front();
      vars.pop();
      if(vars.empty()) {
         update_property(v1,s,!isNot);
         break;
      }
      else{
         Var v2 = vars.front();
         vars.pop();
         Var v3 = s.newVar();
         s.addAigCNF(v3,v1,false,v2,false);
         vars.push(v3);
      }
   }
}


// void
// genProofModel(SatSolver& s)
// {
//    // Allocate and record variables; No Var ID for POs

//    // Hard code the model construction here...
//    // [2] AIG 4 1 2 ==> [2] = [0] & [1]
//    s.addAigCNF(gates[2]->getVar(), gates[0]->getVar(), false,
//                gates[1]->getVar(), false);
//    // [4] AIG 5 1 3 ==> [4] = [0] & [3]
//    s.addAigCNF(gates[4]->getVar(), gates[0]->getVar(), false,
//                gates[3]->getVar(), false);
//    // [5] AIG 6 !4 !5 ==> [5] = ![2] & ![4]
//    s.addAigCNF(gates[5]->getVar(), gates[2]->getVar(), true,
//                gates[4]->getVar(), true);
//    // [7] AIG 7 !2 !3 ==> [7] = ![1] & ![3]
//    s.addAigCNF(gates[7]->getVar(), gates[1]->getVar(), true,
//                gates[3]->getVar(), true);
//    // [8] AIG 8 !7 1 ==> [8] = ![7] & [0]
//    s.addAigCNF(gates[8]->getVar(), gates[7]->getVar(), true,
//                gates[0]->getVar(), false);
// }

// void
// proofModelInitialize(SatSolver& s){
//    s.initialize();
//    for (size_t i = 0, n = gates.size(); i < n; ++i) {
//       for(size_t j = 0, isize = gates[i].size(); j < isize; ++j) {
//          Var v = s.newVar();
//          gates[i][j]->setVar(v);
//       }
//    }
// }

void reportResult(const SatSolver& solver, bool result)
{
   // solver.printStats();
   cout << (result? "Satisfiable assignment:" : "No satisfiable assignment can be found.") << endl;
   if (result) {
      for (size_t j = 0, n = manCount; j < n; ++j){
         for(size_t i = 0, isize = manCount; i < isize; ++i){
            if(solver.getValue(seatAssignment[i][j]->getVar()) == 1){
               cout << j << "(" << i << ")";
               if(j != isize-1) cout << ", ";
            }
         }
      }
      cout << endl;
   }
}

int main(int argc, char **argv)
{
   if(argc != 2) {
      cout << "Usage: seatAtt <file_name>" << endl;
      return 0;
   }
   string fileName = argv[1];
   ifstream file(fileName);
   if(!file) {
      cout << "Cannot open file " << fileName << endl;
      return 0;
   }
   string line;
   string op,i,j;
   int ii,ij;

   SatSolver solver;
   solver.initialize();
   getline(file,line);
   istringstream(line) >> manCount;
   initCondition(solver);
   // cout<<manCount<<endl;
   const char space = ' ', lbracket = '(', rbracket = ')',comma = ',';
   while(getline(file,line)){
      // cout << line << endl;
      int pos1 = line.find(lbracket);
      op = line.substr(0,pos1);
      int pos2 = line.find(comma);
      i = line.substr(pos1+1,pos2-pos1-1);
      int pos3 = line.find(space,pos2);
      int pos4 = line.find(rbracket);
      j = line.substr(pos3+1,pos4-pos3-1);
      // cout << op << " " << i << " " << j << endl;
      // cout << i.size() << " " << j.size() << endl;
      try {
         ii = std::stoi(i);
         ij = std::stoi(j);
         // std::cout << "Integer: " << ii << "," << ij << std::endl;
      } catch (const std::invalid_argument& e) {
         std::cerr << "Invalid argument: " << e.what() << std::endl;
      }
      switch(opMap[op]){
         case Assign:
            // cout << "Assign " << ii << " " << ij << endl;
            assign(ii,ij,solver,false);
            break;
         case AssignNot:
            // cout << "AssignNot " << ii << " " << ij << endl;
            assign(ii,ij,solver,true);
            break;
         case LessThan:
            // cout << "LessThan " << ii << " " << ij << endl;
            lessThan(ii,ij,solver);
            break;
         case Adjacent:
            // cout << "Adjacent " << ii << " " << ij << endl;
            adjacent(ii,ij,solver,false);
            break;
         case AdjacentNot:
            // cout << "AdjacentNot " << ii << " " << ij << endl;
            adjacent(ii,ij,solver,true);
            break;
         default:
            // cout << "Undefined operation" << endl;
            break;
      }
   }
   solver.assumeRelease();
   solver.assumeProperty(property, 1);
   bool result = solver.assumpSolve();
   reportResult(solver, result);
   return 0;
   //
   // genProofModel(solver);

   // bool result;
   // // Solve(Gate(5) ^ Gate(8))
   // Var newV = solver.newVar();
   // solver.addXorCNF(newV, gates[5]->getVar(), false, gates[8]->getVar(), false);
   // solver.assumeRelease();  // Clear assumptions
   // solver.assumeProperty(newV, true);
   // result = solver.assumpSolve();
   // reportResult(solver, result);
}
