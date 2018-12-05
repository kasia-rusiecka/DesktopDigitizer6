// *****************************************
// *                                       *
// *           DesktopDigitizer            *
// *            DDTreeMaker.cc             *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2017              *
// *                                       *
// *****************************************

#include "DDTreeMaker.hh"
using namespace std;

ClassImp(DDTreeMaker);

const int gNS = 1024;	//number of samples in one signal

//------------------------------------------------------------------
/// Default constructor. 
DDTreeMaker::DDTreeMaker(){
  Reset();
  cout << "##### Error in DDTreeMaker Constructor! You are using default constructor." << endl;
  cout << "##### Use standard constructor instead! DDTreeMaker(path)" << endl;
}
//------------------------------------------------------------------
/// Standard constructor (recommended). 
/// \param path - path to the directory containing data for analysis
DDTreeMaker::DDTreeMaker(TString path){
  
  Reset();
  fPath = path;
  TString fname = path+"results.root";
  fFile = new TFile(fname,"RECREATE");
  ReadConfig(path);
  FindCoding(path);
  
  for(Int_t i=0; i<fNch; i++){
    fSignal.push_back(new DDSignal());
  }
}
//------------------------------------------------------------------
/// Default destructor.
DDTreeMaker::~DDTreeMaker(){
  //SaveBaseLines();
  if(fFile->IsOpen()) fFile->Close();
}
//------------------------------------------------------------------
Bool_t DDTreeMaker::ReadConfig(TString path){
 
  TString dummy;
  string line;
  
  TString config_name = path+"config.txt";
  ifstream config(config_name);
  
  if(!config.is_open()){
   cout << "##### Error in DDTreeMaker::ReadConfig! Could not open config file!" << endl;
   return kFALSE;
  }
  
  while(!config.eof()){
   
    config >> dummy;
    
    if(dummy.Contains("NCH")){
     config >> fNch;
     if(fNch>16){
      cout << "##### Error in DDTreeMaker::ReadConfig()!" << endl;
      cout << "Number of channels cannot be larger than 16!" << endl;
      return kFALSE;
     }
     fChannels.reserve(fNch);
     fCalib.reserve(fNch);
     fThresholds.reserve(fNch);
     fFractions.reserve(fNch);
    }
    else if(dummy.Contains("POL")){
     config >> fPolarity;
     if(!(fPolarity=="NEGATIVE" || fPolarity=="POSITIVE")){
      cout << "##### Error in DDTreeMaker::ReadConfig()!" << endl;
      cout << "Unknown signal polarity!" << endl;
      return kFALSE;
     }
    }
    else if(dummy.Contains("OPT")){
     config >> fOption;
     if(!fOption.Contains("FT") && !fOption.Contains("CF")){
      cout << "##### Error in DDTreeMaker::ReadConfig!" << endl;
      cout << "Unknown analysis mode!" << endl;
      return kFALSE;
      }
    }
    else if(dummy.Contains("INT")){
     config >> fIntegrationMode; 
     if(!(fIntegrationMode=="LIMIT" || fIntegrationMode=="TOT")){
       cout << "##### Error in DDTreeMaker::ReadConfig!" << endl;
       cout << "Unknown integration mode!" << endl;
       return kFALSE;
     }
     if(fIntegrationMode=="LIMIT"){
       config >> fLimit;
     }
    }
    else if(dummy.Contains("CH_LIST")){
     getline(config,line);
     for(Int_t i=0; i<fNch; i++){
      config >> fChannels[i] >> fCalib[i] 
             >> fThresholds[i] >> fFractions[i];
      fThresholds[i] = fThresholds[i]/4.096;	//recalculating from ADC channels to mV   
       }
    }
    else{
     cout << "##### Warning in DDTreeMaker::ReadConfig()!" << endl;
     cout << "Unknown syntax, skipping line" << endl;
     getline(config,line);
    }
  }
  
  config.close();

  return kTRUE;
}
//------------------------------------------------------------------
/// Checks data files coding: BINARY or ASCI.
/// \param path - path to the directory containing data files.
Bool_t DDTreeMaker::FindCoding(TString path){
 
  TString fname_binary = path+Form("wave_%i.dat",fChannels[0]);
  TString fname_ascii = path+Form("wave_%i.txt",fChannels[0]);
  ifstream input_binary(fname_binary, ios::binary);
  ifstream input_ascii(fname_ascii);
  
  if(input_binary.is_open()){
    fCoding = "binary";
    input_binary.close();
  }
  else if(input_ascii.is_open()){
    fCoding = "ascii";
    input_ascii.close();
  }
  else{
   cout << "\n\n##### Error in DDTreeMaker::FindCoding()! Unknown file coding!\n\n" << endl;
   return kFALSE;
  }
  
  return kTRUE;
}
//------------------------------------------------------------------
Bool_t DDTreeMaker::MakeTree(void){
  
  TString bname;	//branch name
  Int_t entries = 0;	//number of entries in the tree
  
  fBranch.reserve(fNch);
  
  if(fOption.Contains("FT")){
    
    fTreeFT = new TTree("tree_ft","tree_ft");
  
    for(Int_t i=0; i<fNch; i++){
      bname = Form("ch_%i",fChannels[i]);
      fBranch[i] = fTreeFT->Branch(bname,"DDSignal",&fSignal[i]);
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
      fBranch[i] = fTreeCF->Branch(bname,"DDSignal",&fSignal[i]);
      AnalyzeChannel(i,"CF");
    }
  
    entries = fBranch[0]->GetEntries();
    fTreeCF->SetEntries(entries);
  
    fFile->cd();
    fTreeCF->Write();
  }
  
  return kTRUE;
}
//------------------------------------------------------------------
Bool_t DDTreeMaker::AnalyzeChannel(Int_t index, TString mode){
  
  Int_t ch = fChannels[index];
  TString fname;
  float x;
  ifstream input;
  
  cout << "\n----- CHANNEL " << ch << endl << endl;
    
  //opening input files 
  if(fCoding=="binary"){
    fname = fPath+Form("wave_%i.dat",ch);
    input.open(fname, ios::binary);
  }
  else if(fCoding=="ascii"){
    fname = fPath+Form("wave_%i.txt",ch);
    input.open(fname);
  }
    
  //checking if input files are correctly opened
  if(!input.is_open()){
   cout << "##### Error in DDTreeMaker::AnalyzeChannel! Could not open input file!" << endl;
   cout << fname << endl;
   return kFALSE;
  }
    
  Double_t BL;	//base line  
  Float_t amplitude, t0, tot, charge, pe;
   
  //reading input files
  while(!input.eof()){
    
    for(Int_t ii=0; ii<gNS; ii++){	//loop over samples in one signal
      if(fCoding=="binary") input.read((char*)&x, sizeof x);
      else input >> x;
      fSamples[ii] = x/4.096;	//recalculating from ADC channels to mV
      fTime[ii] = ii;		//for 1GHz sampling: 1 sample = 1 ns
    }
    
    BL = 0.;
    for(Int_t i=0; i<50; i++){	//Base line determination
     BL+=fSamples[i];
    }
    BL=BL/50.;
    
    for(Int_t i=0; i<gNS; i++){	//Base line subtraction
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
    
    pe = charge/fCalib[index];
    fSignal[index]->SetPE(pe);
    
    fBranch[index]->Fill();
  }

  input.close();

  return kTRUE;
}
//------------------------------------------------------------------
/// Finds amplitude of the analyzed signal. Amplitude in mV.
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
Bool_t DDTreeMaker::FindBaseLine(Int_t channel, Bool_t saving, Double_t &line, Double_t &sigma){
  
  const int first = 1;		//first channel for base line determination
  const int last = 50;		//last channel for base line determination
  const int nADC = 4096;	//ADC range
  
  TString fname;
  ifstream input;
  float x = 0.;
  float mean=0;
  int counter =1;
  
  TString hname = Form("base_line_channel_%i",channel);
  TH1F *hBaseLine = new TH1F(hname,hname,nADC,0,nADC);
  TF1 *fun_init = new TF1("fun_init","gaus",0,nADC);
  
  TString shname = Form("BLvsTime_%i",channel);
  
  TH1F* BLvsTime = new TH1F(shname,shname,10000,0,10000);
  
  if(fCoding=="binary"){
    fname = fPath+Form("wave_%i.dat",channel);
    input.open(fname, ios::binary);
  }
  else{
    fname = fPath+Form("wave_%i.txt",channel);
    input.open(fname);
  }
  
  if(!input.is_open()){
    cout << "##### Error in DDTreeMaker::BaseLine()! Could not open input file!" << endl;
    cout << fname << endl;
    return kFALSE;
  }
  
  while(!input.eof()){
    mean=0;
    for(Int_t i=0; i<gNS; i++){
      if(fCoding=="binary") input.read((char*)&x, sizeof x);
      else input >> x;
      if(i>=first && i<= last){
	hBaseLine->Fill(x);//4.096); 
	mean +=x;
	}
      }
    mean=mean/(last-first);
    BLvsTime->SetBinContent(counter,mean);
    counter++;
  }
  
  hBaseLine->Fit("fun_init","QR");		//initial fitting
  Double_t par0 = fun_init->GetParameter(0);
  Double_t par1 = fun_init->GetParameter(1);
  Double_t par2 = fun_init->GetParameter(2);
  Double_t min = par1 - 1.5*par2;
  Double_t max = par1 + 2*par2;
 
  TF1 *fun = new TF1("fun","gaus",min,max);	//proper fitting
  fun->SetParameters(par0,par1,par2);
  hBaseLine->Fit("fun","QR");
  //line =hBaseLine->GetMean() ;	//value of the base line
  line = fun->GetParameter(1);	//value of the base line
  sigma = fun->GetParameter(2); //sigma of the base line
  
  input.close();
  
  if(saving){
   fFile->cd();
   hBaseLine->Write();
   //BLvsTime->Write();
  }
  
  hBaseLine->Delete();
  
  return kTRUE;
}
//------------------------------------------------------------------
Bool_t DDTreeMaker::SaveBaseLines(void){
  
  TGraphErrors *gBaseLines = new TGraphErrors(fNch);
  gBaseLines->SetName("baseLines");
  gBaseLines->SetTitle("baseLines");
  Double_t line = 0.;
  Double_t sigma = 0.;
  
  for(Int_t i=0; i<fNch; i++){
    FindBaseLine(fChannels[i],kTRUE,line,sigma);
    gBaseLines->SetPoint(i,fChannels[i],line);
    gBaseLines->SetPointError(i,0,sigma);
  }
  
  fFile->cd();
  gBaseLines->Write();
  
}
//------------------------------------------------------------------
/// Sets values of all provate class members to their default values.
void DDTreeMaker::Reset(void){
  fPath            = "";
  fCoding          = "";
  fPolarity        = "";
  fOption          = "";
  fIntegrationMode = "";
  fNch             = 0;
  fLimit           = 0;
  fFile            = NULL;
  fTreeFT          = NULL;
  fTreeCF          = NULL;
  if(!fCalib.empty())      fCalib.clear();
  if(!fChannels.empty())   fChannels.clear();
  if(!fThresholds.empty()) fThresholds.clear();
  if(!fFractions.empty())  fFractions.clear();
  return;  
}
//------------------------------------------------------------------
/// Prints details of the DDTreeMaker class object.
void DDTreeMaker::Print(void){
 
  cout << "\n\n------------------------------------------------" << endl;
  cout << "This is DDTreeMaker class object for the measurement: " << fPath << endl; 
  cout << "Number of analyzed channels: " << fNch << endl;
  cout << "Used file coding: " << fCoding << endl;
  cout << "Analysis mode: " << fOption << endl;
  cout << "Polarity of signals: " << fPolarity << endl;
  cout << "Integration mode (for charge determination): " << fIntegrationMode;
  
  if(fIntegrationMode=="LIMIT") cout << ";\t" << "Signal duration = " << fLimit << endl;
  else if(fIntegrationMode=="TOT") cout << endl;
  
  cout << "Channel no \t Calib. factor";
  if(fOption.Contains("FT")) cout << "\t Threshold [mV]";
  if(fOption.Contains("CF")) cout << "\t Fraction";
  cout << endl;
  
  for(Int_t i=0; i<fNch; i++){
    cout << fChannels[i] << "\t" << fCalib[i];
    if(fOption.Contains("FT")) cout << "\t" << fThresholds[i];
    if(fOption.Contains("CF")) cout << "\t" << fFractions[i];
    cout << endl;
  }
  cout << fFile->GetName() << endl;
  cout << "------------------------------------------------\n" << endl;
  
  return;
}
//------------------------------------------------------------------