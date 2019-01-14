// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *            DDCalibBase.hh             *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2019              *
// *                                       *
// *****************************************

#ifndef __DDCalibBase_H_
#define __DDCalibBase_H_ 1
#include "TObject.h"
#include "TFile.h"
#include "TTree.h"

class DDCalibBase : public TObject{
  
protected:
  const Int_t fNPeaks;
  
public:
  DDCalibBase(Int_t npeaks) : fNPeaks(npeaks) {}
  virtual Bool_t Validate() const = 0;
  virtual Bool_t Calibrate(TTree *tree, Int_t ch, TFile *file = 0) = 0;
  
  ClassDef(DDCalibBase,1)
};

#endif