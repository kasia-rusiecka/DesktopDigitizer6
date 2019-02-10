// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *               calib.cc                *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2018              *
// *                                       *
// *****************************************

#include "DDCalib.hh"
#include <iostream>

//argv[0] - ./calib
//argv[1] - path to the data eg. data/test/

int main(int argc, char **argv){
 
  if(argc!=2){
    std::cerr << "To run type: ./calib path/to/data/" << std::endl;
    return 0;
  }
  
  TString path(argv[1]);
  
  DDCalib *calib;
  
  try{
    calib = new DDCalib(path);
  } 
  catch(const char *message){
    std::cout << message << std::endl;
    std::cout << "##### Exception in calib.cc!" << std::endl;
    return 0;
  }
  
  calib->Print();
  calib->Calibrate();
  delete calib;
  
  return 1;
}