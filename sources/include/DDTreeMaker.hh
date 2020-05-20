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
#include "TTree.h"
#include "TFile.h"
#include "TString.h"
#include "DDSignal.hh"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>

/// Class for analysis of regular (non-calibration) 
/// measurements. Based on the provided configuration
/// file data recorded with the Desktop Digitizer are 
/// read and anayzed signal after signal. Signals 
/// parameters are saved in the ROOT's tree, in separate
/// branches for each analyzed channel. 
/// 
/// Configuration file "config.txt" should be stored in 
/// the same directory as the data files from the Desktop
/// Digitizer. 
///
/// Configuration file syntax:
/// * NCH - number of channels for analysis, should be 
/// integer between 0 and 16
/// * POL - signal polarity, available optiopns: POSITIVE
/// or NEGATIVE
/// * OPT - analysis mode, available options: CF for constant 
/// fraction analysis, FT for fixed threshold analysis
/// * INT - signal integration method, available options are:
/// * LIMIT for fixed integration time or TOT for time over
/// threshold integration. Additionally, if LIMIT is chosen
/// the integration window in ns should be given as next parameter.
/// * CH_LIST THR FRAC CALIB_MET CALIB_FAC - respectively:
/// channel list, thresholds (for FT analysis), fractions
/// (for CF analysis), calibration method, calibration factor. 
/// Thresholds should be given in Desktop Digitizer ADC channels.
/// Available calibration methods are: PE for PE calibration or 
/// EN for energy calibration. For the PE calibration one calibrating 
/// factor is needed, while for the energy calibration two factors are 
/// expected (slope, constant). All parameters should be given in rows
/// for respective channel numbers.
///
/// All of the parameters can be listed in the configuration file in 
/// any order. 

class DDTreeMaker : public TObject{
  
private:
  TString fPath;               ///< Path to the data files
  Int_t   fNch;                ///< Number of channels to be analyzed 
  TString fCoding;             ///< Data file coding: BINARY or ASCII
  TString fPolarity;           ///< Signal polarity: NEGATIVE or POSITIVE
  TString fOption;             ///< Analysis mode: CF, FT or both
  TString fIntegrationMode;    ///< Method of signal integration: LIMIT - for 
                               ///< fixed signal duration, TOT for time over threshold
  Float_t fLimit;              ///< Duration of the signal (if LIMIT integration chosen)
  
public:
  /// Structure containing calibration details
  /// for a given channel
  struct Calibration{
    TString fCalibMethod;      ///< Calibration method: PE or EN
    Int_t   fCh;               ///< Channel number
    Float_t fEnSlope;          ///< Slope for energy calibration
    Float_t fEnConst;          ///< Constant for energy calibraion
    Float_t fCalibPE;          ///< Calibration factors for charge to PE conversion
  };
  
private:
  TFile*  fFile;                        ///< Results ROOT file
  TTree*  fTreeFT;                      ///< Tree contining data analyzed with Fixed Threshold
  TTree*  fTreeCF;                      ///< Tree containing data analyzed with Constant Fraction
  std::vector <Int_t>     fChannels;    ///< Vector containing list of channels to be analyzed
  std::vector <Double_t>  fThresholds;  ///< Vector containing list of thresholds for each analyzed 
                                        ///< channel. Thresholds given in ADC channels.
  std::vector <Double_t>  fFractions;   ///< Vector containing list of fraction for each analyzed channel

  std::vector <DDSignal*>     fSignal;   ///< Vector containing DDSignals for each channel
  std::vector <TBranch*>      fBranch;   ///< Vector containing branches of the tree
  std::vector <Calibration>   fCalib;    ///< Vector containing calibration details for each channel
  
  Float_t  fSamples[1024];          ///< Table containing samples (amplitudes) in one signal 
  Int_t    fTime[1024];             ///< Table containing time for each sample of the signal
  
  const Int_t gNS = 1024;           ///< Number of samples in one signal
  
public:
  DDTreeMaker();
  DDTreeMaker(TString path);
  ~DDTreeMaker();
  
  bool    ReadConfig(void);
  bool    FindCoding(void);
  Int_t   GetNentries(void);
  bool    MakeTree(void);
  bool    AnalyzeChannel(Int_t channel, TString mode);
  Float_t FindAmplitude(void);
  Float_t FindT0(Int_t index, Float_t amplitude, TString mode);
  Float_t FindTOT(Int_t index, Float_t amplitude, Float_t t0, TString mode);
  Float_t FindCharge(Float_t t0, Float_t tot);
  Float_t CalibrateCharge(Int_t index, Float_t charge);
  Int_t   CheckVeto(Float_t veto_threshold = 10.);
  void    Print(void);

  ClassDef(DDTreeMaker,1)
};

#endif
