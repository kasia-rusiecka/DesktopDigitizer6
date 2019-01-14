// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *             DDCalibAmp.hh             *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2019              *
// *                                       *
// *****************************************

#ifndef __DDCalibAmp_H_
#define __DDCalibAmp_H_ 1
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

class DDCalibAmp : public DDCalibBase{
  
public:
  struct AmpPeak{
   Float_t fCutMin;
   Float_t fCutMax;
  };
  
private:
  std::vector <AmpPeak> fPeaks;
  
public:
  DDCalibAmp(Int_t npeaks) : DDCalibBase(npeaks) {}
  
  void AddPeak(Float_t cutMin, Float_t cutMax);
  Bool_t Validate() const override;
  Bool_t Calibrate(TTree *tree, Int_t ch, TFile *file = 0) override;
  
  //ClassDef(DDCalibAmp,1)
};

#endif