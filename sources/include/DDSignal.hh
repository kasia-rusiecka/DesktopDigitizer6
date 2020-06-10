// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *              DDSignal.hh              *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2017              *
// *                                       *
// *****************************************

#ifndef __DDSignal_H_
#define __DDSignal_H_ 1
#include "TObject.h"
#include <iostream>
#include <vector>
#include <cstdlib>

/// Class containing basic parameters describing the signal.

class DDSignal : public TObject{
  
public:
  Int_t fVeto;      ///<
  Float_t fAmp;		///< Amplitude [mV]
  Float_t fT0;		///< Time T0 [ns] 
  Float_t fTOT;		///< Time over threshold [ns]
  Float_t fCharge;	///< Uncalibrated charge [a.u.]
  Float_t fPE;		///< Calibrated charge [P.E.] or [keV]
  Float_t fTmax;
  
public:
  DDSignal();
  DDSignal(Float_t amp, Float_t t0, Float_t tot, Float_t charge, Float_t cal, Int_t veto);
  ~DDSignal();
  
  /// Sets signal amplitude in mV
  void SetAmplitude(Float_t amp){ fAmp = amp; };
  /// Sets time T0 in ns
  void SetT0(Float_t t0){ fT0 = t0; }
  /// Sets time over threshold in mV
  void SetTOT(Float_t tot){ fTOT = tot; };
  /// Sets uncalibrated charge, i.e. signal integral
  void SetCharge(Float_t charge){ fCharge = charge; };
  /// Sets value of charge calibrated to PE or keV
  void SetPE(Float_t cal){ fPE = cal; };
  /// Returns signal amplitude in mV
  void SetVeto(Int_t veto){ fVeto = veto; };
  Float_t GetAmplitude(void){ return fAmp; };
  /// Returns time T0 in ns
  Float_t GetT0(void){ return fT0; };
  /// Retursns time over threshold in ns
  Float_t GetTOT(void){ return fTOT; };
  /// Returns uncalibrated charge, i.e. signal integral
  Float_t GetCharge(void){ return fCharge; };
  /// Returns value of calibrated charge in PE or keV
  Float_t GetPE(void){ return fPE; };
  Int_t GetVeto(void){ return fVeto; };
  
  void SetAll(std::vector <Float_t> parameters);
  void Clear(void);
  void Print(void);

  ClassDef(DDSignal,1) 
};

#endif
