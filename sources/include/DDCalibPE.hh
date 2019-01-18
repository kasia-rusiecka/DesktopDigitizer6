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
    float fConst;
    float fMean;
    float fSigma;
  };
  
private:
  float fFitMin;
  float fFitMax;
  
  std::vector <GausPeak> fPeaks;
  
public:
  DDCalibPE(int npeaks) : DDCalibBase(npeaks) {}
  
  void SetFitRange(float min, float max) { fFitMin = min; fFitMax = max; }
  void AddPeak(float constant, float mean, float sigma);
  bool Validate() const override;
  bool Calibrate(TTree *tree, int ch, TFile *file = 0) override;
  
  //ClassDef(DDCalibPE,1)
};

#endif