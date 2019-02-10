// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *              DDCalib.hh               *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2018              *
// *                                       *
// ***************************************** 

#ifndef __DDCalib_H_
#define __DDCalib_H_ 1
#include "TObject.h"
#include "TString.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TSpectrum.h"
#include "DDSignal.hh"
#include "DDCalibEnergy.hh"
#include "DDCalibPE.hh"
#include "DDCalibAmp.hh"
#include "DDCalibEnergy.hh"
#include <string>
#include <fstream>

/// Class performing calibration based on the provided configuration file
/// "calib_config.txt". Configuration file should be in the same directory
/// as the analyzed data. This class accesses already digitized trees, so 
/// digitization with digit and calibration factors 1 should be run before
/// calibration. 
///
/// Configuration file syntax:
/// - CH - channel number
/// - CAL - calibration method. Available options are: PE_CUT - PE calibration
/// based on cuts on the amplitude spectrum, PE_SUM - PE calibration with the
/// sum of Gaussians fitted to the charge spectrum, EN - energy calibration.
/// - NPEAKS - number of peaks in the calibration procedure
/// - PEAK_MIN PEAK_MAX - for the PE_CUT method, lower and upper ranges of the
/// peaks in the amplitude spectrum
/// - FIT_MIN FIT_MAX - for the PE_SUM method, lower and upper range of the fit
/// - CONST MEAN SIGMA - for the PE_SUM method, parameters of the peaks in the
/// charge spectrum
/// - PEAK_ID PEAK_POS PEAK_WIDTH - for the EN method, parameters of the peaks 
/// in the charge spectrum, where ID is energy in keV
///
/// Both methods of the PE calibration can be used simultaneusly. Channel
/// number, calibration method and number of peaks should be listed in the 
/// beginning of the file, while the other parameters can be listed in any order.
/// Results of the calibration are saved in ROOT files in form of histograms with
/// fitted functions and calibration curves. 

class DDCalib : public TObject{
  
private:
  TString fPath;        ///< Path to the directory containing data files
  Int_t   fCh;          ///< Number of the channel to be analyzed
  Int_t   fNPeaks;      ///< Number of peaks for the calibration procedure
  TFile*  fInputFile;   ///< Input ROOT file containing data from the calibration measurement
  TFile*  fOutputFile;  ///< Output ROOT containing results of the calibration
  TTree*  fTree;        ///< Tree containing data from the calibration measurememnt 

  /// Enumeration which elements represent available calibration methods:
  /// - AmplitudePeakCalib / PE_CUT / PE calibratin with the cut based on amplitude
  /// - ChargePeakCalib / PE_SUM / PE calibration with the sum of Gaussians
  /// - EnergyPeakCalib / EN / energy calibration
  enum CalibType {
    AmplitudePeakCalib = 0x01,
    ChargePeakCalib = 0x02,
    EnergyPeakCalib = 0x04
  };
  
  unsigned int   fMethod;       ///< Calibration method
  DDCalibPE     *fPECalib;      ///< Template object for the PE calibration
  DDCalibAmp    *fAmpCalib;     ///< Template object for the PE calibration with the 
                                ///< cuts based on signals amplitude
  DDCalibEnergy *fEnergyCalib;  ///< Template object for the energy calibration
  std::vector <DDCalibBase*> fCalibFunctions;   ///< Vector containing calibration objects

public:
  DDCalib(TString path);
  ~DDCalib();
  
  bool ReadConfig(void);
  bool GetTree(void);
  bool Calibrate(void);
  void Print(void);
  
  ClassDef(DDCalib,1)
};

#endif
