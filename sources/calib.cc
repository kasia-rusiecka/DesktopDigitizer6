// *****************************************
// *                                       *
// *           DesktopDigitizer            *
// *               calib.cc                *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2018              *
// *                                       *
// *****************************************

#include "DDCalib.hh"
#include <iostream>
using namespace std;

//argv[0] - ./calib
//argv[1] - path to the data eg. data/test/

int main(int argc, char **argv){
 
  if(!argc!=2){
    cout << "To run type: ./calib path/to/data/" << endl;
    return 0;
  }
  
  TString path(argv[1]);
  
  DDCalib *calib;
  
  try{
    calib = new DDCalib(path);
  } 
  catch(const char *message){
    cout << message << endl;
    cout << "##### Exception in calib.cc!" << endl;
    return 0;
  }
  
  calib->Print();
  calib->Calibrate();
  delete calib;
  
  return 1;
}