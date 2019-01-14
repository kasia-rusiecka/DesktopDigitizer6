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

class DDCalibPE : public DDCalibBase{
  
public:
  struct GausPeak{
    Float_t fConst;
    Float_t fMean;
    Float_t fSigma;
  };
  
private:
  Float_t fFitMin;
  Float_t fFitMax;
  
  std::vector <GausPeak> fPeaks;
  
public:
  DDCalibPE(Int_t npeaks) : DDCalibBase(npeaks) {}
  
  void SetFitRange(Float_t min, Float_t max) { fFitMin = min; fFitMax = max; }
  void AddPeak(Float_t constant, Float_t mean, Float_t sigma);
  Bool_t Validate() const override;
  Bool_t Calibrate(TTree *tree, Int_t ch, TFile *file = 0) override;
  
  //ClassDef(DDCalibPE,1)
};

#endif