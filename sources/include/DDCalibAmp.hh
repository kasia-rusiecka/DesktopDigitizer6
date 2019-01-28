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

// 

class DDCalibAmp : public DDCalibBase{
  
public:
  /// Structure representing single peak in the amplitude spectrum.
  /// It's members identify the peak (fNPE) and are used to impose 
  /// cuts on the charge spectrum in the calibration procedure (fCutMin,
  /// fCutMax).
  struct AmpPeak{
   int   fNPE;          ///< Number of photoelectrons corresponding to the 
                        ///< peak in the amplitude spectrum
   float fCutMin;       ///< Lower range of the peak 
   float fCutMax;       ///< Upper range of the peak 
  };
  
private:
  std::vector <AmpPeak> fPeaks;   ///< Vector containing all the peaks in the amplitude  
                                  ///< spectrum for the calibration procedure
  
public:
  /// Standard constructor 
  /// \param npeaks - number of peaks for the cilbration
  DDCalibAmp(int npeaks) : DDCalibBase(npeaks) {}
  
  void AddPeak(float cutMin, float cutMax);
  bool Validate() const;
  bool Calibrate(TTree *tree, int ch, TFile *file = 0);
  
  ClassDef(DDCalibAmp,1)
};

#endif