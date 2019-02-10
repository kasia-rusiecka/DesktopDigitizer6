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

/// Class responsible for the PE calibration of the charge spectra.
/// In this method cuts are imposed on the charge spectrum based on 
/// the amplitude spectrum. Gaussian function is fitted to every cut 
/// charge spectrum. Mean of the fitted Gauss is then plotted as a
/// function of PE numbers corresponding to the peak. Calibration
/// factor is determined in two ways:
/// - based on the linear fit to the peak mean vs. PE graph
/// - as a mean distance between subsequent peaks.

class DDCalibAmp : public DDCalibBase{
  
public:
  /// Structure representing single peak in the amplitude spectrum.
  /// It's members identify the peak (fNPE) and are used to impose 
  /// cuts on the charge spectrum in the calibration procedure (fCutMin,
  /// fCutMax).
  struct AmpPeak{
   Int_t   fNPE;          ///< Number of photoelectrons corresponding to the 
                          ///< peak in the amplitude spectrum
   Float_t fCutMin;       ///< Lower range of the peak 
   Float_t fCutMax;       ///< Upper range of the peak 
  };
  
private:
  std::vector <AmpPeak> fPeaks;   ///< Vector containing all the peaks in the amplitude  
                                  ///< spectrum for the calibration procedure
  
public:
  /// Standard constructor 
  /// \param npeaks - number of peaks for the cilbration
  DDCalibAmp(Int_t npeaks) : DDCalibBase(npeaks) {}
  /// Standard destructor.
  ~DDCalibAmp() {}
  
  void AddPeak(Int_t nPE, Float_t cutMin, Float_t cutMax);
  bool Validate(void) const;
  bool Calibrate(TTree *tree, Int_t ch, TFile *file = 0);
  void Print(void);
  
  ClassDef(DDCalibAmp,1)
};

#endif