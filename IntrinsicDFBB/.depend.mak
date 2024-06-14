myString.o: myString.cpp
fddNode.o: fddNode.cpp bddMgr.h myHash.h bddNode.h fddNode.h
testBdd.o: testBdd.cpp bddNode.h fddNode.h bddMgr.h myHash.h
bddMgr.o: bddMgr.cpp bddNode.h bddMgr.h myHash.h fddNode.h
bddNode.o: bddNode.cpp bddNode.h bddMgr.h myHash.h fddNode.h
