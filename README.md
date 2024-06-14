# SoCV final project: implementing FDD engine

### FDD engine based on the GV project with Functional Decision Diagram and its interface with BDD is implemented and can support the new commands below:
- FAND
- FINV
- FOR
- FXOR
- FXNOR
- FCofactor
- FExist 
- FCompare 
- FReport
- FDraw
- FSetvar
- F2B
- B2F

Above are basic function of FDD and interface to switch between FDD and BDD

# How to compile
## Using GV project
under folder ./socv-1122, type the command below to install GV:
```json=
./SETUP.sh
./INSTALL.sh 
```

### To use the GV program
under folder ./socv-1122, type the command below to use GV:
```json=
./gv
```

## Using implementation on RicBDD
under folder ./socv-1122, type the command below to compile test program testBDD:
```json=
./cd IntrinsicBFDD
make
```

And a test program **testBdd** is generated under folder ./socv-1122/IntrinsicBFDD
