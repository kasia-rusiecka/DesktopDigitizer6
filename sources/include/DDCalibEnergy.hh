// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *           DDCalibEnergy.hh            *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2019              *
// *                                       *
// *****************************************

#ifndef __DDCalibEnergy_H_
#define __DDCalibEnergy_H_ 1
#include "DDCalibBase.hh"
#include "TROOT.h"
#include "TRandom.h"
#include "TString.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TH1F.h"
#include "TSpectrum.h"
#include <vector>
#include <stdlib.h>
#include <iostream>

//

class DDCalibEnergy : public DDCalibBase{
  
public:
  /// Structure representing single peak in the charge 
  /// spectrum. Its members identify the peak by the 
  /// true energy in keV (fPeakID) and describe their
  /// key features, used later in the calibration procedure 
  /// as starting parameters.
  struct EnergyPeak{
    int   fPeakID;      ///< Peak ID i.e. it's energy in keV
    float fMean;        ///< Peak position
    float fSigma;       ///< Peak sigma
  };
  
private:
  std::vector <EnergyPeak> fPeaks;	///< Vector containing all peaks for the 
                                        ///< calibration procedure
  
public:
  /// Standard constructor.
  /// \param npeaks - number of peaks for the calibration 
  DDCalibEnergy(int npeaks) : DDCalibBase(npeaks) {}
  
  void AddPeak(float id, float mean, float sigma);
  bool Validate() const;
  bool Calibrate(TTree *tree, int ch, TFile *file = 0);
  
  ClassDef(DDCalibEnergy,1)
};

#endif