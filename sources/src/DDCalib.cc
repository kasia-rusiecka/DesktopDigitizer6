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
DDCalib::DDCalib() : fPath(""), 
                     fMethod(""), 
                     fCh(0),
                     fNPeaks(0), 
                     fInputFile(NULL) {
                         
  cout << "##### Warning in DDCalib constructor! You are using default constructor!" << endl;
}
//------------------------------------------------------------------
/// Standard constructor.
/// \param path - path to the directory containing data files
DDCalib::DDCalib(TString path) : fPath(path), 
                                 fMethod(""), 
                                 fCh(0), 
                                 fNPeaks(0), 
                                 fInputFile(NULL),
                                 fOutputFile(NULL) {
                                     
  fInputFile = new TFile(path+"results.root","READ");
  fOutputFile = new TFile(path+"calib.root","RECREATE");
  
  if(!(fInputFile->IsOpen() || fOutputFile->IsOpen())){
    throw "##### Exception in DDCalib constructor!"; 
  }
  
  ReadConfig();
}
//------------------------------------------------------------------
/// Default destructor.
DDCalib::~DDCalib(){
  if(fInputFile->IsOpen()) fInputFile->Close();
  if(fOutputFile->IsOpen()) fOutputFile->Close();
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
  
  cout << "---------- Reading config file " << config_name << endl;
  
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
      cout << "Unknown syntax, skipping line: " << endl;
      getline(config,line);
      cout << line << endl;
    }
  }
  
  config.close();
  
  return kTRUE;
}
//------------------------------------------------------------------
Bool_t DDCalib::Calibrate(void){
  
  if(fMethod=="PE_CUT") 
    CalibratePEcut();
  else if(fMethod=="PE_SUM")
    CalibratePEsum();
  else if(fMethod=="EN")
    CalibrateEnergy();
  else{
    cout << "##### Error in DDCalib::Calibrate()! Unknown calibration method!" << endl;
    return kFALSE;
  }
  
  return kTRUE;
}
//------------------------------------------------------------------
Bool_t DDCalib::CalibratePEcut(void){
  
  return kTRUE;
}
//------------------------------------------------------------------
Bool_t DDCalib::CalibratePEsum(void){
  
  cout << "---------- Channel " << fCh << " calibration..." << endl;
  
  TString hname = Form("ch%i_PE_calib",fCh);
  TH1D *hist = new TH1D(hname,hname,2000,0,1000);
  
  TString selection = Form("ch_%i.fCharge>>hist",fCh);
  TString cut = Form("ch_%i.fCharge>0",fCh);
  fTree->Draw(selection,cut);
  
  TString formula;
  for(int i=0; i<fNPeaks; i++){
    if(i!=fNPeaks-1)
     formula.Append(Form("gaus(%i)+",3*i)); 
    else 
     formula.Append(Form("gaus(%i)",3*i));
  }
  
  cout << formula << endl;
  TF1 *fun = new TF1("fun",formula,0,100);
  
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
  cout << "Input file: " << fInputFile->GetName() << endl;
  cout << "Results saved in: " << fOutputFile->GetName() << endl;
  return;
}
//------------------------------------------------------------------