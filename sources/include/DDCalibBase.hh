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

// 

class DDCalibBase : public TObject{
  
protected:
  const int fNPeaks;	///< Number of peaks for calibration
  
public:
  /// Standard constructor.
  /// \param npeaks - number of peaks for calibration
  DDCalibBase(int npeaks) : fNPeaks(npeaks) {}
  /// Checks whether DDCalibBase-derived class object
  /// is correctly defined
  virtual bool Validate() const = 0;
  /// Performs calibration procedure. 
  /// \param tree - tree containing data from calibration measurement
  /// \param ch - channel number
  /// \param file - ROOT output file, where calibration results will be stored 
  virtual bool Calibrate(TTree *tree, int ch, TFile *file = 0) = 0;
  
  ClassDef(DDCalibBase,1)
};

#endif