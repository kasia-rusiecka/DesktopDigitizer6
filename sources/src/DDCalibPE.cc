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
void DDCalibPE::AddPeak(Float_t constant, Float_t mean, Float_t sigma)
{
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
Bool_t DDCalibPE::Validate() const
{
  return fPeaks.size() == fNPeaks && fFitMin != fFitMax;
}
//------------------------------------------------------------------
Bool_t DDCalibPE::Calibrate(TTree* tree, Int_t ch, TFile* file)
{
  std::cout << "\n---------- Channel " << ch << " calibration..." << std::endl;
  std::cout << "---------- PE callibration. Fitting sum of Gaussians to the charge spectrum..." << std::endl;
  
  //----- Getting Charge spectrum
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
  
  for(int i=0; i<fNPeaks; i++){
    if (i == 0)
     formula.Append(Form("gaus(%i)",3*i)); 
    else 
     formula.Append(Form("+gaus(%i)",3*i));
  }

  TF1 *fun = new TF1("fun",formula,0,100);
  
  for(Int_t i=0; i<fNPeaks; i++){
   fun->SetParameter(i*3+0,fPeaks[i].fConst);
   fun->SetParameter(i*3+1,fPeaks[i].fMean);
   fun->SetParameter(i*3+2,fPeaks[i].fSigma);
  }
  
  hist->Fit("fun","","",fFitMin,fFitMax);
  
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
  
  for(Int_t i=0; i<fNPeaks; i++){	// starting with 2PE peak
    graph->SetPoint(i,i+2,fun->GetParameter((i*3)+1)); 
    graph->SetPointError(i,0,fun->GetParError((i*3)+1));
  }
  
  TF1 *fpol1 = new TF1("fpol1","pol1",2,fNPeaks+1);
  graph->Fit(fpol1,"R");
  
  //----- Printing results
  std::cout << "\n\n---------- Fit results:" << std::endl;
  std::cout << "Gauss fit Chi2/NDF = " << fun->GetChisquare()/fun->GetNDF() << std::endl;
  std::cout << "Linear fit: Chi2/NDF = " << fpol1->GetChisquare()/fpol1->GetNDF() << std::endl;
  std::cout << "Fitted callibration factor for channel " << ch << " = " << fpol1->GetParameter(1) 
       << " + /- " << fpol1->GetParError(1) << std::endl;
  std::cout << "Calculated callibration factor for channel " << ch << " = " << calibFactor << std::endl;
  std::cout << "---------------------------\n" << std::endl;
  
  //----- Saving results
  if(file){
    file->cd();
    hist->Write();
    graph->Write();
  }

  return kTRUE;
}
//------------------------------------------------------------------