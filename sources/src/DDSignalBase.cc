// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *            DDSignalBase.cc            *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2017              *
// *                                       *
// *****************************************

#include "DDSignalBase.hh"

ClassImp(DDSignalBase);

//------------------------------------------------------------------
/// Default constructor.
DDSignalBase::DDSignalBase() : fAmp(-100), 
                               fT0(-100), 
                               fTOT(-100),
                               fCharge(-100) {}
//------------------------------------------------------------------
/// Standard constructor.
/// \param amplitude - signal amplitude [mV]
/// \param t0 - time T0 [ns]
/// \param tot - time over threshold [mV]
/// \param charge - signal integral (uncalibrated charge)
DDSignalBase::DDSignalBase(float amp, float t0, float tot, float charge) : 
                           fAmp(amp), 
                           fT0(t0), 
                           fTOT(tot), 
                           fCharge(charge) {}
//------------------------------------------------------------------
/// Standard constructor.
/// \param parameters - vector containing values of the signal 
/// parameters in the following order: amplitude, T0, TOT, charge.
DDSignalBase::DDSignalBase(std::vector <float> parameters) : 
                           fAmp(parameters[0]), 
                           fT0(parameters[1]),
                           fTOT(parameters[2]),
                           fCharge(parameters[3]) {}
//------------------------------------------------------------------
/// Default destructor
DDSignalBase::~DDSignalBase(){ 
}
//------------------------------------------------------------------
/// Sets values of all signals's parameters.
/// \param parameters - vector containing values of the signal 
/// parameters in the following order: amplitude, T0, TOT, charge.
void DDSignalBase::SetAll(std::vector <float> parameters){
 fAmp    = parameters[0];
 fT0     = parameters[1];
 fTOT    = parameters[2];
 fCharge = parameters[3];
 return;
}
//------------------------------------------------------------------
/// Sets all values of signals parameters to their default values. 
void DDSignalBase::Clear(void){
 fAmp    = -100.;
 fT0     = -100.;
 fTOT    = -100.;
 fCharge = -100.; 
 return;
}
//------------------------------------------------------------------
/// Prints details of the DDSignal class object.
void DDSignalBase::Print(void){
  std::cout << "\n\n------------------------------------------------" << std::endl;
  std::cout << "This is Print() for DDSignalBase class object" << std::endl;
  std::cout << "Amplitude = " << fAmp << " mV" << std::endl;
  std::cout << "T0 = " << fT0 << " ns" << std::endl;
  std::cout << "Time over threshold = " << fTOT << " ns" << std::endl;
  std::cout << "Charge (signal integral) = " << fCharge << std::endl;
  std::cout << "------------------------------------------------\n" << std::endl;
  return;
}
//------------------------------------------------------------------