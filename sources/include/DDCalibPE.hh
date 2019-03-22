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
#include "Math/MinimizerOptions.h"
#include "TF1.h"
#include "TH1F.h"
#include <vector>
#include <stdlib.h>
#include <iostream>

/// Class responsible for the PE calibration of the charge spectra.
/// In this method a sum of Gaussian functions is fitted to the 
/// charge spectrum. starting parameters for the fit are chosen based
/// on the list of peaks along with their basing features defined
/// by the user in the configuration file (see DDcalib class reference). 
/// Mean positions of the charge peaks are plotted as a function of 
/// number of PE. Calibration factor is determined in two ways:
/// - based on the linear fit to the peak mean vs. PE graph
/// - as a mean distance between subsequent peaks on the PE spectrum.

class DDCalibPE : public DDCalibBase{
  
public:
  /// Structure representing single peak in the charge spectrum. 
  /// It's members describe key features of the peak, later used 
  /// as starting parameters for the fit in calibration procedure. 
  struct ChargePeak{
    Float_t fConst;   ///< Peak height
    Float_t fMean;    ///< Peak position
    Float_t fSigma;   ///< Peak sigma
  };
  
private:
  Float_t fFitMin;   ///< Fit minimum
  Float_t fFitMax;   ///< Fit maximum
  
  std::vector <ChargePeak> fPeaks;  ///< Vector containing all peaks in the charge 
                                    ///< spectrum for the calibration procedure
  
public:
  /// Standard constructor.
  /// \param npeaks - number of peaks for the calibration
  DDCalibPE(Int_t npeaks) : DDCalibBase(npeaks) {}
  /// Standard destructor.
  ~DDCalibPE() {}
  
  /// Sets fitting range.
  /// \param min - fit minimum
  /// \param max - fit maximum
  void SetFitRange(Float_t min, Float_t max) { fFitMin = min; fFitMax = max; return; }
  void AddPeak(Float_t constant, Float_t mean, Float_t sigma);
  bool Validate(void) const;
  bool Calibrate(TTree *tree, Int_t ch, TFile *file = 0);
  void Print(void);
  
  ClassDef(DDCalibPE,1)
};

#endif
