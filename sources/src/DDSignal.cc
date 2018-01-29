// *****************************************
// *                                       *
// *           DesktopDigitizer            *
// *             DDSignal.cc               *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2017              *
// *                                       *
// *****************************************

#include "DDSignal.hh"
using namespace std;

ClassImp(DDSignal);

//------------------------------------------------------------------
DDSignal::DDSignal(){
 fAmp    = -100.;
 fT0     = -100.;
 fTOT    = -100.;
 fCharge = -100.; 
 fPE     = -100.;
}
//------------------------------------------------------------------
DDSignal::DDSignal(Float_t amplitude, Float_t t0, Float_t tot, Float_t charge, Float_t pe){
 fAmp    = amplitude; 
 fT0     = t0;
 fTOT    = tot;
 fCharge = charge;
 fPE     = pe;
}
//------------------------------------------------------------------
DDSignal::DDSignal(Float_t *parameters){
 fAmp    = parameters[0];
 fT0     = parameters[1];
 fTOT    = parameters[2];
 fCharge = parameters[3]; 
 fPE     = parameters[4]; 
}
//------------------------------------------------------------------
DDSignal::~DDSignal(){ 
}
//------------------------------------------------------------------
void DDSignal::SetAll(Float_t *parameters){
 fAmp    = parameters[0];
 fT0     = parameters[1];
 fTOT    = parameters[2];
 fCharge = parameters[3];
 fPE     = parameters[4]; 
 return;
}
//------------------------------------------------------------------
void DDSignal::Clear(void){
 fAmp    = -100.;
 fT0     = -100.;
 fTOT    = -100.;
 fCharge = -100.; 
 fPE     = -100.;
 return;
}
//------------------------------------------------------------------
void DDSignal::Print(void){
  cout << "\n\n------------------------------------------------" << endl;
  cout << "This is Print() for DDSignal class object" << endl;
  cout << "Amplitude = " << fAmp << endl;
  cout << "T0 = " << fT0 << endl;
  cout << "Time over threshold = " << fTOT << endl;
  cout << "Charge (integral) = " << fCharge << endl;
  cout << "PE = " << fPE << endl;
  cout << "------------------------------------------------\n" << endl;
  return;
}
//------------------------------------------------------------------