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
  vector <Double_t> fPeaksMin; 
  vector <Double_t> fPeaksMax;
  vector <Double_t> fGaussPar;
  
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
  
  void Print(void);
  void Reset(void);
  
  ClassDef(DDCalib,1)
};

#endif