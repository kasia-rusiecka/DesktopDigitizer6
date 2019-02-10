// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *               digit.cc                *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2017              *
// *                                       *
// *****************************************

#include "DDTreeMaker.hh" 
#include <iostream>

//argv[0] - ./digit
//argv[1] - path to the data eg. data/test/

int main(int argc, char **argv){
 
  if(argc!=2){
    std::cerr << "To run type: ./digit path/to/data/" << std::endl;
    return 0;
  }
  
  TString path(argv[1]);
  
  DDTreeMaker *maker;
  
  try{
    maker = new DDTreeMaker(path);
  } 
  catch(const char *message){
    std::cerr << message << std::endl;
    std::cerr << "##### Exception in digit.cc!" << std::endl;
    return 0;
  }
  
  maker->Print();
  maker->MakeTree();
  delete maker;
  
  return 1;
}
