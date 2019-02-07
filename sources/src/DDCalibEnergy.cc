// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *           DDCalibEnergy.cc            *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2019              *
// *                                       *
// *****************************************

#include "DDCalibEnergy.hh"

ClassImp(DDCalibEnergy);
 
//------------------------------------------------------------------
/// Adds peak to the fPeaks vector, used for the calibration procedure.
/// \param id - peak ID, i.e. its energy given in keV
/// \param mean - peak position on the charge spectrum
/// \param sigma - peak width
void DDCalibEnergy::AddPeak(Float_t id, Float_t mean, Float_t sigma){
  
  EnergyPeak peak;
  peak.fPeakID = id;
  peak.fMean = mean;
  peak.fSigma = sigma;
  
  fPeaks.push_back(peak);
  
  if(fPeaks.size()>fNPeaks){
   std::cerr << "##### Error in DDCalibEnergy::AddPeak()!" << std::endl;
   std::cerr << "Too many peaks!" << std::endl;
   std::abort();
  }
  
  return;
}
//------------------------------------------------------------------
/// Checks whether the DDCalibEnergy calss object has been created 
/// correctly.
bool DDCalibEnergy::Validate() const{
  return fPeaks.size() == fNPeaks;
}
//------------------------------------------------------------------
/// Performs energy calibration.
/// \param tree - tree containing calibration measurement
/// \param ch - channel number
/// \param file - ROOT output file, where calibration results are saved.
bool DDCalibEnergy::Calibrate(TTree* tree, Int_t ch, TFile* file){
  
  std::cout << "\n---------- Channel " << ch << " calibration..." << std::endl;
  std::cout << "---------- Energy callibration..." << std::endl;
  
  //----- Getting charge spectrum
  Double_t unique = gRandom->Uniform(0,1);
  TString hname = Form("energy_calib_ch%i",ch);
  TString selection = Form("ch_%i.fCharge>>htemp%.7f(2000,0,1.5E5)",ch,unique);
  TString cut = Form("ch_%i.fCharge>0",ch);
  tree->Draw(selection,cut,"");
  TH1D *hist = (TH1D*)gROOT->FindObjectAny(Form("htemp%.7f",unique));
  hist->SetName(hname);
  hist->SetTitle(hname);
  hist->GetXaxis()->SetTitle("charge [a.u.]");
  hist->GetYaxis()->SetTitle("counts");

  //----- Setting functions and fitting Gaussians to peaks
  TF1 *funPeakBg = new TF1("funPeakBg","gaus(0)+pol2(3)",0,100);
  TF1 *funPeak = new TF1("funPeak","gaus(0)",0,100);
  std::vector <TF1*> peaks;
  peaks.resize(fNPeaks);
  Int_t npar = 0;
  Int_t fit_status = -1;
  
  for(Int_t i=0; i<fNPeaks; i++){
     if(fPeaks[i].fPeakID==511){
       peaks[i] = (TF1*)funPeakBg->Clone("peak511");
       peaks[i]->SetParameter(0,hist->GetBinContent(hist->FindBin(fPeaks[i].fMean)));
       peaks[i]->SetParameter(1,fPeaks[i].fMean);
       peaks[i]->SetParameter(2,fPeaks[i].fSigma);
       peaks[i]->SetParameter(3,-2E4);
       peaks[i]->SetParameter(4,1);
       peaks[i]->SetParameter(5,-1E-5);
       fit_status = hist->Fit(peaks[i],"+Q","",fPeaks[i].fMean-3*fPeaks[i].fSigma,
                                               fPeaks[i].fMean+3*fPeaks[i].fSigma);
     }
     else{
       peaks[i] = (TF1*)funPeak->Clone(Form("peak%i",fPeaks[i].fPeakID));
       peaks[i]->SetParameter(0,hist->GetBinContent(hist->FindBin(fPeaks[i].fMean)));
       peaks[i]->SetParameter(1,fPeaks[i].fMean);
       peaks[i]->SetParameter(2,fPeaks[i].fSigma);
       fit_status = hist->Fit(peaks[i],"+Q","",fPeaks[i].fMean-3*fPeaks[i].fSigma,
                                               fPeaks[i].fMean+3*fPeaks[i].fSigma);
     }
     if(fit_status!=0){
     std::cout << "Warning in DDCalibEnrgy::Calibrate()!" << std::endl;
     std::cout << "Gaussian fit for peak " << fPeaks[i].fPeakID << " failed!" <<std::endl;
     std::cout << "Fit status = " << fit_status << std::endl;
     }
  }
  
  //----- Energy calibration
  TGraphErrors *graph = new TGraphErrors(fNPeaks);
  TString gname = Form("PE_calib_ch%i_graph",ch);
  graph->SetName(gname);
  graph->SetTitle(gname);
  graph->GetXaxis()->SetTitle("peak position [a.u.]");
  graph->GetYaxis()->SetTitle("energy [keV]");
  
  for(Int_t i=0; i<fNPeaks; i++){
    graph->SetPoint(i,peaks[i]->GetParameter(1),fPeaks[i].fPeakID);
    graph->SetPointError(i,peaks[i]->GetParError(1),0);
  }

  TF1 *fpol1 = new TF1("fpol1","pol1",peaks[0]->GetParameter(1),
		       peaks[fNPeaks-1]->GetParameter(1));
  fpol1->SetParameter(0,-200);
  fpol1->SetParameter(1,1);
  fit_status = graph->Fit(fpol1,"RQ");
  
  if(fit_status!=0){
     std::cout << "Warning in DDCalibEnergy::Calibrate()!" << std::endl;
     std::cout << "Linear fit failed! Fit status = " << fit_status << std::endl;
  }
  
  //----- Printing results
  std::cout << "\n\n---------- Fit results:" << std::endl;
  std::cout << "Peak fitting results: " << std::endl;
  for(Int_t i=0; i<fNPeaks; i++){
   std::cout << fPeaks[i].fPeakID << " keV:\tChi2/NDF = " << peaks[i]->GetChisquare()/peaks[i]->GetNDF() 
        << "\tpeak pos. = " << peaks[i]->GetParameter(1) << std::endl;
  }
  std::cout << "Linear fit Chi2/NDF = " << fpol1->GetChisquare()/fpol1->GetNDF() << std::endl;
  std::cout << "Energy callibration formula: E = " << fpol1->GetParameter(1) << "*x + " 
       << fpol1->GetParameter(0) << std::endl;
  std::cout << "---------------------------\n" << std::endl;
  
  //----- Saving
  if(file){
    file->cd();
    hist->Write();
    graph->Write();
  }
  
  return true;
}
//------------------------------------------------------------------
/// Prints details of the DDCalibEnergy class object.
void DDCalibEnergy::Print(void){
  std::cout << "\n\n------------------------------------------------" << std::endl;
  std::cout << "This is DDCalibEnergy class object" << std::endl;
  std::cout << "Number of peaks in the calibration procedure: " << fNPeaks << std::endl;
  std::cout << "Charge peaks initial parameters: " << std::endl;
  std::cout << "ID \t Mean \t Sigma" << std::endl;
  for(Int_t i=0; i<fNPeaks; i++){
   std::cout << fPeaks[i].fPeakID << "\t" << fPeaks[i].fMean << "\t" << fPeaks[i].fSigma << std::endl; 
  }
  std::cout << "------------------------------------------------\n\n" << std::endl;
  return;
}
//------------------------------------------------------------------