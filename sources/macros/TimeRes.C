Bool_t TimeRes(void){
  
  const int imax = 9;
  TString path = "/home/lab/Workspace/";
  Double_t position[imax] = {10.3, 20.0, 30.0, 40.0,50.0, 60.0, 70.0, 80.0, 90.0};
  TString fname[imax] = {"2018_01_20_13_56", "2018_01_20_14_43", "2018_01_20_15_30",
			   "2018_01_22_9_29", "2018_01_20_13_07", "2018_01_22_10_16",
			   "2018_01_22_11_02", "2018_01_22_11_51", "2018_01_22_12_40"}; 
  
 TFile *file;
 TTree *tree_ft;
 TTree *tree_cf;
 TH1F *hist_ft[imax];
 TH1F *hist_cf[imax];
 TH1F *hist_ratio_ft[imax];
 TH1F *hist_ratio_cf[imax];
 TH1F* Middle_ft;
 TH1F* Middle_cf; 
 TString hname_ft; 
 TString hname_cf;

 TCanvas *can = new TCanvas("can","can",1200,1200);
 can->Divide(3,3);
 
 TCanvas *c = new TCanvas("c","c",800,800);
 c->Divide(1,2);
 
 TCanvas *cratio = new TCanvas("cratio","cratio",800,800);
 cratio->Divide(3,3);

 TString cut;
 TString selection;
 
 Double_t mean_ft=0;
 Double_t sigma_ft=0;
 Double_t mean_cf=0;
 Double_t sigma_cf=0;
 
 
 for(Int_t i=0; i<imax; i++){
  file = new TFile(path+fname[i]+"/results.root","READ");
  tree_ft = (TTree*)file->Get("tree_ft");
  tree_cf = (TTree*)file->Get("tree_cf");
  
  cratio->cd(i+1);
  tree_ft->Draw("log(ch_0.fPE/ch_1.fPE)>>hratioft(500,-2.5,2.5)","ch_0.fT0<590 && ch_0.fPE>0 && ch_1.fT0<590 && ch_1.fPE>0");
  hist_ratio_ft[i]= (TH1F*)gPad->GetPrimitive("hratioft");
  tree_cf->Draw("log(ch_0.fPE/ch_1.fPE)>>hratiocf(500,-2.5,2.5)","ch_0.fT0<590 && ch_0.fPE>0 && ch_1.fT0<590 && ch_1.fPE>0","same");
  hist_ratio_cf[i]= (TH1F*)gPad->GetPrimitive("hratiocf");
    
  
  can->cd(i+1);
  mean_ft = hist_ratio_ft[i]->GetMean();
  sigma_ft = hist_ratio_ft[i]->GetRMS();
  
  mean_cf = hist_ratio_cf[i]->GetMean();
  sigma_cf= hist_ratio_cf[i]->GetRMS();
  
  selection = "ch_0.fT0-ch_1.fT0>>hft(1000,-30,30)";
  cut = Form("ch_0.fT0>0 && ch_1.fT0>0 && log(ch_0.fPE/ch_1.fPE)>%f && log(ch_0.fPE/ch_1.fPE)<%f",mean_ft-sigma_ft,mean_ft+sigma_ft);
  tree_ft->Draw(selection, cut);
  hist_ft[i] = (TH1F*)gPad->GetPrimitive("hft");
  selection = "ch_0.fT0-ch_1.fT0>>hcf(1000,-30,30)";
  cut = Form("ch_0.fT0>0 && ch_1.fT0>0 && log(ch_0.fPE/ch_1.fPE)>%f && log(ch_0.fPE/ch_1.fPE)<%f",mean_cf-sigma_cf,mean_cf+sigma_cf);
  tree_cf->Draw(selection,cut,"same");
  hist_cf[i] = (TH1F*)gPad->GetPrimitive("hcf");
  
  
  if(i==4){ 
    hist_ft[i]->Scale(3./4.);
    tree_ft->Draw("ch_0.fT0-ch_1.fT0>>htemp(1000,-30,30)","ch_0.fT0>0 && ch_1.fT0>0 && ch_0.fPE>215 && ch_0.fPE<240 && ch_1.fPE>200 && ch_1.fPE<225");
    Middle_ft= (TH1F*)gPad->GetPrimitive("htemp");
    tree_cf->Draw("ch_0.fT0-ch_1.fT0>>htempcf(1000,-30,30)","ch_0.fT0>0 && ch_1.fT0>0 && ch_0.fPE>215 && ch_0.fPE<240 && ch_1.fPE>200 && ch_1.fPE<225");
     
    Middle_cf= (TH1F*)gPad->GetPrimitive("htempcf");
    
  }
  
  hname_ft = Form("%.1f_mm_ft",position[i]);
  hist_ft[i]->SetLineColor(i+1);
  hist_ft[i]->SetTitle(hname_ft);
  hist_ft[i]->SetName(hname_ft);
  hist_ft[i]->Draw();
  
  hname_cf = Form("%.1f_mm_cf",position[i]);
  hist_cf[i]->SetLineColor(i+11);
  hist_cf[i]->SetTitle(hname_cf);
  hist_cf[i]->SetName(hname_cf);
  hist_cf[i]->Draw("same");
  
  c->cd(1);  
  hist_ft[i]->Draw("SAME");
  c->cd(2);  
  hist_cf[i]->Draw("SAME");
   
}

   
//  c->cd();
 
//  for(Int_t i=0; i<imax; i++){
//   if(i==0) hist_ft[i]->DrawClone();
//   else hist[i]->Draw("SAME");
//  }
 //c->BuildLegend();
 
 TCanvas *c1 = new TCanvas("c1","c1",800,800);
 c1->Divide(1,2);
 c1->cd(1);
// Middle_ft->Draw();
 c1->cd(2);
// Middle_cf->Draw();
 
 
  return kTRUE;
}
