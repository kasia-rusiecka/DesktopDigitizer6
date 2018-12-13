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
   
  ReadConfig();
  
  fInputFile = new TFile(path+"results.root","READ");
  fOutputFile = new TFile(path+"calib_"+fMethod+".root","RECREATE");
  
  if(!(fInputFile->IsOpen() || fOutputFile->IsOpen())){
    throw "##### Exception in DDCalib constructor!"; 
  }
  
  Bool_t tree_flag = GetTree();
  cout << "Tree access flag: " << tree_flag << endl; 
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
  
  if(fTree!=NULL){
    cout << "\nIn DDCalib::GetTree(). Accessing Constant Fraction data..." << endl;
    return kTRUE;
  }
  else{
    fTree = (TTree*)fInputFile->Get("tree_ft");
    if(fTree==NULL){
     cout << "\n##### Error in DDCalib::GetTree()! Could not access data!"<< endl;
     return kFALSE;
    }
    else{
        cout << "\nIn DDCalib::GetTree(). Accessing Fixed Threshold data..." << endl;
        return kTRUE;
    }
  }
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
  
  cout << "\n---------- Reading config file " << config_name << endl;
  
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
      if(fMethod=="PE_CUT"){
        fPeaksMin.reserve(fNPeaks);
        fPeaksMax.reserve(fNPeaks);
      }
      else if(fMethod=="PE_SUM"){
        fGaussPar.reserve(fNPeaks*3);
	fGausFitRange.reserve(2);
      }
    }
    else if(dummy.Contains("PEAK_MIN")){
      getline(config,line);
      for(Int_t i=0; i<fNPeaks; i++){
        config >> fPeaksMin[i] >> fPeaksMax[i];
      }
    }
    else if(dummy.Contains("CONST")){
      getline(config,line);
      for(Int_t i=0; i<fNPeaks; i++){
          config >> fGaussPar[i*3] >> fGaussPar[(i*3)+1] >> fGaussPar[(i*3)+2];
      }
    }
    else if(dummy.Contains("FIT_MIN")){
      getline(config,line);
      config >> fGausFitRange[0] >> fGausFitRange[1];
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
  
  cout << "\n---------- Channel " << fCh << " calibration..." << endl;
  cout << "---------- PE callibration. Fitting sum of Gaussians to the charge spectrum..." << endl;
  
  Double_t unique = gRandom->Uniform(0,1);
  TString hname = Form("ch%i_PE_calib",fCh);
  
  TString selection = Form("ch_%i.fCharge>>htemp%.7f(1000,0,500)",fCh,unique);
  TString cut = Form("ch_%i.fCharge>0",fCh);
  fTree->Draw(selection,cut,"");
  TH1D *hist = (TH1D*)gROOT->FindObjectAny(Form("htemp%.7f",unique));
  
  TString formula;
  for(int i=0; i<fNPeaks; i++){
    if(i!=fNPeaks-1)
     formula.Append(Form("gaus(%i)+",3*i)); 
    else 
     formula.Append(Form("gaus(%i)",3*i));
  }

  TF1 *fun = new TF1("fun",formula,0,100);
  Double_t npar = fun->GetNpar();
  
  for(Int_t i=0; i<npar; i++){
   fun->SetParameter(i,fGaussPar[i]);   
  }
  
  hist->Fit("fun","","",fGausFitRange[0],fGausFitRange[1]);
  
  TString gname = Form("ch_%i_PE_calib_graph",fCh);
  TGraphErrors *graph = new TGraphErrors(fNPeaks);
  graph->SetName(gname);
  graph->SetTitle(gname);
  
  for(Int_t i=0; i<fNPeaks; i++){
    graph->SetPoint(i,i+1,fun->GetParameter((i*3)+1)); 
    graph->SetPointError(i,0,fun->GetParError((i*3)+1));
  }
  
  
  TF1 *fpol1 = new TF1("fpol1","pol1",1,fNPeaks);
  graph->Fit(fpol1);
  
  cout << "\n\n---------- Fit results:" << endl;
  cout << "Gauss fit Chi2/NDF = " << fun->GetChisquare()/fun->GetNDF() << endl;
  cout << "Linear fit: Chi2/NDF = " << fpol1->GetChisquare()/fpol1->GetNDF() << endl;
  cout << "---------------------------\n" << endl;
  
  //----- Saving 
  fOutputFile->cd();
  hist->Write();
  graph->Write();
  
  return kTRUE;
}
//------------------------------------------------------------------
Bool_t DDCalib::CalibrateEnergy(void){
    
  cout << "\n---------- Channel " << fCh << " calibration..." << endl;
  cout << "---------- Energy callibration..." << endl;
  
  TString hname = Form("ch%i_Energy_calib",fCh);
  TH1D *hist = new TH1D(hname,hname,2000,0,1.5E5);
  
  TString selection = Form("ch_%i.fCharge>>hist",fCh);
  TString cut = Form("ch_%i.fCharge>0",fCh);
  fTree->Draw(selection,cut,"");
  hist->Write();
  
  return kTRUE;
}
//------------------------------------------------------------------
/// Prints details of the DDCalib class object.
void DDCalib::Print(void){
  cout << "\n\n------------------------------------------------" << endl;
  cout << "This is DDCalib class object for the measurement: " << fPath << endl;
  cout << "Analyzed channel: " << fCh << endl;
  cout << "Calibration method: " << fMethod << endl;
  cout << "Input file: " << fInputFile->GetName() << endl;
  cout << "Results saved in: " << fOutputFile->GetName() << endl;
  cout << "------------------------------------------------\n" << endl;
  return;
}
//------------------------------------------------------------------