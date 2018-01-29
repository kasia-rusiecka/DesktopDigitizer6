// *****************************************
// *                                       *
// *           DesktopDigitizer            *
// *               digit.cc                *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2017              *
// *                                       *
// *****************************************

#include "DDTreeMaker.hh" 
#include "DDSignal.hh"
#include <iostream>
using namespace std;

//argv[0] - ./digit
//argv[1] - path to the data eg. data/test/

int main(int argc, char **argv){
 
  if(argc!=2){
    cout << "To run type: ./digit path/to/data/" << endl;
    return 0;
  }
  
  TString path(argv[1]);
  
  DDTreeMaker *maker = new DDTreeMaker(path);
  maker->Print();
  maker->MakeTree();
  delete maker;
  
  return 1;
}
