// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *             DDSignalPE.cc             *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2019              *
// *                                       *
// *****************************************

#include "DDSignalPE.hh"

ClassImp(DDSignalPE);

//------------------------------------------------------------------
/// Deafault constructor.
DDSignalPE::DDSignalPE() : DDSignalBase(), 
                           fPE(-100) {}
//------------------------------------------------------------------
/// Standard constructor.
/// \param amp - signal amplitude [mV]
/// \param t0 - time T0 [ns]
/// \param tot - time over threshold [ns]
/// \param charge - signal integral (uncalibrated charge)
/// \param pe - calibrated charge [PE]
DDSignalPE::DDSignalPE(float amp, float t0, float tot, float charge, float pe) : 
                       DDSignalBase(amp, t0, tot, charge),
                       fPE(pe) {}
//------------------------------------------------------------------
/// Standard constructor.
DDSignalPE::DDSignalPE(std::vector <float> parameters) : 
                       fAmp(parameters[0]),
		       fT0(parameters[1]),
		       fTOT(parameters[2]),
		       fCharge(parameters[3]),
		       fPE(parameters[4]) {}
		       
//------------------------------------------------------------------
/// Standard destructor.
DDSignalPE::~DDSignalPE(){
}
//------------------------------------------------------------------
void DDSignalPE::SetAll(std::vector <float> parameters){
 fAmp    = parameters[0];
 fT0     = parameters[1];
 fTOT    = parameters[2];
 fCharge = parameters[3];
 fPE     = parameters[4];
 return;
}
//------------------------------------------------------------------
/// Sets values of signal parameters to their default values.
void DDSignalPE::Clear(void){
 DDSignalBase::Clear();
 fPE = -100;
 return;
}
//------------------------------------------------------------------
/// Prints details of the DDSignalPE class object.
void DDSignalPE::Print(void){
  std::cout << "\n\n------------------------------------------------" << std::endl;
  std::cout << "This is Print() for DDSignalPE class object" << std::endl;
  std::cout << "Amplitude = " << fAmp << " mV" << std::endl;
  std::cout << "T0 = " << fT0 << " ns" << std::endl;
  std::cout << "Time over threshold = " << fTOT << " ns" << std::endl;
  std::cout << "Charge (signal integral) = " << fCharge << std::endl;
  std::cout << "Calibrated charge = " << fPE << " PE" << std::endl;
  std::cout << "------------------------------------------------\n" << std::endl;
  return;
}
//------------------------------------------------------------------