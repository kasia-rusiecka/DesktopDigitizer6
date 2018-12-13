// *****************************************
// *                                       *
// *           DesktopDigitizer            *
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
#include "TH1F.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TFitResultPtr.h"
#include "TROOT.h"
#include "TRandom.h"
#include "DDSignal.hh"
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>

using namespace std;

class DDCalib : public TObject{
  
private:
  TString fPath;
  Int_t   fCh;
  TString fMethod;
  Int_t   fNPeaks;
  TFile*  fInputFile; 
  TFile*  fOutputFile;
  TTree*  fTree;
  vector <Double_t> fAmpPeakMin; 
  vector <Double_t> fAmpPeakMax;
  vector <Double_t> fGaussPar;
  vector <Double_t> fGaussFitRange;
  vector <Double_t> fPeakID;
  vector <Double_t> fPeakPos;
  vector <Double_t> fPeakWidth;
  
public:
  DDCalib();
  DDCalib(TString path);
  ~DDCalib();
  
  Bool_t ReadConfig(void);
  Bool_t GetTree(void);
  Bool_t Calibrate(void);
  Bool_t CalibratePEcut(void);
  Bool_t CalibratePEsum(void);
  Bool_t CalibrateEnergy(void);
  void   Print(void);
  
  ClassDef(DDCalib,1)
};

#endif