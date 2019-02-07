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

/// 

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