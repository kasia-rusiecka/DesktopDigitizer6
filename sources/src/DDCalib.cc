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
  
  fTree = (TTree*)fInputFile->Get("tree_ft");
  
  if(fTree!=NULL){
    cout << "\nIn DDCalib::GetTree(). Accesing fixed threshold data..." << endl;
    return kTRUE;
  }
  else{
    fTree = (TTree*)fInputFile->Get("tree_cf");
    if(fTree==NULL){
     cout << "\n##### Error in DDCalib::GetTree()! Could not access data!"<< endl;
     return kFALSE;
    }
    else{
        cout << "\nIn DDCalib::GetTree(). Accessing constant fraction data..." << endl;
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
    
    if(dummy.Contains("#")){
      getline(config,line);
    }
    else if(dummy.Contains("CH")){
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
        fAmpPeakMin.resize(fNPeaks);
        fAmpPeakMax.resize(fNPeaks);
      }
      else if(fMethod=="PE_SUM"){
        fGaussPar.resize(fNPeaks*3);
        fGaussFitRange.resize(2);
      }
      else if(fMethod=="EN"){
        fPeakID.resize(fNPeaks);
        fPeakPos.resize(fNPeaks);
        fPeakWidth.resize(fNPeaks);
      }
    }
    else if(dummy.Contains("CONST")){
      getline(config,line);
      if(fMethod!="PE_SUM"){
        cout << "##### Error in DDCalib::ReadConfig()! Incorrect syntax!" << endl;
        return kFALSE;
      }
      for(Int_t i=0; i<fNPeaks; i++){
        config >> fGaussPar[i*3] >> fGaussPar[(i*3)+1] >> fGaussPar[(i*3)+2];
      }
    }
    else if(dummy.Contains("FIT_MIN")){
      getline(config,line); 
      if(fMethod!="PE_SUM"){
        cout << "##### Error in DDCalib::ReadConfig()! Incorrect syntax!" << endl;
        return kFALSE;
      }
      config >> fGaussFitRange[0] >> fGaussFitRange[1];
    }
    else if(dummy.Contains("PEAK_MIN")){
      getline(config,line);
      if(fMethod!="PE_CUT"){
        cout << "##### Error in DDCalib::ReadConfig()! Incorrect syntax!" << endl;
        return kFALSE;
      }
      for(Int_t i=0; i<fNPeaks; i++){
        config >> fAmpPeakMin[i] >> fAmpPeakMax[i];
      }
    }
    else if(dummy.Contains("PEAK_ID")){
      getline(config,line);
      if(fMethod!="EN"){
        cout << "##### Error in DDCalib::ReadConfig()! Incorrect syntax!" << endl;
        return kFALSE;
      }
      for(Int_t i=0; i<fNPeaks; i++){
        config >> fPeakID[i] >> fPeakPos[i] >> fPeakWidth[i];
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
  
  cout << "\n---------- Channel " << fCh << " calibration..." << endl;
  cout << "---------- PE callibration. Cutting PE spectrum based on amplitude spectrum..." << endl;
  
  //----- Getting Amplitude spectrum
  Double_t unique = gRandom->Uniform(0,1);
  TString hAmpName = Form("PE_calib_amp_ch%i",fCh);
  TString selection = Form("ch_%i.fAmp>>htemp%.7f(400,0,20)",fCh,unique);
  fTree->Draw(selection,"");
  TH1D *hAmp = (TH1D*)gROOT->FindObjectAny(Form("htemp%.7f",unique));
  hAmp->SetName(hAmpName);
  hAmp->SetTitle(hAmpName);
  hAmp->GetXaxis()->SetTitle("amplitude [mV]");
  hAmp->GetYaxis()->SetTitle("counts");
  
  //----- Getting Charge spectra
  TString hChargeName;
  TString cut;
  Int_t n = 0;
  Double_t *peaksX;
  TSpectrum *spectrum = new TSpectrum(1);
  vector <TH1D*> hCharge;
  vector <TF1*> fun;
  hCharge.resize(fNPeaks);
  fun.resize(fNPeaks);
  
  for(Int_t i=0; i<fNPeaks; i++){
    unique = gRandom->Uniform(0,1);
    hChargeName = Form("PE_calib_%iPE_ch%i",i,fCh);
    selection = Form("ch_%i.fCharge>>htemp%.7f(1000,0,400)",fCh,unique);
    cut = Form("ch_%i.fAmp>%.f && ch_%i.fAmp<%.f && ch_%i.fCharge>50", fCh, fAmpPeakMin[i], fCh, fAmpPeakMax[i], fCh);
    fTree->Draw(selection,cut);
    hCharge[i] = (TH1D*)gROOT->FindObjectAny(Form("htemp%.7f",unique));
    hCharge[i]->SetName(hChargeName);
    hCharge[i]->SetTitle(hChargeName);
    hCharge[i]->GetXaxis()->SetTitle("charge [a.u.]");
    hCharge[i]->GetYaxis()->SetTitle("counts");
    n = spectrum->Search(hCharge[i],50,"goff");
    peaksX = spectrum->GetPositionX();
    fun[i] = new TF1("fun","gaus",peaksX[0]-30,peaksX[0]+50);
    fun[i]->SetParameter(0,hCharge[i]->GetBinContent(hCharge[i]->FindBin(peaksX[0])));
    fun[i]->SetParameter(1,peaksX[0]);
    fun[i]->SetParameter(2,50);
    hCharge[i]->Fit(fun[i],"R");
  }
  
  //----- Calculating calib factor as mean distance between peaks
  vector <Double_t> diff;
  Double_t sum = 0;
  
  for(Int_t i=0; i<fNPeaks-1; i++){
    diff.push_back(fun[i+1]->GetParameter(1)-
                   fun[i]->GetParameter(1));
    sum+=diff[i];
  }
  
  Double_t calibFactor = sum/diff.size();
  
  //----- Calculating calib factor from linear fit
  TGraphErrors *graph = new TGraphErrors(fNPeaks);
  TString gname = Form("PE_calib_amp_ch%i_graph",fCh);
  graph->SetName(gname);
  graph->SetTitle(gname);
  graph->GetXaxis()->SetTitle("number of PE");
  graph->GetYaxis()->SetTitle("peak position [a.u.]");
  
  for(Int_t i=0; i<fNPeaks; i++){
    graph->SetPoint(i,i+2,fun[i]->GetParameter(1));
    graph->SetPointError(i,0,fun[i]->GetParError(1));
  }
  
  TF1 *fpol1 = new TF1("fpol1","pol1",2,fNPeaks+1);
  graph->Fit(fpol1,"R");
  
   //----- Printing results
  cout << "\n\n---------- Fit results:" << endl;
  cout << "Linear fit: Chi2/NDF = " << fpol1->GetChisquare()/fpol1->GetNDF() << endl;
  cout << "Fitted callibration factor for channel " << fCh << " = " << fpol1->GetParameter(1) 
       << " + /- " << fpol1->GetParError(1) << endl;
  cout << "Calculated callibration factor for channel " << fCh << " = " << calibFactor << endl;
  cout << "---------------------------\n" << endl;
  
  //----- Saving
  fOutputFile->cd();
  hAmp->Write();
  for(Int_t i=0; i<fNPeaks; i++){
    hCharge[i]->Write(); 
  }
  graph->Write();
  
  return kTRUE;
}
//------------------------------------------------------------------
Bool_t DDCalib::CalibratePEsum(void){
  
  cout << "\n---------- Channel " << fCh << " calibration..." << endl;
  cout << "---------- PE callibration. Fitting sum of Gaussians to the charge spectrum..." << endl;
  
  //----- Getting Charge spectrum
  Double_t unique = gRandom->Uniform(0,1);
  TString hname = Form("PE_calib_ch%i",fCh);
  TString selection = Form("ch_%i.fCharge>>htemp%.7f(1000,0,500)",fCh,unique);
  TString cut = Form("ch_%i.fCharge>0",fCh);
  fTree->Draw(selection,cut,"");
  TH1D *hist = (TH1D*)gROOT->FindObjectAny(Form("htemp%.7f",unique));
  hist->SetName(hname);
  hist->SetTitle(hname);
  hist->GetXaxis()->SetTitle("charge [a.u.]");
  hist->GetYaxis()->SetTitle("counts");
  
  //----- Fitting sum of Gaussians to the charge spectrum
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
  
  hist->Fit("fun","","",fGaussFitRange[0],fGaussFitRange[1]);
  
  //----- Calculating calib factor as mean distance between peaks
  vector <Double_t> diff;
  Double_t sum = 0;
  
  for(Int_t i=0; i<fNPeaks-1; i++){
    diff.push_back(fun->GetParameter((i*3)+4)-
                   fun->GetParameter((i*3)+1));
    sum+=diff[i];
  }
  
  Double_t calibFactor = sum/diff.size();
  
  //----- Calculating calib factor from linear fit
  TString gname = Form("PE_calib_ch%i_graph",fCh);
  TGraphErrors *graph = new TGraphErrors(fNPeaks);
  graph->SetName(gname);
  graph->SetTitle(gname);
  graph->GetXaxis()->SetTitle("number of PE");
  graph->GetYaxis()->SetTitle("peak position [a.u.]");
  
  for(Int_t i=0; i<fNPeaks; i++){	// starting with 2PE peak
    graph->SetPoint(i,i+2,fun->GetParameter((i*3)+1)); 
    graph->SetPointError(i,0,fun->GetParError((i*3)+1));
  }
  
  TF1 *fpol1 = new TF1("fpol1","pol1",2,fNPeaks+1);
  graph->Fit(fpol1,"R");
  
  //----- Printing results
  cout << "\n\n---------- Fit results:" << endl;
  cout << "Gauss fit Chi2/NDF = " << fun->GetChisquare()/fun->GetNDF() << endl;
  cout << "Linear fit: Chi2/NDF = " << fpol1->GetChisquare()/fpol1->GetNDF() << endl;
  cout << "Fitted callibration factor for channel " << fCh << " = " << fpol1->GetParameter(1) 
       << " + /- " << fpol1->GetParError(1) << endl;
  cout << "Calculated callibration factor for channel " << fCh << " = " << calibFactor << endl;
  cout << "---------------------------\n" << endl;
  
  //----- Saving results
  fOutputFile->cd();
  hist->Write();
  graph->Write();
  
  return kTRUE;
}
//------------------------------------------------------------------
Bool_t DDCalib::CalibrateEnergy(void){
    
  cout << "\n---------- Channel " << fCh << " calibration..." << endl;
  cout << "---------- Energy callibration..." << endl;
  
  //----- Getting Charge spectrum
  Double_t unique = gRandom->Uniform(0,1);
  TString hname = Form("energy_calib_ch%i",fCh);
  TString selection = Form("ch_%i.fCharge>>htemp%.7f(2000,0,1.5E5)",fCh,unique);
  TString cut = Form("ch_%i.fCharge>0",fCh);
  fTree->Draw(selection,cut,"");
  TH1D *hist = (TH1D*)gROOT->FindObjectAny(Form("htemp%.7f",unique));
  hist->SetName(hname);
  hist->SetTitle(hname);
  hist->GetXaxis()->SetTitle("charge [a.u.]");
  hist->GetYaxis()->SetTitle("counts");

  //----- Setting functions and fitting Gaussians to peaks
  TF1 *funPeakBg = new TF1("funPeakBg","gaus(0)+pol2(3)",0,100);
  TF1 *funPeak = new TF1("funPeak","gaus(0)",0,100);
  vector <TF1*> peaks;
  peaks.resize(fNPeaks);
  Int_t npar = 0;
  
  for(Int_t i=0; i<fNPeaks; i++){
     if(fPeakID[i]==511){
       peaks[i] = (TF1*)funPeakBg->Clone("peak511");
       peaks[i]->SetParameter(0,hist->GetBinContent(hist->FindBin(fPeakPos[i])));
       peaks[i]->SetParameter(1,fPeakPos[i]);
       peaks[i]->SetParameter(2,fPeakWidth[i]);
       peaks[i]->SetParameter(3,-2E4);
       peaks[i]->SetParameter(4,1);
       peaks[i]->SetParameter(5,-1E-5);
       hist->Fit(peaks[i],"+","",fPeakPos[i]-3*fPeakWidth[i],
                                 fPeakPos[i]+3*fPeakWidth[i]);
     }
     else{
       peaks[i] = (TF1*)funPeak->Clone(Form("peak%i",fPeakID[i]));
       peaks[i]->SetParameter(0,hist->GetBinContent(hist->FindBin(fPeakPos[i])));
       peaks[i]->SetParameter(1,fPeakPos[i]);
       peaks[i]->SetParameter(2,fPeakWidth[i]);
       hist->Fit(peaks[i],"+","",fPeakPos[i]-3*fPeakWidth[i],
                                 fPeakPos[i]+3*fPeakWidth[i]);
     }
  }
  
  //----- Energy callibration
  TGraphErrors *graph = new TGraphErrors(fNPeaks);
  TString gname = Form("PE_calib_ch%i_graph",fCh);
  graph->SetName(gname);
  graph->SetTitle(gname);
  graph->GetXaxis()->SetTitle("peak position [a.u.]");
  graph->GetYaxis()->SetTitle("energy [keV]");
  
  for(Int_t i=0; i<fNPeaks; i++){
    graph->SetPoint(i,peaks[i]->GetParameter(1),fPeakID[i]);
    graph->SetPointError(i,peaks[i]->GetParError(1),0);
  }

  TF1 *fpol1 = new TF1("fpol1","pol1",peaks[0]->GetParameter(1),
		       peaks[fNPeaks-1]->GetParameter(1));
  fpol1->SetParameter(0,-200);
  fpol1->SetParameter(1,1);
  graph->Fit(fpol1,"R");
  
  //----- Printing results
  cout << "\n\n---------- Fit results:" << endl;
  cout << "Peak fitting results: " << endl;
  for(Int_t i=0; i<fNPeaks; i++){
   cout << fPeakID[i] << " keV:\tChi2/NDF = " << peaks[i]->GetChisquare()/peaks[i]->GetNDF() 
        << "\tpeak pos. = " << peaks[i]->GetParameter(1) << endl;
  }
  cout << "Linear fit Chi2/NDF = " << fpol1->GetChisquare()/fpol1->GetNDF() << endl;
  cout << "Energy callibration formula: E = " << fpol1->GetParameter(1) << "*x + " 
       << fpol1->GetParameter(0) << endl;
  cout << "---------------------------\n" << endl;
  
  //----- Saving
  fOutputFile->cd();
  hist->Write();
  graph->Write();
  
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