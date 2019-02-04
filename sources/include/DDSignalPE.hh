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

/// Class derived from DDSignalBase, describing signal 
/// properties. It contains extra parameter representing 
/// charge calibrated to PE.

class DDSignalPE : public DDSignalBase{
  
public:
  float fPE;	///< Calibrated charge [PE]

public:  
  DDSignalPE();
  DDSignalPE(float amp, float t0, float tot, float charge, float pe);
  ~DDSignalPE();
  
  /// Sets calibrated charge in PE
  void  SetCalibrated(float pe){ fPE = pe; }
  /// Returns calibrated charge in PE
  float GetCalibrated(void){ return fPE; }
  
  void  SetAll(std::vector <float> parameters);
  void  Clear(void);
  void  Print(void);
  
  ClassDef(DDSignalPE,1)
};

#endif