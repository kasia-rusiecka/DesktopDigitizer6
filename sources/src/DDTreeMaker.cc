// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *            DDTreeMaker.cc             *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2017              *
// *                                       *
// *****************************************

#include "DDTreeMaker.hh"

ClassImp(DDTreeMaker);

const int gNS = 1024;	//number of samples in one signal

//------------------------------------------------------------------
/// Default constructor. 
DDTreeMaker::DDTreeMaker() : fPath(""), 
                             fCoding(""), 
                             fPolarity(""), 
                             fOption(""),
                             fIntegrationMode(""), 
                             fNch(0), fLimit(0),
                             fFile(nullptr), 
                             fTreeFT(nullptr), 
                             fTreeCF(nullptr) {
       
  std::cout << "##### Warning in DDTreeMaker Constructor!" << std::endl;
  std::cout << "You are using default constructor!" << std::endl;
}
//------------------------------------------------------------------
/// Standard constructor (recommended). 
/// \param path - path to the directory containing data for analysis
DDTreeMaker::DDTreeMaker(TString path) : fPath(path), 
                                         fCoding(""), 
                                         fPolarity(""), 
                                         fOption(""),
                                         fIntegrationMode(""), 
                                         fNch(0), 
                                         fLimit(0),
                                         fFile(nullptr), 
                                         fTreeFT(nullptr), 
                                         fTreeCF(nullptr) {
  
  TString fname = path+"results.root";
  fFile = new TFile(fname,"RECREATE");
  ReadConfig();
  FindCoding();
  
  for(Int_t i=0; i<fNch; i++){
    if(fCalib[i].fCalibMethod.Contains("PE"))
      fSignal.push_back(new DDSignalPE());
    else if(fCalib[i].fCalibMethod.Contains("EN"))
      fSignal.push_back(new DDSignalEnergy());
  }
}
//------------------------------------------------------------------
/// Default destructor.
DDTreeMaker::~DDTreeMaker(){
  if(fFile->IsOpen()) fFile->Close();
}
//------------------------------------------------------------------
/// Reads content of the config file config.txt and sets values to 
/// private members of the class necessary for the analysis.
Bool_t DDTreeMaker::ReadConfig(void){
 
  TString dummy;
  std::string line;
  
  TString config_name = fPath+"config.txt";
  std::ifstream config(config_name);
  
  if(!config.is_open()){
   std::cerr << "##### Error in DDTreeMaker::ReadConfig! Could not open config file!" << std::endl;
   return false;
  }
  
  while(config.good()){
   
    config >> dummy;
    
    if(dummy.Contains("NCH")){
     config >> fNch;
     if(fNch>16 && fNch<0){
      std::cerr << "##### Error in DDTreeMaker::ReadConfig()!" << std::endl;
      std::cerr << "Number of channels cannot be larger than 16 or smaller than 0!" << std::endl;
      return false;
     }
     fChannels.resize(fNch);
     fThresholds.resize(fNch);
     fFractions.resize(fNch);
    }
    else if(dummy.Contains("POL")){
     config >> fPolarity;
     if(!(fPolarity=="NEGATIVE" || fPolarity=="POSITIVE")){
      std::cerr << "##### Error in DDTreeMaker::ReadConfig()!" << std::endl;
      std::cerr << "Unknown signal polarity! Possible options: NEGATIVE or POSITIVE" << std::endl;
      return false;
     }
    }
    else if(dummy.Contains("OPT")){
     config >> fOption;
     if(!fOption.Contains("FT") && !fOption.Contains("CF")){
      std::cerr << "##### Error in DDTreeMaker::ReadConfig!" << std::endl;
      std::cerr << "Unknown analysis mode! Possible options: FT or CF" << std::endl;
      return false;
      }
    }
    else if(dummy.Contains("INT")){
     config >> fIntegrationMode; 
     if(!(fIntegrationMode=="LIMIT" || fIntegrationMode=="TOT")){
       std::cerr << "##### Error in DDTreeMaker::ReadConfig!" << std::endl;
       std::cerr << "Unknown integration mode! Possible options: LIMIT or TOT" << std::endl;
       return false;
     }
     if(fIntegrationMode=="LIMIT"){
       config >> fLimit;
     }
    }
    else if(dummy.Contains("CH_LIST")){
     getline(config,line);

     for(Int_t i=0; i<fNch; i++){
      fCalib.push_back(Calibration());
      config >> fChannels[i] >> fThresholds[i] >> fFractions[i] >> fCalib[i].fCalibMethod;
      fThresholds[i] = fThresholds[i]/4.096;   // recalculating from ADC channels to mV
      if(fCalib[i].fCalibMethod=="PE"){ 
	config >> fCalib[i].fCalibPE;
      }
      else if(fCalib[i].fCalibMethod=="EN"){
	config >> fCalib[i].fEnSlope >> fCalib[i].fEnConst;
      }
      else {
	std::cerr << "##### Error in DDTreeMaker::ReadConfig()" << std::endl;
	std::cerr << "Unknown calibration method. Possible options: PE or EN" << std::endl;
	return false;
      }
     }
    }
    else{
     std::cout << "##### Warning in DDTreeMaker::ReadConfig()!" << std::endl;
     std::cout << "Unknown syntax, skipping line: " << std::endl;
     getline(config,line);
     std::cout << line << std::endl;
    }
  }
  
  config.close();

  return true;
}
//------------------------------------------------------------------
/// Checks data files coding: BINARY or ASCI.
Bool_t DDTreeMaker::FindCoding(void){
 
  TString fname_binary = fPath+Form("wave_%i.dat",fChannels[0]);
  TString fname_ascii = fPath+Form("wave_%i.txt",fChannels[0]);
  std::ifstream input_binary(fname_binary, std::ios::binary);
  std::ifstream input_ascii(fname_ascii);
  
  if(input_binary.is_open()){
    fCoding = "binary";
    input_binary.close();
  }
  else if(input_ascii.is_open()){
    fCoding = "ascii";
    input_ascii.close();
  }
  else{
   std::cerr << "\n\n##### Error in DDTreeMaker::FindCoding()! Unknown file coding!\n\n" << std::endl;
   return false;
  }
  
  return true;
}
//------------------------------------------------------------------
Bool_t DDTreeMaker::MakeTree(void){
  
  TString bname;	//branch name
  Int_t entries = 0;	//number of entries in the tree
  
  fBranch.resize(fNch);
  
  if(fOption.Contains("FT")){
    
    fTreeFT = new TTree("tree_ft","tree_ft");
  
    for(Int_t i=0; i<fNch; i++){
      bname = Form("ch_%i",fChannels[i]);
      if(fCalib[i].fCalibMethod.Contains("PE"))
        fBranch[i] = fTreeFT->Branch(bname,(DDSignalPE**)&fSignal[i]);
      else if(fCalib[i].fCalibMethod.Contains("EN"))
	fBranch[i] = fTreeFT->Branch(bname,(DDSignalEnergy**)fSignal[i]);
      AnalyzeChannel(i,"FT");
    }
  
    entries = fBranch[0]->GetEntries();
    fTreeFT->SetEntries(entries);
  
    fFile->cd();
    fTreeFT->Write();
  }
  
// -----  
  if(fOption.Contains("CF")){
    
    fTreeCF = new TTree("tree_cf","tree_cf");
  
    for(Int_t i=0; i<fNch; i++){
      bname = Form("ch_%i",fChannels[i]);
      if(fCalib[i].fCalibMethod.Contains("PE"))
        fBranch[i] = fTreeCF->Branch(bname,(DDSignalPE**)&fSignal[i]);
      else if(fCalib[i].fCalibMethod.Contains("EN"))
	fBranch[i] = fTreeCF->Branch(bname,(DDSignalEnergy**)&fSignal[i]);
      AnalyzeChannel(i,"CF");
    }
  
    entries = fBranch[0]->GetEntries();
    fTreeCF->SetEntries(entries);
  
    fFile->cd();
    fTreeCF->Write();
  }
  
  return true;
}
//------------------------------------------------------------------
Bool_t DDTreeMaker::AnalyzeChannel(Int_t index, TString mode){
  
  Int_t ch = fChannels[index];
  TString fname;
  float x;
  std::ifstream input;
  
  std::cout << "\n----- CHANNEL " << ch << std::endl << std::endl;
    
  //opening input files 
  if(fCoding=="binary"){
    fname = fPath+Form("wave_%i.dat",ch);
    input.open(fname, std::ios::binary);
  }
  else if(fCoding=="ascii"){
    fname = fPath+Form("wave_%i.txt",ch);
    input.open(fname);
  }
    
  //checking if input files are correctly opened
  if(!input.is_open()){
   std::cerr << "##### Error in DDTreeMaker::AnalyzeChannel! Could not open input file!" << std::endl;
   std::cerr << fname << std::endl;
   return false;
  }
    
  Double_t BL;      //base line  
  Float_t amplitude, t0, tot, charge, calibrated;
   
  //reading input files
  while(input.good()){
    
    for(Int_t ii=0; ii<gNS; ii++){      //loop over samples in one signal
      if(fCoding=="binary") input.read((char*)&x, sizeof x);
      else input >> x;
      fSamples[ii] = x/4.096;   //recalculating from ADC channels to mV
      fTime[ii] = ii;           //for 1GHz sampling: 1 sample = 1 ns
    }
    
    BL = 0.;
    for(Int_t i=0; i<50; i++){  //base line determination
     BL+=fSamples[i];
    }
    BL=BL/50.;
    
    for(Int_t i=0; i<gNS; i++){ //base line subtraction
     fSamples[i]=fSamples[i]-BL;
    }
    
    //setting signal properties
    amplitude = FindAmplitude();
    fSignal[index]->SetAmplitude(amplitude);
    
    t0 = FindT0(index,amplitude,mode);
    fSignal[index]->SetT0(t0);
    
    tot = FindTOT(index, amplitude, t0, mode);
    fSignal[index]->SetTOT(tot);
    
    charge = FindCharge(t0, tot); 
    fSignal[index]->SetCharge(charge);
    
    //pe = charge/fCalib[index];
    calibrated = CalibrateCharge(index,charge);
    fSignal[index]->SetCalibrated(calibrated);
    
    fBranch[index]->Fill();
  }

  input.close();

  return true;
}
//------------------------------------------------------------------
/// Finds amplitude of the analyzed signal as:
/// - highest sample in the signal for POSITIVE signals
/// - lowest sample in the signal for NEGATIVE signals.
/// Amplitude is given in mV.
Float_t DDTreeMaker::FindAmplitude(void){
  
  Float_t amplitude = 0.;
  
  if(fPolarity=="NEGATIVE"){
   for(Int_t i=0; i<gNS; i++){
    if(fSamples[i]<amplitude) amplitude = fSamples[i]; 
   }
  }
  else if(fPolarity=="POSITIVE"){
   for(Int_t i=0; i<gNS; i++){
    if(fSamples[i]>amplitude) amplitude = fSamples[i]; 
   }
  }
 
  return amplitude;
}
//------------------------------------------------------------------
Float_t DDTreeMaker::FindT0(Int_t index, Float_t amplitude, TString mode){
  
  Float_t t0 = 0.;
  Float_t threshold = 0.;
  Int_t istop = -1;

  if(index!=-1 && mode=="FT") threshold = fThresholds[index];
  else if(index!=-1 && mode=="CF") threshold = fFractions[index]*amplitude;
  
  if(fPolarity=="NEGATIVE"){
   for(Int_t i=0; i<gNS; i++){
    if(fSamples[i]<threshold){
     istop = i;
     if(i==0) t0=0.;
     break;
    }
     else { 
       t0=-100.;
       istop = i;
     }
    }
  }

  if(fPolarity=="POSITIVE"){
   for(Int_t i=0; i<gNS; i++){
    if(fSamples[i]>threshold){
     istop = i;
     if(i==0) t0=0.;
     break;
    }
    else{ 
      t0=-100.;
      istop = i;
    }
   }
  }
  
  if(istop!=0 && istop!=gNS-1){
    t0 = fTime[istop-1] + 
         (threshold - fSamples[istop-1])*
         ((fTime[istop] - fTime[istop-1])/
         (fSamples[istop] - fSamples[istop-1]));
  }
  
  return t0;
}
//------------------------------------------------------------------
Float_t DDTreeMaker::FindTOT(Int_t index, Float_t amplitude, Float_t t0, TString mode){
 
  if(t0==-100) return -100.;
  
  Int_t istart = 0;
  Float_t tot = 0.;
  Float_t tmax = 0.;
  Float_t threshold = 0.;
 
  if(mode=="FT") threshold = fThresholds[index];
  else if(mode=="CF") threshold = fFractions[index]*amplitude;

  for(Int_t i=0; i<gNS; i++){
    if(fTime[i]>t0){
      istart = i;
      break;
    }
  }
  
  if(fPolarity=="NEGATIVE"){
   for(Int_t ii=istart; ii<gNS; ii++){
    if(fSamples[ii]>threshold){  
      tmax = fTime[ii-1] + 
             (threshold - fSamples[ii-1])*
             ((fTime[ii] - fTime[ii-1])/
             (fSamples[ii] - fSamples[ii-1]));
      break;
    }
   }
  }
  
  if(fPolarity=="POSITIVE"){
   for(Int_t ii=istart; ii<gNS; ii++){
    if(fSamples[ii]<threshold){
      tmax = fTime[ii-1] + 
             (threshold - fSamples[ii-1])*
             ((fTime[ii] - fTime[ii-1])/
             (fSamples[ii] - fSamples[ii-1]));
      break;
    }
   }
  }
   
  if(fabs(tmax)<1E-8){
   tmax = fTime[gNS-1]; 
  }
   
  tot = tmax - t0;
  
  return tot;
}
//------------------------------------------------------------------
Float_t DDTreeMaker::FindCharge(Float_t t0, Float_t tot){
 
  Float_t charge = 0.;
  Float_t tmax = 0.;
  Float_t sum = 0.;
  
  if(t0==-100) return charge;
  
  if(fIntegrationMode=="LIMIT") tmax = t0+fLimit;
  else if(fIntegrationMode=="TOT") tmax = t0+tot;
  
  for(Int_t i=0; i<gNS; i++){
   if(fTime[i]>t0 && fTime[i]<tmax){
     sum += fSamples[i];
   }
  }
  
  charge = sum;
  
  return charge;
}
//------------------------------------------------------------------
Float_t DDTreeMaker::CalibrateCharge(Int_t ch, Float_t charge){

  Float_t calibrated = 0;
  
  if(fCalib[ch].fCalibMethod=="PE"){
    calibrated = charge/fCalib[ch].fCalibPE;
  }
  else if(fCalib[ch].fCalibMethod=="EN"){
    calibrated = (charge*fCalib[ch].fEnSlope) + fCalib[ch].fEnConst;
  }
  else{
    std::cerr << "##### Error in DDTreeMaker::CalibrateCharge()!" << std::endl;
    std::cerr<< "Unknown callibration method!" << std::endl;
    abort();
  }
  
  return calibrated;
}
//------------------------------------------------------------------
/// Prints details of the DDTreeMaker class object.
void DDTreeMaker::Print(void){
 
  std::cout << "\n\n------------------------------------------------" << std::endl;
  std::cout << "This is DDTreeMaker class object for the measurement: " << fPath << std::endl; 
  std::cout << "Number of analyzed channels: " << fNch << std::endl;
  std::cout << "Used file coding: " << fCoding << std::endl;
  std::cout << "Analysis mode: " << fOption << std::endl;
  std::cout << "Polarity of signals: " << fPolarity << std::endl;
  std::cout << "Integration mode (for charge determination): " << fIntegrationMode;
  
  if(fIntegrationMode=="LIMIT") std::cout << ";\t" << "Signal duration = " << fLimit << std::endl;
  else if(fIntegrationMode=="TOT") std::cout << std::endl;
  
  std::cout << "Channel no \t Calib. method \tCalib. factor";
  if(fOption.Contains("FT")) std::cout << "\t Threshold [mV]";
  if(fOption.Contains("CF")) std::cout << "\t Fraction";
  std::cout << std::endl;
  
  for(Int_t i=0; i<fNch; i++){
    std::cout << fChannels[i] << "\t" << fCalib[i].fCalibMethod;
    if(fCalib[i].fCalibMethod=="PE") std::cout << "\t" << fCalib[i].fCalibPE;
    else if(fCalib[i].fCalibMethod=="EN")  std::cout << "\t" << fCalib[i].fEnSlope 
                                              << " " << fCalib[i].fEnConst;
    if(fOption.Contains("FT")) std::cout << "\t" << fThresholds[i];
    if(fOption.Contains("CF")) std::cout << "\t" << fFractions[i];
    std::cout << std::endl;
  }
  std::cout << fFile->GetName() << std::endl;
  std::cout << "------------------------------------------------\n" << std::endl;
  
  return;
}
//------------------------------------------------------------------
