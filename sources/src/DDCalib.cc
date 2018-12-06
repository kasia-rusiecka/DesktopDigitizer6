// *****************************************
// *                                       *
// *           DesktopDigitizer            *
// *              DDCalib.cc               *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2018              *
// *                                       *
// ***************************************** 

#include "DDCalib.hh"

ClassImp(DDCalib);

//------------------------------------------------------------------
/// Deafault constructor.
DDCalib::DDCalib() : fPath(""), fMethod(""), fCh(0),
                     fNPeaks(0), fInputFile(NULL){
                         
  cout << "##### Warning in DDCalib constructor! You are using default constructor!" << endl;
}
//------------------------------------------------------------------
/// Standard constructor.
/// \param path - path to the directory containing data files
DDCalib::DDCalib(TString path) : fPath(path), fMethod(""), fCh(0), 
                                 fNPeaks(0), fInputFile(NULL){
                                     
  fInputFile = new TFile(path+"results.root","READ");
  
  if(!fInputFile->IsOpen()){
    throw "##### Exception in DDCalib constructor!"; 
  }
  
  ReadConfig();
}
//------------------------------------------------------------------
/// Default destructor.
DDCalib::~DDCalib(){
  if(fInputFile->IsOpen()) 
    fInputFile->Close();
}
//------------------------------------------------------------------
Bool_t DDCalib::ReadConfig(void){
  
  TString dummy;
  string line;
  
  TString config_name = fPath+"calib_config.txt";
  ifstream config(config_name);
  
  if(!config.is_open()){
    cout << "##### Error in DDCalib::ReadConfig()!" << endl;
    cout << "Could not open config file!" << endl;
    return kFALSE;
  }
  
  while(config.good()){
   
    config >> dummy;
    
    if(dummy.Contains("CH")){
      config >> fCh;
      if(fCh>15){
       cout << "##### Error in DDCalib::ReadConfig()!" << endl;
       cout << "Channel number cannot be larger than 15!" << endl;
       return kFALSE;
      }
    }
    else if(dummy.Contains("CAL")){
      config >> fMethod;
      if(!(fMethod=="PE_CUT" || fMethod=="PE_SUM" || fMethod=="EN")){
       cout << "##### Error in DDCalib::ReadConfig()!" << endl;
       cout << "Unknown calibration method!" << endl;
       return kFALSE;
      }
    }
    else if(dummy.Contains("NPEAKS")){
      config >> fNPeaks;
      if(fNPeaks<2){
       cout << "##### Error in DDCalib::ReadConfig()!" << endl;
       cout << "At least 2 peaks needed for the calibration" << endl;
       return kFALSE;
      }
      fPeaksMin.reserve(fNPeaks);
      fPeaksMax.reserve(fNPeaks);
    }
    else if(dummy.Contains("PEAK_MIN")){
      getline(config,line);
      for(Int_t i=0; i<fNPeaks; i++){
        config >> fPeaksMin[i] >> fPeaksMax[i];
      }
    }
    else{
      cout << "##### Warning in DDCalib::ReadConfig()!" << endl;
      cout << "Unknown syntax, skipping line" << endl;
      getline(config,line);
    }
  }
  
  return kTRUE;
}
//------------------------------------------------------------------
Bool_t DDCalib::Calibrate(void){
  
  return kTRUE;
}
//------------------------------------------------------------------
Bool_t DDCalib::CalibratePEcut(void){
  
  return kTRUE;
}
//------------------------------------------------------------------
Bool_t DDCalib::CalibratePEsum(void){
  
  return kTRUE;
}
//------------------------------------------------------------------
Bool_t DDCalib::CalibrateEnergy(void){
  
  return kTRUE;
}
//------------------------------------------------------------------
/// Prints details of the DDCalib class object.
void DDCalib::Print(void){
  cout << "\n\n------------------------------------------------" << endl;
  cout << "This is DDCalib class objecto for the measurement: " << fPath << endl;
  cout << "Analyzed channel: " << fCh << endl;
  cout << "Calibration method: " << fMethod << endl;
  return;
}
//------------------------------------------------------------------