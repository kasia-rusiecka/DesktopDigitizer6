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
    Int_t fPeakID;
    Float_t fMean;
    Float_t fSigma;
  };
  
private:
  std::vector <EnergyPeak> fPeaks;
  
public:
  DDCalibEnergy(Int_t npeaks) : DDCalibBase(npeaks) {}
  
  void AddPeak(Float_t id, Float_t mean, Float_t sigma);
  Bool_t Validate() const override;
  Bool_t Calibrate(TTree *tree, Int_t ch, TFile *file = 0) override;
  
  //ClassDef(DDCalibEnergy,1)
};

#endif