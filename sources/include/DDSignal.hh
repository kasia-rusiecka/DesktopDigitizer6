// *****************************************
// *                                       *
// *           DesktopDigitizer            *
// *             DDSignal.hh               *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2017              *
// *                                       *
// *****************************************

#ifndef __DDSignal_H_
#define __DDSignal_H_ 1
#include "TObject.h"
#include <iostream>

class DDSignal : public TObject{
  
private:
  Float_t fAmp;		///< Amplitude [mV]
  Float_t fT0;		///< Time T0 [ns] 
  Float_t fTOT;		///< Time over threshold [ns]
  Float_t fCharge;	///< Uncalibrated charge 
  Float_t fPE;		///< Calibrated charge [PE]
  
public:
  DDSignal();
  DDSignal(Float_t amplitude, Float_t t0, Float_t tot, Float_t charge, Float_t pe);
  DDSignal(Float_t *parameters);
  ~DDSignal();
  
  /// Sets signal amplitude in mV
  void SetAmplitude(Float_t amplitude){ fAmp = amplitude; };
  /// Sets time T0 in ns
  void SetT0(Float_t t0){ fT0 = t0; };
  /// Sets time over threshold in mV
  void SetTOT(Float_t tot){ fTOT = tot; };
  /// Sets uncalibrated charge, i.e. signal integral
  void SetCharge(Float_t charge){ fCharge = charge; };
  /// Sets calibrated charge in PE
  void SetPE(Float_t pe){ fPE = pe; };
  /// Returns signal amplitude in mV
  Float_t GetAmplitude(void){ return fAmp; };
  /// Returns time T0 in ns
  Float_t GetT0(void){ return fT0; };
  /// Retursns time over threshold in ns
  Float_t GetTOT(void){ return fTOT; };
  /// Returns uncalibrated charge, i.e. signal integral
  Float_t GetCharge(void){ return fCharge; };
  /// Returns calibrated charge in PE
  Float_t GetPE(void){ return fPE; };
  
  void SetAll(Float_t *parameters);
  void Clear(void);
  void Print(void);

  ClassDef(DDSignal,1)
  
};

#endif
