// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *             DDSignalPE.hh             *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2019              *
// *                                       *
// *****************************************

#ifndef __DDSignalPE_H_
#define __DDSignalPE_H_ 1
#include "DDSignalBase.hh"

class DDSignalPE : public DDSignalBase{
  
private:
  float fPE;	///< Calibrated charge [PE]

public:  
  DDSignalPE();
  DDSignalPE(float amp, float t0, float tot, float charge, float pe);
  DDSignalPE(std::vector <float> parameters);
  ~DDSignalPE();
  
  ///Sets calibrated charge in PE
  void  SetPE(float pe){ fPE = pe; };
  ///Returns calibrated charge in PE
  float GetPE(void){ return fPE; };
  
  void  SetAll(std::vector <float> parameters);
  void  Clear(void);
  void  Print(void);
  
  ClassDef(DDSignalPE,1)
};

#endif