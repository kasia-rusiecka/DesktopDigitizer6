// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *             DDCalibAmp.cc             *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2019              *
// *                                       *
// *****************************************

#include "DDCalibAmp.hh"

ClassImp(DDCalibAmp);

//------------------------------------------------------------------
void DDCalibAmp::AddPeak(Float_t cutMin, Float_t cutMax)
{
  AmpPeak peak;
  peak.fCutMin = cutMin;
  peak.fCutMax = cutMax;
  
  fPeaks.push_back(peak);
  
  if(fPeaks.size()>fNPeaks){
   std::cerr << "##### Error in DDCalibAmp::AddPeak()!" << std::endl;
   std::cerr << "Too many peaks!" << std::endl;
   abort();
  }
  
  return;
}
//------------------------------------------------------------------
Bool_t DDCalibAmp::Validate() const
{
  return fPeaks.size() == fNPeaks; 
}
//------------------------------------------------------------------
Bool_t DDCalibAmp::Calibrate(TTree* tree, Int_t ch, TFile* file)
{
  std::cout << "\n---------- Channel " << ch << " calibration..." << std::endl;
  std::cout << "---------- PE callibration. Cutting PE spectrum based on amplitude spectrum..." << std::endl;
  
  //----- Getting Amplitude spectrum
  Double_t unique = gRandom->Uniform(0,1);
  TString hAmpName = Form("PE_calib_amp_ch%i",ch);
  TString selection = Form("ch_%i.fAmp>>htemp%.7f(400,0,20)",ch,unique);
  tree->Draw(selection,"");
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
  std::vector <TH1D*> hCharge;
  std::vector <TF1*> fun;
  hCharge.resize(fNPeaks);
  fun.resize(fNPeaks);
  
  for(Int_t i=0; i<fNPeaks; i++){
    unique = gRandom->Uniform(0,1);
    hChargeName = Form("PE_calib_%iPE_ch%i",i,ch);
    selection = Form("ch_%i.fCharge>>htemp%.7f(1000,0,400)",ch,unique);
    cut = Form("ch_%i.fAmp>%.f && ch_%i.fAmp<%.f && ch_%i.fCharge>50", 
	       ch, fPeaks[i].fCutMin, ch, fPeaks[i].fCutMax, ch);
    tree->Draw(selection,cut);
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
  std::vector <Double_t> diff;
  Double_t sum = 0;
  
  for(Int_t i=0; i<fNPeaks-1; i++){
    diff.push_back(fun[i+1]->GetParameter(1)-
                   fun[i]->GetParameter(1));
    sum+=diff[i];
  }
  
  Double_t calibFactor = sum/diff.size();
  
  //----- Calculating calib factor from linear fit
  TGraphErrors *graph = new TGraphErrors(fNPeaks);
  TString gname = Form("PE_calib_amp_ch%i_graph",ch);
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
  std::cout << "\n\n---------- Fit results:" << std::endl;
  std::cout << "Linear fit: Chi2/NDF = " << fpol1->GetChisquare()/fpol1->GetNDF() << std::endl;
  std::cout << "Fitted callibration factor for channel " << ch << " = " << fpol1->GetParameter(1) 
       << " + /- " << fpol1->GetParError(1) << std::endl;
  std::cout << "Calculated callibration factor for channel " << ch << " = " << calibFactor << std::endl;
  std::cout << "---------------------------\n" << std::endl;
  
  //----- Saving
  if (file) {
    file->cd();
    hAmp->Write();
    for(Int_t i=0; i<fNPeaks; i++){
      hCharge[i]->Write();
    }
    graph->Write();
  }

  return kTRUE;
}
//------------------------------------------------------------------