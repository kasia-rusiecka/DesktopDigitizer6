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

class DDCalibEnergy : public DDCalibBase{
  
public:
  struct EnergyPeak{
    int fPeakID;
    float fMean;
    float fSigma;
  };
  
private:
  std::vector <EnergyPeak> fPeaks;
  
public:
  DDCalibEnergy(int npeaks) : DDCalibBase(npeaks) {}
  
  void AddPeak(float id, float mean, float sigma);
  bool Validate() const override;
  bool Calibrate(TTree *tree, int ch, TFile *file = 0) override;
  
  //ClassDef(DDCalibEnergy,1)
};

#endif