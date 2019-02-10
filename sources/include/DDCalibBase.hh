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

/// Base class for the calibration procedure.  

class DDCalibBase : public TObject{
  
protected:
  const Int_t fNPeaks;	///< Number of peaks for calibration
  
public:
  /// Standard constructor.
  /// \param npeaks - number of peaks for the calibration.
  DDCalibBase(Int_t npeaks) : fNPeaks(npeaks) {}
  /// Standard destructor.
  virtual ~DDCalibBase() {}
  
  /// Checks whether DDCalibBase-derived class object
  /// is correctly defined.
  virtual bool Validate() const = 0;
  /// Performs the calibration procedure. 
  /// \param tree - tree containing data from calibration measurement
  /// \param ch - channel number
  /// \param file - ROOT output file, where calibration results will be stored.
  virtual bool Calibrate(TTree *tree, Int_t ch, TFile *file = 0) = 0;
  /// Prints details of the DDCalibBase-derived class object.
  virtual void Print(void) = 0;
  
  ClassDef(DDCalibBase,1)
};

#endif