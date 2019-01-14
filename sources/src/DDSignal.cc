// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
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
/// Default constructor.
DDSignal::DDSignal() : fAmp(-100), 
                       fT0(-100), 
                       fTOT(-100),
                       fCharge(-100), 
                       fPE(-100) {
}
//------------------------------------------------------------------
/// Standard constructor.
/// \param amplitude - signal amplitude [mV]
/// \param t0 - time T0 [ns]
/// \param tot - time over threshold [mV]
/// \param charge - signal integral (uncalibrated charge)
/// \param pe - calibrated charge [PE]
DDSignal::DDSignal(Float_t amplitude, Float_t t0, Float_t tot, Float_t charge, Float_t pe) : 
                   fAmp(amplitude), 
                   fT0(t0), 
                   fTOT(tot), 
                   fCharge(charge), 
                   fPE(pe) {
 }
//------------------------------------------------------------------
/// Standard constructor.
/// \param parameters - table containing values of the signal 
/// parameters in the following order: amplitude, T0, TOT, charge,
/// charge in PE
DDSignal::DDSignal(Float_t *parameters) : fAmp(parameters[0]), 
                                          fT0(parameters[1]),
                                          fTOT(parameters[2]),
                                          fCharge(parameters[3]),
                                          fPE(parameters[4]) {
}
//------------------------------------------------------------------
/// Default destructor
DDSignal::~DDSignal(){ 
}
//------------------------------------------------------------------
/// Sets values of all signals's parameters.
/// \param parameters - table containing values of the signal 
/// parameters in the following order: amplitude, T0, TOT, charge,
/// charge in PE
void DDSignal::SetAll(Float_t *parameters){
 fAmp    = parameters[0];
 fT0     = parameters[1];
 fTOT    = parameters[2];
 fCharge = parameters[3];
 fPE     = parameters[4]; 
 return;
}
//------------------------------------------------------------------
/// Sets all values of signals parameters tot heir default values. 
void DDSignal::Clear(void){
 fAmp    = -100.;
 fT0     = -100.;
 fTOT    = -100.;
 fCharge = -100.; 
 fPE     = -100.;
 return;
}
//------------------------------------------------------------------
/// Prints details of the DDSignal class object.
void DDSignal::Print(void){
  cout << "\n\n------------------------------------------------" << endl;
  cout << "This is Print() for DDSignal class object" << endl;
  cout << "Amplitude = " << fAmp << " mV" << endl;
  cout << "T0 = " << fT0 << " ns" << endl;
  cout << "Time over threshold = " << fTOT << " ns" << endl;
  cout << "Charge (signal integral) = " << fCharge << endl;
  cout << "Charge (calibrated) = " << fPE << " PE" << endl;
  cout << "------------------------------------------------\n" << endl;
  return;
}
//------------------------------------------------------------------