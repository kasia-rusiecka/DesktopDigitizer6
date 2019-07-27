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
/// Adds peak to the fPeaks vector, used for the calibration procedure.
/// \param nPE - number of photoelectrons corresponding to the peak
/// \param cutMin - lower range of the peak
/// \param cutMax - upper range of the peak.
void DDCalibAmp::AddPeak(Int_t nPE, Float_t cutMin, Float_t cutMax){
  
  AmpPeak peak;
  peak.fNPE = nPE;
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
/// Checks whether the DDCalibAmp class object has been created
/// correctly.
bool DDCalibAmp::Validate() const {
  return fPeaks.size() == fNPeaks; 
}
//------------------------------------------------------------------
/// Performs PE calibration based on the cuts on the amplitude spectrum.
/// \param tree - tree containing calibration measurement
/// \param ch - channel number
/// \param file - ROOT output file, where calibration results are saved.
Bool_t DDCalibAmp::Calibrate(TTree* tree, Int_t ch, TFile* file){
  
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
  Int_t fit_status = -1;
  Double_t *peaksX;
  TSpectrum *spectrum = new TSpectrum(1);
  std::vector <TH1D*> hCharge;
  std::vector <TF1*> fun;
  hCharge.resize(fNPeaks);
  fun.resize(fNPeaks);
  
  for(Int_t i=0; i<fNPeaks; i++){
    unique = gRandom->Uniform(0,1);
    hChargeName = Form("PE_calib_%iPE_ch%i",fPeaks[i].fNPE,ch);
    selection = Form("ch_%i.fCharge>>htemp%.7f(1000,0,400)",ch,unique);
    cut = Form("ch_%i.fAmp>%.f && ch_%i.fAmp<%.f && ch_%i.fCharge>0", 
          ch, fPeaks[i].fCutMin, ch, fPeaks[i].fCutMax, ch);
    tree->Draw(selection,cut);
    hCharge[i] = (TH1D*)gROOT->FindObjectAny(Form("htemp%.7f",unique));
    hCharge[i]->SetName(hChargeName);
    hCharge[i]->SetTitle(hChargeName);
    hCharge[i]->GetXaxis()->SetTitle("charge [a.u.]");
    hCharge[i]->GetYaxis()->SetTitle("counts");
    n = spectrum->Search(hCharge[i],50,"goff");
    peaksX = spectrum->GetPositionX();
    fun[i] = new TF1("fun","gaus",peaksX[0]-15,peaksX[0]+15);
    fun[i]->SetParameter(0,hCharge[i]->GetBinContent(hCharge[i]->FindBin(peaksX[0])));
    fun[i]->SetParameter(1,peaksX[0]);
    fun[i]->SetParameter(2,50);
    fit_status = hCharge[i]->Fit(fun[i],"RQ");
    if(fit_status!=0){
     std::cout << "Warning in DDCalibAmp::Calibrate()!" << std::endl;
     std::cout << "Fitting " << fPeaks[i].fNPE << " PE peak failed!" << std::endl;
     std::cout << "Fit status = " << fit_status << std::endl;
    }
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
    graph->SetPoint(i,fPeaks[i].fNPE,fun[i]->GetParameter(1));
    graph->SetPointError(i,0,fun[i]->GetParError(1));
  }
  
  TF1 *fpol1 = new TF1("fpol1","pol1",fPeaks[0].fNPE,fPeaks[fNPeaks-1].fNPE);
  fit_status = graph->Fit(fpol1,"RQ");
  
  if(fit_status!=0){
     std::cout << "Warning in DDCalibAmp::Calibrate()!" << std::endl;
     std::cout << "Linear fit failed! Fit status = " << fit_status << std::endl;
    }
  
   //----- Printing results
  std::cout << "\n\n---------- Fit results:" << std::endl;
  std::cout << "Linear fit: Chi2/NDF = " << fpol1->GetChisquare()/fpol1->GetNDF() << std::endl;
  std::cout << "Fitted callibration factor for channel " << ch << " = " << fpol1->GetParameter(1) << " + /- " << fpol1->GetParError(1) << std::endl;
  std::cout << "Offset = " << fpol1->GetParameter(0) << " +/- " << fpol1->GetParError(0) << std::endl; 
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

  return true;
}
//------------------------------------------------------------------
/// Prints details of the DDCalibAmp class object.
void DDCalibAmp::Print(void){
  std::cout << "\n\n------------------------------------------------" << std::endl;
  std::cout << "This is DDCalibAmp class object" << std::endl;
  std::cout << "Number of peaks in the calibration procedure: " << fNPeaks << std::endl;
  std::cout << "Amplitude peaks initial parameters: " << std::endl;
  std::cout << "n PE \t Cut Min \t Cut Max" << std::endl;
  for(Int_t i=0; i<fNPeaks; i++){
   std::cout << fPeaks[i].fNPE << "\t" << fPeaks[i].fCutMin << "\t" << fPeaks[i].fCutMax << std::endl; 
  }
  std::cout << "------------------------------------------------\n\n" << std::endl;
  return;
}
//------------------------------------------------------------------
