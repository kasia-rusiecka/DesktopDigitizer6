// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *              DDCalibPE.hh             *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2019              *
// *                                       *
// *****************************************

#ifndef __DDCalibPE_H_
#define __DDCalibPE_H_ 1
#include "DDCalibBase.hh"
#include "TROOT.h"
#include "TRandom.h"
#include "TString.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TH1F.h"
#include <vector>
#include <stdlib.h>
#include <iostream>

//

class DDCalibPE : public DDCalibBase{
  
public:
  /// Structure representing single peak in the charge spectrum. 
  /// It's members describe key features of the peak, later used 
  /// as starting parameters for the fit in calibration procedure. 
  struct ChargePeak{
    float fConst;   ///< Peak height
    float fMean;    ///< Peak position
    float fSigma;   ///< Peak sigma
  };
  
private:
  float fFitMin;   ///< Fit minimum
  float fFitMax;   ///< Fit maximum
  
  std::vector <ChargePeak> fPeaks;  ///< Vector containing all peaks in the charge 
                                    ///< spectrum for the calibration procedure
  
public:
  /// Standard constructor.
  /// \param npeaks - number of peaks for the calibration
  DDCalibPE(int npeaks) : DDCalibBase(npeaks) {}
  
  /// Sets fitting range.
  /// \param min - fit minimum
  /// \param max - fit maximum
  void SetFitRange(float min, float max) { fFitMin = min; fFitMax = max; }
  void AddPeak(float constant, float mean, float sigma);
  bool Validate() const;
  bool Calibrate(TTree *tree, int ch, TFile *file = 0);
  
  ClassDef(DDCalibPE,1)
};

#endif