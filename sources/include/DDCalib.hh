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
#include "DDSignalBase.hh"
#include "DDSignalPE.hh"
#include "DDSignalEnergy.hh"
#include "DDCalibEnergy.hh"
#include "DDCalibPE.hh"
#include "DDCalibAmp.hh"
#include "DDCalibEnergy.hh"
#include <string>
#include <fstream>

//

class DDCalib : public TObject{
  
private:
  TString fPath;
  int     fCh;
  int     fNPeaks;
  TFile*  fInputFile; 
  TFile*  fOutputFile;
  TTree*  fTree;

  enum CalibType {
    AmplitudePeakCalib = 0x01,
    ChargePeakCalib = 0x02,
    EnergyPeakCalib = 0x04
  };
  
  unsigned int   fMethod;
  DDCalibPE     *fPECalib;
  DDCalibAmp    *fAmpCalib;
  DDCalibEnergy *fEnergyCalib;
  std::vector <DDCalibBase*> fCalibFunctions;

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