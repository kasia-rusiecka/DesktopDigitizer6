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

class DDSignal;

class DDTreeMaker : public TObject{
  
private:
  TString  fPath;		//path to the data files
  Int_t    fNch;		//number of channels
  TString  fCoding;		//binary or ASCII
  TString  fPolarity;		//signal polarity: NEGATIVE or POSITIVE
  TString  fOption;		// CF, FT or both
  TString  fIntegrationMode;	//LIMIT - for fixed signal duration, TOT for time over threshold
  Float_t  fLimit;		//for LIMIT integration mode - duration of the signal
  Double_t fCal0;
  Double_t fCal1;
  Int_t    fChannels[16];	//list of channel numbers
  Double_t fThresholds[16];	//list of thresholds
  Double_t fFractions[16];	//list of fractions
  TFile    *fFile;		//results ROOT file
  TTree    *fTreeFT;		//tree (fixed threshold)
  TTree    *fTreeCF;		//tree (constant fraction)
  DDSignal *fSignal[16];
  TBranch  *fBranch[16];
  
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

  ClassDef(DDTreeMaker,1)

};

#endif
