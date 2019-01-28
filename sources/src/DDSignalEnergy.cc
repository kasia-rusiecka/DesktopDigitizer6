// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *           DDSignalEnergy.cc           *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2019              *
// *                                       *
// *****************************************

#include "DDSignalEnergy.hh"

ClassImp(DDSignalEnergy);

//------------------------------------------------------------------
/// Default constructor.
DDSignalEnergy::DDSignalEnergy() : DDSignalBase(),
                                   fEnergy(-100) {}
//------------------------------------------------------------------
/// Standard constructor.
/// \param amp - signal amplitude [mV]
/// \param t0 - time t0 [ns]
/// \param tot - time over threshold [ns]
/// \param charge - signal integral (uncalibrated charge) [a.u.]
/// \param energy - energy [keV]
DDSignalEnergy::DDSignalEnergy(float amp, float t0, float tot, float charge, float energy) : 
                               DDSignalBase(amp, t0, tot, charge),
                               fEnergy(energy) {}
//------------------------------------------------------------------
/// Standard destructor.
DDSignalEnergy::~DDSignalEnergy(){
}
//------------------------------------------------------------------
/// Sets all parameters of the signal.
/// \param parameters - vector containing parameters in the following 
/// order: amplitude, T0, TOT, charge, energy.
void DDSignalEnergy::SetAll(std::vector <float> parameters){
 
 std::size_t i = parameters.size();
 if(i<5){
   std::cerr << "##### Error in DDSignalEnergy::SetAll()" << std::endl;
   std::cerr << "Not sufficient number of parameters! Expected 5 parameters!" << std::endl;
   abort();
 }
 
 DDSignalBase::SetAll(parameters);
 fEnergy = parameters[4];
 
 return;
}
//------------------------------------------------------------------
/// Sets values of signal parameters to their default values.
void DDSignalEnergy::Clear(void){
 DDSignalBase::Clear();
 fEnergy = -100;
 return;
}
//------------------------------------------------------------------
/// Prints details of the DDSignalEnergy class object.
void DDSignalEnergy::Print(void){
 std::cout << "\n\n------------------------------------------------" << std::endl;
 std::cout << "This is Print() for DDSignalPE class object" << std::endl;
 std::cout << "Amplitude = " << fAmp << " mV" << std::endl;
 std::cout << "T0 = " << fT0 << " ns" << std::endl;
 std::cout << "Time over threshold = " << fTOT << " ns" << std::endl;
 std::cout << "Charge (signal integral) = " << fCharge << std::endl;
 std::cout << "Energy = " << fEnergy << " keV" << std::endl;
 std::cout << "------------------------------------------------\n" << std::endl;
 return; 
}
//------------------------------------------------------------------