// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *              DDSignal.cc              *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2017              *
// *                                       *
// *****************************************

#include "DDSignal.hh"
#include <cmath>

ClassImp(DDSignal);

//------------------------------------------------------------------
/// Default constructor.
DDSignal::DDSignal() : fAmp(-100),
                       fT0(-100),
                       fTOT(-100),
                       fCharge(-100),
                       fPE(-100),
                       fVeto(0) {}
//------------------------------------------------------------------
/// Standard constructor.
/// \param amp - signal amplitude [mV]
/// \param t0 - time T0 [ns]
/// \param tot - time over threshold [mV]
/// \param charge - signal integral (uncalibrated charge) [a.u.]
/// \param cal - calibrated charge [PE] or [keV]
DDSignal::DDSignal(Float_t amp, Float_t t0, Float_t tot, Float_t charge, Float_t cal, Int_t veto) : 
                   fAmp(amp),
                   fT0(t0),
                   fTOT(tot),
                   fCharge(charge),
                   fPE(cal),
                   fVeto(veto) {}
//------------------------------------------------------------------
/// Default destructor.
DDSignal::~DDSignal(){
}
//------------------------------------------------------------------
/// Sets values of all signals's parameters.
/// \param parameters - vector containing values of the signal 
/// parameters in the following order: amplitude, T0, TOT, charge.
void DDSignal::SetAll(std::vector <Float_t> parameters){
  
 std::size_t i = parameters.size();
 if(i!=6){
   std::cerr << "##### Error in DDSignalEnergy::SetAll()" << std::endl;
   std::cerr << "Not sufficient number of parameters! Expected 6 parameters!" << std::endl;
   abort();
 }
 
 fAmp    = parameters[0];
 fT0     = parameters[1];
 fTOT    = parameters[2];
 fCharge = parameters[3];
 fPE     = parameters[4];
 
 if(fabs(parameters[5])<1E-8)
     fVeto = 0;
 else 
     fVeto = 1;
 
 return;
}
//------------------------------------------------------------------
/// Sets all values of signals parameters to their default values. 
void DDSignal::Clear(void){
 fAmp    = -100.;
 fT0     = -100.;
 fTOT    = -100.;
 fCharge = -100.;
 fPE     = -100.; 
 fVeto   = 0;
 return;
}
//------------------------------------------------------------------
/// Prints details of the DDSignal class object.
void DDSignal::Print(void){
  std::cout << "\n\n------------------------------------------------" << std::endl;
  std::cout << "This is Print() for DDSignalBase class object" << std::endl;
  std::cout << "Amplitude = " << fAmp << " mV" << std::endl;
  std::cout << "T0 = " << fT0 << " ns" << std::endl;
  std::cout << "Time over threshold = " << fTOT << " ns" << std::endl;
  std::cout << "Charge (signal integral) = " << fCharge << std::endl;
  std::cout << "Calibrated charge = " << fPE << " P.E. / keV" << std::endl;
  std::cout << "Veto flag: " << fVeto << std::endl;
  std::cout << "------------------------------------------------\n" << std::endl;
  return;
}
//------------------------------------------------------------------
