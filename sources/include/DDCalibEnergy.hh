// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *           DDCalibEnergy.hh            *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2019              *
// *                                       *
// *****************************************

#ifndef __DDCalibEnergy_H_
#define __DDCalibEnergy_H_ 1
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

/// Class responsible for the energy calibration of the charge spectra. 
/// Calibration is performed using peaks defined by the user in the
/// configuration file (see DDCalib class reference) and stored in
/// fPeaks vector. Sum of Gaussian function and polynomial of the 2nd
/// order is fitted to the defined peaks. Subsequently peak ID energy 
/// is plotted as a function of their mean values and linear fit is performed.
/// Parameters of that fit are the parameters of the energy calibration for
/// regular measurements.

class DDCalibEnergy : public DDCalibBase{
  
public:
  /// Structure representing single peak in the charge 
  /// spectrum. Its members identify the peak by the 
  /// true energy in keV (fPeakID) and describe their
  /// key features, used later in the calibration procedure 
  /// as starting parameters.
  struct EnergyPeak{
    Int_t   fPeakID;      ///< Peak ID i.e. it's energy in keV
    Float_t fMean;        ///< Peak position
    Float_t fSigma;       ///< Peak sigma
  };
  
private:
  std::vector <EnergyPeak> fPeaks;	///< Vector containing all peaks for the 
                                        ///< calibration procedure
  
public:
  /// Standard constructor.
  /// \param npeaks - number of peaks for the calibration 
  DDCalibEnergy(Int_t npeaks) : DDCalibBase(npeaks) {}
  /// Standard destructor.
  ~DDCalibEnergy() {}
  
  void AddPeak(Float_t id, Float_t mean, Float_t sigma);
  bool Validate(void) const;
  bool Calibrate(TTree *tree, Int_t ch, TFile *file = 0);
  void Print(void);
  
  ClassDef(DDCalibEnergy,1)
};

#endif