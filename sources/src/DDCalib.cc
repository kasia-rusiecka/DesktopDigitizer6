// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *              DDCalib.cc               *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2018              *
// *                                       *
// ***************************************** 

#include "DDCalib.hh"

ClassImp(DDCalib);

//------------------------------------------------------------------
/// Standard constructor.
/// \param path - path to the directory containing data files
DDCalib::DDCalib(TString path) : fPath(path), 
                                 fMethod(0), 
                                 fCh(0), 
                                 fNPeaks(0), 
                                 fInputFile(nullptr),
                                 fOutputFile(nullptr) {
   
  ReadConfig();
  
  fInputFile = new TFile(path+"results.root","READ");
  fOutputFile = new TFile(path+"calib_" + (
    fMethod == AmplitudePeakCalib ? "PE_CUT" :
      fMethod == ChargePeakCalib ? " PE_SUM" :
        fMethod == EnergyPeakCalib ? "EN" : "NONE"
    ) + ".root","RECREATE");
  
  if(!(fInputFile->IsOpen() || fOutputFile->IsOpen())){
    throw "##### Exception in DDCalib constructor!"; 
  }
  
  Bool_t tree_flag = GetTree();
  std::cout << "Tree access flag: " << tree_flag << std::endl; 
}
//------------------------------------------------------------------
/// Default destructor.
DDCalib::~DDCalib(){
  if(fOutputFile->IsOpen()) fOutputFile->Close();
  if(fInputFile->IsOpen()) fInputFile->Close();
}
//------------------------------------------------------------------
Bool_t DDCalib::GetTree(void){
  
  fTree = (TTree*)fInputFile->Get("tree_cf");
  
  if(fTree!=nullptr){
    std::cout << "\nIn DDCalib::GetTree(). Accessing Constant Fraction data..." << std::endl;
    return kTRUE;
  }
  else{
    fTree = (TTree*)fInputFile->Get("tree_ft");
    if(fTree==nullptr){
     std::cerr << "\n##### Error in DDCalib::GetTree()! Could not access data!"<< std::endl;
     return kFALSE;
    }
    else{
        std::cout << "\nIn DDCalib::GetTree(). Accessing Fixed Threshold data..." << std::endl;
        return kTRUE;
    }
  }
}
//------------------------------------------------------------------
Bool_t DDCalib::ReadConfig(void){
  
  TString dummy;
  std::string line;
  
  TString config_name = fPath+"calib_config.txt";
  std::ifstream config(config_name);
  
  if(!config.is_open()){
    std::cerr << "##### Error in DDCalib::ReadConfig()!" << std::endl;
    std::cerr << "Could not open config file!" << std::endl;
    abort();
  }
  
  std::cout << "\n---------- Reading config file " << config_name << std::endl;

  while(config.good()){
   
    config >> dummy;
    
    if(dummy.Contains("#")){		//comment
      getline(config,line);
    }
    else if(dummy.Contains("CH")){	//channel number
      config >> fCh;
      if(fCh>15){
        std::cerr << "##### Error in DDCalib::ReadConfig()!" << std::endl;
        std::cout << "Channel number cannot be larger than 15!" << std::endl;
        abort();
      }
    }
    else if(dummy.Contains("CAL")){	//callibration method
      TString method;
      config >> method;
      if(method=="PE_CUT")       fMethod |= AmplitudePeakCalib;
      else if(method=="PE_SUM")  fMethod |= ChargePeakCalib;
      else if(method=="EN")      fMethod |= EnergyPeakCalib;
      else {
        std::cerr << "##### Error in DDCalib::ReadConfig()!" << std::endl;
        std::cerr << "Unknown calibration method!" << std::endl;
        abort();
      }
    }
    else if(dummy.Contains("NPEAKS")){	//number of peaks
      config >> fNPeaks;
      if(fNPeaks<2){
        std::cerr << "##### Error in DDCalib::ReadConfig()!" << std::endl;
        std::cerr << "At least 2 peaks needed for the calibration" << std::endl;
        abort();
      }
    }
    else if(dummy.Contains("CONST")){	//parameters for PE/Gaus callibration
      getline(config,line);
      if(fMethod & ChargePeakCalib == 0){
        std::cerr << "##### Error in DDCalib::ReadConfig()! Incorrect syntax!" << std::endl;
        abort();
      }
      if(!fPECalib)
        fPECalib = new DDCalibPE(fNPeaks);
      for(Int_t i=0; i<fNPeaks; i++){
        Float_t par1, par2, par3;
        config >> par1 >> par2 >> par3;
        fPECalib->AddPeak(par1, par2, par3);
      }
      if(fPECalib->Validate()){
        fCalibFunctions.push_back(fPECalib);
        fPECalib = nullptr;
      }
    }
    else if(dummy.Contains("FIT_MIN")){	//fit range for PE/Gaus callibration
      getline(config,line); 
      if(fMethod & ChargePeakCalib == 0){
        std::cerr << "##### Error in DDCalib::ReadConfig()! Incorrect syntax!" << std::endl;
        abort();
      }
      if(!fPECalib)
	  fPECalib = new DDCalibPE(fNPeaks);
      Float_t minFit, maxFit;
      config >> minFit >> maxFit;
      fPECalib->SetFitRange(minFit, maxFit);
      if(fPECalib->Validate()){
        fCalibFunctions.push_back(fPECalib);
        fPECalib = nullptr;
      }
    }
    else if(dummy.Contains("PEAK_MIN")){	//parameters for PE/Amp callibration
      getline(config,line);
      if(fMethod & AmplitudePeakCalib == 0){
        std::cerr << "##### Error in DDCalib::ReadConfig()! Incorrect syntax!" << std::endl;
        abort();
      }
      fAmpCalib = new DDCalibAmp(fNPeaks);
      Float_t minAmp, maxAmp;
      for(Int_t i=0; i<fNPeaks; i++){
        config >> minAmp >> maxAmp;
	fAmpCalib->AddPeak(minAmp, maxAmp);
      }
      if(fAmpCalib->Validate()){
	fCalibFunctions.push_back(fAmpCalib);
        fAmpCalib = nullptr;
      }
      else{
	std::cerr << "##### Error in DDCalib::ReadConfig()!" << std::endl;
	std::cerr << "Problem with DDCalibAmp object!" << std::endl;
	abort();
      }
    }
    else if(dummy.Contains("PEAK_ID")){		//parameters for energy callibration
      getline(config,line);
      if(fMethod & EnergyPeakCalib == 0){
        std::cerr << "##### Error in DDCalib::ReadConfig()! Incorrect syntax!" << std::endl;
        abort();
      }
      fEnergyCalib = new DDCalibEnergy(fNPeaks);
      Float_t id, mean, sigma;
      for(Int_t i=0; i<fNPeaks; i++){
        config >> id >> mean >> sigma;
	fEnergyCalib->AddPeak(id, mean, sigma);
      }
      if(fEnergyCalib->Validate()){
	fCalibFunctions.push_back(fEnergyCalib);
        fEnergyCalib = nullptr;
      }
      else{
	std::cerr << "##### Error in DDCalib::ReadConfig()!" << std::endl;
	std::cerr << "Problem with DDCalibEnergy object!" << std::endl;
	abort();
      }
    }
    else{
      std::cout << "##### Warning in DDCalib::ReadConfig()!" << std::endl;
      std::cout << "Unknown syntax, skipping line: " << std::endl;
      getline(config,line);
      std::cout << dummy << "\t" << line << std::endl;
    }
  }
  
  config.close();
  
  return kTRUE;
}
//------------------------------------------------------------------
Bool_t DDCalib::Calibrate(void){

  size_t s = fCalibFunctions.size();
  for(size_t i = 0; i < s; ++i)
  {
    Bool_t res = fCalibFunctions[i]->Calibrate(fTree, fCh, fOutputFile);
    if (!res) {
      // Error handling
    }
  }

  return kTRUE;
}
//------------------------------------------------------------------
/// Prints details of the DDCalib class object.
void DDCalib::Print(void){
  std::cout << "\n\n------------------------------------------------" << std::endl;
  std::cout << "This is DDCalib class object for the measurement: " << fPath << std::endl;
  std::cout << "Analyzed channel: " << fCh << std::endl;
  std::cout << "Calibration method: " << fMethod << std::endl;
  std::cout << "Input file: " << fInputFile->GetName() << std::endl;
  std::cout << "Results saved in: " << fOutputFile->GetName() << std::endl;
  std::cout << "------------------------------------------------\n" << std::endl;
  return;
}
//------------------------------------------------------------------