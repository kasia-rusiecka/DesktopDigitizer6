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
  const int fNPeaks;
  
public:
  DDCalibBase(int npeaks) : fNPeaks(npeaks) {}
  virtual bool Validate() const = 0;
  virtual bool Calibrate(TTree *tree, int ch, TFile *file = 0) = 0;
  
  ClassDef(DDCalibBase,1)
};

#endif