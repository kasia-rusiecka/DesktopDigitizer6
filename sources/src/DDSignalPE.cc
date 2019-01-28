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
/// Standard destructor.
DDSignalPE::~DDSignalPE(){
}
//------------------------------------------------------------------
/// Sets all parameters of the signal.
/// \param parameters - vector containing parameters in the following 
/// order: amplitude, T0, TOT, charge, calibrated charge.
void DDSignalPE::SetAll(std::vector <float> parameters){
 
 std::size_t i = parameters.size();
 if(i<5){
   std::cerr << "##### Error in DDSignalPE::SetAll()" << std::endl;
   std::cerr << "Not sufficient number of parameters! Expected 5 parameters!" << std::endl;
   abort();
 }
  
 DDSignalBase::SetAll(parameters);
 fPE = parameters[4];
 
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