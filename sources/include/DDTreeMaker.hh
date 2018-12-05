// *****************************************
// *                                       *
// *           DesktopDigitizer            *
// *            DDTreeMaker.hh             *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2017              *
// *                                       *
// *****************************************

#ifndef __DDTreeMaker_H_
#define __DDTreeMaker_H_ 1
#include "TObject.h"
#include "TH1F.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"
#include "DDSignal.hh"
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>

class DDSignal;

using namespace std;

class DDTreeMaker : public TObject{
  
private:
  TString  fPath;		///< Path to the data files
  Int_t    fNch;		///< Number of channels to be analyzed 
  TString  fCoding;		///< Data file coding: BINARY or ASCII
  TString  fPolarity;		///< Signal polarity: NEGATIVE or POSITIVE
  TString  fOption;		///< Analysis mode: CF, FT or both
  TString  fIntegrationMode;	//LIMIT - for fixed signal duration, TOT for time over threshold
  Float_t  fLimit;		//for LIMIT integration mode - duration of the signal
  
  
  vector <Double_t> fCalib;	///< Vector containing calibration factors for charge to PE conversion for each channel
  vector <Int_t> fChannels;	///< Vector containing list of channels to be analyzed
  vector <Double_t> fThresholds;	///< Vector containing list of thresholds for each analyzed channel
  vector <Double_t> fFractions;	///< Vector containing list of fraction for each analyzed channel
  TFile    *fFile;		//results ROOT file
  TTree    *fTreeFT;		//tree (fixed threshold)
  TTree    *fTreeCF;		//tree (constant fraction)
  vector <DDSignal*> fSignal;
  vector <TBranch*>  fBranch;
  
  Float_t  fSamples[1024];
  Int_t    fTime[1024];
  
public:
  DDTreeMaker();
  DDTreeMaker(TString path);
  ~DDTreeMaker();
  
  Bool_t ReadConfig(TString path);
  Bool_t FindCoding(TString path);
  
  Bool_t MakeTree(void);
  Bool_t AnalyzeChannel(Int_t channel, TString mode);
  Float_t FindAmplitude(void);
  Float_t FindT0(Int_t index, Float_t amplitude, TString mode);
  Float_t FindTOT(Int_t index, Float_t amplitude, Float_t t0, TString mode);
  Float_t FindCharge(Float_t t0, Float_t tot);
  
  Bool_t FindBaseLine(Int_t channel, Bool_t saving, Double_t &line, Double_t &sigma);
  Bool_t SaveBaseLines(void);
  void   Print(void);
  void   Reset(void);

  ClassDef(DDTreeMaker,1)

};

#endif
