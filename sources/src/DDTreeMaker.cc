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
  
  Int_t config_flag = ReadConfig();
  Int_t coding_flag = FindCoding();
  
  if(!config_flag || !coding_flag)
    throw "##### Exception in DDTreeMaker constructor!";
  
  for(Int_t i=0; i<fNch; i++){
    fSignal.push_back(new DDSignal());
  }
}
//------------------------------------------------------------------
/// Default destructor.
DDTreeMaker::~DDTreeMaker(){
  if(fFile->IsOpen()) fFile->Close();
}
//------------------------------------------------------------------
/// Reads content of the config file config.txt and sets values of 
/// private members of the class necessary for the analysis: 
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
    
    if(dummy.Contains("NCH")){     ///- number of channels
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
    else if(dummy.Contains("POL")){     ///- signal polarity
     config >> fPolarity;
     if(!(fPolarity=="NEGATIVE" || fPolarity=="POSITIVE")){
      std::cerr << "##### Error in DDTreeMaker::ReadConfig()!" << std::endl;
      std::cerr << "Unknown signal polarity! Possible options: NEGATIVE or POSITIVE" << std::endl;
      return false;
     }
    }
    else if(dummy.Contains("OPT")){     ///- analysis mode
     config >> fOption;
     if(!fOption.Contains("FT") && !fOption.Contains("CF")){
      std::cerr << "##### Error in DDTreeMaker::ReadConfig!" << std::endl;
      std::cerr << "Unknown analysis mode! Possible options: FT or CF" << std::endl;
      return false;
      }
    }
    else if(dummy.Contains("INT")){     ///- integration mode
     config >> fIntegrationMode; 
     if(!(fIntegrationMode=="LIMIT" || fIntegrationMode=="TOT")){
       std::cerr << "##### Error in DDTreeMaker::ReadConfig!" << std::endl;
       std::cerr << "Unknown integration mode! Possible options: LIMIT or TOT" << std::endl;
       return false;
     }
     if(fIntegrationMode=="LIMIT"){     ///- integration limit
       config >> fLimit;
     }
    }
    else if(dummy.Contains("CH_LIST")){     ///- list of: channels, thresholds, fractions, calib methods
     getline(config,line);

     for(Int_t i=0; i<fNch; i++){
      fCalib.push_back(Calibration());
      config >> fChannels[i] >> fThresholds[i] >> fFractions[i] >> fCalib[i].fCalibMethod;
      fThresholds[i] = fThresholds[i]/4.096;   // recalculating from ADC channels to mV
      if(fCalib[i].fCalibMethod=="PE"){     ///- calib factor for PE calibration
	config >> fCalib[i].fCalibPE;
      }
      else if(fCalib[i].fCalibMethod=="EN"){     ///- calib factors for energy calibration
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
     std::cout << dummy << line << std::endl;
    }
  }
  
  config.close();

  return true;
}
//------------------------------------------------------------------
/// Checks data files coding: BINARY or ASCI.
bool DDTreeMaker::FindCoding(void){
 
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
/// Creates requested trees, according to information from the configuration file:
/// "tree_ft" for Fixed Threshold analysis and "tree_cf" for Constant Fraction 
/// analysis. Sets addresses of respective branches. Each branch represents one 
/// analyzed channel. Subsequently data from chosen channels are analyzed and trees
/// filled. Finally trees are saved in the ROOT file. 
bool DDTreeMaker::MakeTree(void){
  
  TString bname;
  Int_t entries = 0;     //number of entries in the tree
  
  fBranch.resize(fNch);
  
  if(fOption.Contains("FT")){     // tree containing Fixed Threshold data
    
    fTreeFT = new TTree("tree_ft","tree_ft");
  
    for(Int_t i=0; i<fNch; i++){
      bname = Form("ch_%i",fChannels[i]);
      fBranch[i] = fTreeFT->Branch(bname,"DDSignal",&fSignal[i]);
      //fBranch[i] = fTreeFT->Branch(bname,(DDSignal**)&fSignal[i]);
      AnalyzeChannel(i,"FT");     // tree filling
    }
  
    entries = fBranch[0]->GetEntries();
    fTreeFT->SetEntries(entries);
    fFile->cd();
    fTreeFT->Write();
  }
  
// -----  
  if(fOption.Contains("CF")){     // tree containing Constant Fraction data
    
    fTreeCF = new TTree("tree_cf","tree_cf");
  
    for(Int_t i=0; i<fNch; i++){
      bname = Form("ch_%i",fChannels[i]);
      fBranch[i] = fTreeCF->Branch(bname,"DDSignal",&fSignal[i]);
      //fBranch[i] = fTreeCF->Branch(bname,(DDSignal**)&fSignal[i]);
      AnalyzeChannel(i,"CF");     // tree filling
    }
  
    entries = fBranch[0]->GetEntries();
    fTreeCF->SetEntries(entries);
    fFile->cd();
    fTreeCF->Write();
  }
  
  return true;
}
//------------------------------------------------------------------
/// Opens data files for the requested channel and analyzes saved data
/// signal after signal. As the data file is being read a 1024-element
/// array fSamples is filled with the saved ADC read-out (1024 samples saved  
/// in one signal). ADC read out is converted to mV. fTimes array is filled 
/// with corresponding time value - in that case 1 GHz samplinq was used, 
/// i.e. 1 sample corresponds to 1 ns. Base line is calculated for every 
/// signal separately, as an average of the first 50 signals in the sample, 
/// subsequently it is subtracted from the signal. fSamples array is analyzed
/// in order to find signal's amplitude, time T0, time over threshold, integral
/// and calibrated integral. Corresponding tree branch is then filled.
/// \param index - index in the fChannels vector where requested channel number
/// is stored
/// \param mode - analysis mode, i.e. FT or CF
bool DDTreeMaker::AnalyzeChannel(Int_t index, TString mode){
  
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
    
  Double_t BL;     //base line  
  Float_t amplitude, t0, tot, charge, calibrated;
   
  //reading input files
  while(input.good()){
    
    for(Int_t ii=0; ii<gNS; ii++){     //loop over samples in one signal
      if(fCoding=="binary") input.read((char*)&x, sizeof x);
      else input >> x;
      fSamples[ii] = x/4.096;   //recalculating from ADC channels to mV
      fTime[ii] = ii;           //for 1GHz sampling: 1 sample = 1 ns
    }
    
    BL = 0.;
    for(Int_t i=0; i<50; i++){     //base line determination
     BL+=fSamples[i];
    }
    BL=BL/50.;
    
    for(Int_t i=0; i<gNS; i++){     //base line subtraction
     fSamples[i]=fSamples[i]-BL;
    }
    
    //setting signal properties
    amplitude = FindAmplitude();
    fSignal[index]->SetAmplitude(amplitude);
    
    t0 = FindT0(index, amplitude, mode);
    fSignal[index]->SetT0(t0);
    
    tot = FindTOT(index, amplitude, t0, mode);
    fSignal[index]->SetTOT(tot);
    
    charge = FindCharge(t0, tot); 
    fSignal[index]->SetCharge(charge);
    
    calibrated = CalibrateCharge(index, charge);
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
/// Finds time T0 of the analyzed signal: 
/// - if analysis mode FT is chosen, T0 is found as time when signal's 
/// amplitude crosses given threshold value
/// - if analysis mode CF is chosen, T0 is found as time when signal's
/// amplitude crosses value FRACTION*MAX_AMPLITUDE
///
/// In order to achieve sub-nanosecond precision time T0 is determined
/// with the linear interpolation, i.e. linear interpolation between 
/// two points sourrounding threshold-crossing point and precise T0 
/// value is calculated.
/// 
/// If threshold is crossed already in the first sample returns value 0.
/// If threshold is not crossed at all returns value -100.
/// \param index - index in fThresholds or fFractions vectors where 
/// data for analyzed channel is stored
/// \param amplitude - amplitude of the analyzed signal, needed if CF 
/// mode is used
/// \param mode - analysis mode, i.e. FT/CF
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
/// Finds Time Over Threshold i.e. time during which signal'a amplitude
/// was higher than given threshold value. Analysis mode FT/CF also
/// applies here. Second point in time when signal's amplitude crosses 
/// the threshold is determined in the same way as T0 in FindT0(), also
/// with the linear interpolation. 
///
/// If T0=-100 i.e. this function also returns -100.
/// If amplidue never crosses the threshold for the second time, this 
/// moment is assumed to be last recorded sample in the signal, i.e. 1024 ns.
/// \param index - index in fThresholds or fFractions vectors where 
/// data for analyzed channel is stored
/// \param amplitude - amplitude of the analyzed signal, needed if CF 
/// mode is used
/// \param t0 - time T0 of the analyzed signal
/// \param mode - analysis mode, i.e. FT/CF
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
/// Returns integral of the analyzed signal. Signal integration starts 
/// at time T0 and depending on the chosen integration mode stops:
/// - after fixed amount of time - LIMIT mode 
/// - after Time Over Threshold - TOT mode
/// 
/// Signal integration is performed as simple addition of subsequent 
/// amplitude samples within the integration range.
/// \param t0 - time T0 of the analyzed signal
/// \param tot - Time Over Threshold
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
/// Returns calibrated charge value, depending on the chosen calibration 
/// method - in PE or in keV.
/// \param index - index in the fCalib vector where calibration information 
/// for the requested channel is stored
/// \param charge - signal integral.
Float_t DDTreeMaker::CalibrateCharge(Int_t index, Float_t charge){

  Float_t calibrated = 0;
  
  if(fCalib[index].fCalibMethod=="PE"){
    calibrated = charge/fCalib[index].fCalibPE;
  }
  else if(fCalib[index].fCalibMethod=="EN"){
    calibrated = (charge*fCalib[index].fEnSlope) + fCalib[index].fEnConst;
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
