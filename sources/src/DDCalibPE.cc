// *****************************************
// *                                       *
// *           DesktopDigitizer6           *
// *              DDCalibPE.cc             *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2019              *
// *                                       *
// *****************************************

#include "DDCalibPE.hh"

ClassImp(DDCalibPE);

//------------------------------------------------------------------
/// Adds peak to the fPeaks vector, used for the calibration procedure.
/// \param constant - peak height
/// \param mean - peak position
/// \param sigma - peak sigma.
void DDCalibPE::AddPeak(Float_t constant, Float_t mean, Float_t sigma){
  
  ChargePeak peak;
  peak.fConst = constant;
  peak.fMean = mean;
  peak.fSigma = sigma;
  
  fPeaks.push_back(peak);
  
  if(fPeaks.size()>fNPeaks){
    std::cerr << "##### Error in DDCalibPE::AddPeak()!" << std::endl;
    std::cerr << "Too many peaks!" << std::endl;
    abort();
  }
  
  return;
}
//------------------------------------------------------------------
/// Checks whether the DDCalibPE class object has been created correclty.
bool DDCalibPE::Validate() const {
  return fPeaks.size() == fNPeaks && fFitMin<fFitMax;
}
//------------------------------------------------------------------
/// Performs PE calibration.
/// \param tree - tree containing calibration measurement
/// \param ch - channel number
/// \param file - ROOT output file, where calibration results are saved.
bool DDCalibPE::Calibrate(TTree* tree, Int_t ch, TFile* file){
  
  std::cout << "\n---------- Channel " << ch << " calibration..." << std::endl;
  std::cout << "---------- PE callibration. Fitting sum of Gaussians to the charge spectrum..." << std::endl;
 
  ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls(500000);
  
  //----- Getting charge spectrum
  Double_t unique = gRandom->Uniform(0,1);
  TString hname = Form("PE_calib_ch%i",ch);
  TString selection = Form("ch_%i.fCharge>>htemp%.7f(1000,0,500)",ch,unique);
  TString cut = Form("ch_%i.fCharge>0",ch);
  tree->Draw(selection,cut,"");
  TH1D *hist = (TH1D*)gROOT->FindObjectAny(Form("htemp%.7f",unique));
  hist->SetName(hname);
  hist->SetTitle(hname);
  hist->GetXaxis()->SetTitle("charge [a.u.]");
  hist->GetYaxis()->SetTitle("counts");
  
  //----- Fitting sum of Gaussians to the charge spectrum
  TString formula;
  
  for(Int_t i=0; i<fNPeaks; i++){
    if (i == 0)
     formula.Append(Form("gaus(%i)",3*i)); 
    else 
     formula.Append(Form("+gaus(%i)",3*i));
  }

  TF1 *fun = new TF1("fun",formula,0,100);
  
  for(Int_t i=0; i<fNPeaks; i++){
   fun->SetParameter(i*3+0,fPeaks[i].fConst);
   fun->SetParameter(i*3+1,fPeaks[i].fMean);
   fun->SetParLimits(i*3+1,fPeaks[i].fMean-20,fPeaks[i].fMean+20);
   fun->SetParameter(i*3+2,fPeaks[i].fSigma);
  }
  
  Int_t fit_status = hist->Fit("fun","","",fFitMin,fFitMax);  
 
  if(fit_status!=0){
     std::cout << "Warning in DDCalibPE::Calibrate()!" << std::endl;
     std::cout << "Sum of Gaussians fit failed! Fit status = " << fit_status << std::endl;
  }
  
  //----- Calculating calib factor as mean distance between peaks
  std::vector <Double_t> diff;
  Double_t sum = 0;
  
  for(Int_t i=0; i<fNPeaks-1; i++){
    diff.push_back(fun->GetParameter((i*3)+4)-
                   fun->GetParameter((i*3)+1));
    sum+=diff[i];
  }
  
  Double_t calibFactor = sum/diff.size();
  
  //----- Calculating calib factor from linear fit
  TString gname = Form("PE_calib_ch%i_graph",ch);
  TGraphErrors *graph = new TGraphErrors(fNPeaks);
  graph->SetName(gname);
  graph->SetTitle(gname);
  graph->GetXaxis()->SetTitle("number of PE");
  graph->GetYaxis()->SetTitle("peak position [a.u.]");
  
  for(Int_t i=0; i<fNPeaks; i++){
    graph->SetPoint(i,i+1,fun->GetParameter((i*3)+1)); 
    graph->SetPointError(i,0,fun->GetParError((i*3)+1));
  }
  
  TF1 *fpol1 = new TF1("fpol1","pol1",1,fNPeaks);
  graph->Fit(fpol1,"RQ");
    
  if(fit_status!=0){
     std::cout << "Warning in DDCalibPE::Calibrate()!" << std::endl;
     std::cout << "Linear fit failed! Fit status = " << fit_status << std::endl;
  }
    
  //----- Printing results
  std::cout << "\n\n---------- Fit results:" << std::endl;
  std::cout << "Gauss fit Chi2/NDF = " << fun->GetChisquare()/fun->GetNDF() << std::endl;
  std::cout << "Linear fit: Chi2/NDF = " << fpol1->GetChisquare()/fpol1->GetNDF() << std::endl;
  std::cout << "Fitted callibration factor for channel " << ch << " = " << fpol1->GetParameter(1) << " + /- " << fpol1->GetParError(1) << std::endl;
  std::cout << "Offset = " << fpol1->GetParameter(0) << " +/- " << fpol1->GetParError(0) << std::endl;
  std::cout << "Calculated callibration factor for channel " << ch << " = " << calibFactor << std::endl;
  std::cout << "---------------------------\n" << std::endl;
  
  //----- Saving results
  if(file){
    file->cd();
    hist->Write();
    graph->Write();
  }

  return true;
}
//------------------------------------------------------------------
/// Prints details of the DDCalibPE class object.
void DDCalibPE::Print(void){
  std::cout << "\n\n------------------------------------------------" << std::endl;
  std::cout << "This is DDCalibPE class object" << std::endl;
  std::cout << "Number of peaks in the calibration procedure: " << fNPeaks << std::endl;
  std::cout << "Fitting range: " << fFitMin << " - " << fFitMax << std::endl;
  std::cout << "Charge peaks initial parameters: " << std::endl;
  std::cout << "Const \t Mean \t Sigma" << std::endl;
  for(Int_t i=0; i<fNPeaks; i++){
   std::cout << fPeaks[i].fConst << "\t" << fPeaks[i].fMean << "\t" << fPeaks[i].fSigma << std::endl; 
  }
  std::cout << "------------------------------------------------\n\n" << std::endl;
  return;
}
//------------------------------------------------------------------
