// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *            DDSignalBase.hh            *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2017              *
// *                                       *
// *****************************************

#ifndef __DDSignalBase_H_
#define __DDSignalBase_H_ 1
#include "TObject.h"
#include <iostream>
#include <vector>
#include <cstdlib>

/// Class containing basic parameters describing the signal.
/// This is base class for DDSignalPE and DDSignalEnergy,
/// which contain additional information about charge
/// cnverted into PE or energy respectively. 

class DDSignalBase : public TObject{
  
public:
  float fAmp;		///< Amplitude [mV]
  float fT0;		///< Time T0 [ns] 
  float fTOT;		///< Time over threshold [ns]
  float fCharge;	///< Uncalibrated charge [a.u.]
  
public:
  DDSignalBase();
  DDSignalBase(std::vector <float> parameters);
  DDSignalBase(float amp, float t0, float tot, float charge);
  virtual ~DDSignalBase();
  
  /// Sets signal amplitude in mV
  void SetAmplitude(float amp){ fAmp = amp; };
  /// Sets time T0 in ns
  void SetT0(float t0){ fT0 = t0; };
  /// Sets time over threshold in mV
  void SetTOT(float tot){ fTOT = tot; };
  /// Sets uncalibrated charge, i.e. signal integral
  void SetCharge(float charge){ fCharge = charge; };
  /// Sets value of the calibrated charge or energy,
  /// depending on derived type
   virtual void SetCalibrated(float x) = 0;
  /// Returns signal amplitude in mV
  float GetAmplitude(void){ return fAmp; };
  /// Returns time T0 in ns
  float GetT0(void){ return fT0; };
  /// Retursns time over threshold in ns
  float GetTOT(void){ return fTOT; };
  /// Returns uncalibrated charge, i.e. signal integral
  float GetCharge(void){ return fCharge; };
  /// Returns value of calibrated charge or energy, 
  /// depending on derived type
  virtual float GetCalibrated(void) = 0;
  
  virtual void SetAll(std::vector <float> parameters);
  virtual void Clear(void);
  virtual void Print(void);

  ClassDef(DDSignalBase,1) 
};

#endif
