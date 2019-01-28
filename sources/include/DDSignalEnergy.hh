// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *           DDSignalEnergy.hh           *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2019              *
// *                                       *
// *****************************************

#ifndef __DDSignalEnergy_H_
#define __DDSignalEnergy_H_ 1
#include "DDSignalBase.hh"

/// Class derived from DDSignalBase, describing signal 
/// properties. It contains extra parameter representing 
/// charge calibrated to energy in keV.

class DDSignalEnergy : public DDSignalBase{
  
private:
  float fEnergy;	///< Calibrated energy [keV]
  
public:
  DDSignalEnergy();
  DDSignalEnergy(float amp, float t0, float tot, float charge, float energy);
  ~DDSignalEnergy();
  
  /// Sets calibrated energy [keV].
  void SetCalibrated(float energy){ fEnergy = energy; };
  /// Returns calibrated energy [keV].
  float GetCalibrated(void){ return fEnergy; };
  
  void SetAll(std::vector <float> parameters);
  void Clear(void);
  void Print(void);
  
  ClassDef(DDSignalEnergy,1)
};

#endif