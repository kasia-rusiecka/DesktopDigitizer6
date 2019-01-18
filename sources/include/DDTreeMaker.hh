// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
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
#include "DDSignalPE.hh"
#include "DDSignalEnergy.hh"
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>

class DDTreeMaker : public TObject{
  
private:
  TString fPath;               ///< Path to the data files
  int     fNch;                ///< Number of channels to be analyzed 
  TString fCoding;             ///< Data file coding: BINARY or ASCII
  TString fPolarity;           ///< Signal polarity: NEGATIVE or POSITIVE
  TString fOption;             ///< Analysis mode: CF, FT or both
  TString fIntegrationMode;    //LIMIT - for fixed signal duration, TOT for time over threshold
  float   fLimit;              //for LIMIT integration mode - duration of the signal
  float   fSlope;
  float   fConst;
  TFile*  fFile;                   //results ROOT file
  TTree*  fTreeFT;                 //tree (fixed threshold)
  TTree*  fTreeCF;                 //tree (constant fraction)
  vector <double>  fCalib;         ///< Vector containing calibration factors for charge to PE conversion for each channel
  vector <int>     fChannels;      ///< Vector containing list of channels to be analyzed
  vector <double>  fThresholds;    ///< Vector containing list of thresholds for each analyzed channel
  vector <double>  fFractions;     ///< Vector containing list of fraction for each analyzed channel
  vector <TString> fCalibMethod;

  vector <DDSignalBase*> fSignal;       ///< Vector containing DDSignals for each channel
  vector <TBranch*>      fBranch;       ///< Vector containing branches of the tree
  
  float  fSamples[1024];          ///< Table containing samples (amplitudes) in one signal 
  int    fTime[1024];             ///< Table containing time for each sample of the signal
  
public:
  DDTreeMaker();
  DDTreeMaker(TString path);
  ~DDTreeMaker();
  
  bool  ReadConfig(void);
  bool  FindCoding(void);
  bool  MakeTree(void);
  bool  AnalyzeChannel(int channel, TString mode);
  float FindAmplitude(void);
  float FindT0(int index, float amplitude, TString mode);
  float FindTOT(int index, float amplitude, float t0, TString mode);
  float FindCharge(float t0, float tot);
  float CalibrateCharge(int ch, float charge);
  
  void   Print(void);

  ClassDef(DDTreeMaker,1)
};

#endif
