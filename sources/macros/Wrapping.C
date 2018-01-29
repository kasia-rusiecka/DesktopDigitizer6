Bool_t Wrapping(void){
 
  const int imax = 9;
  TString fname[imax] = {"2018_01_25_9_20","2018_01_25_9_52","2018_01_25_10_23",
		         "2018_01_25_10_54","2018_01_25_8_47","2018_01_25_11_26",
			 "2018_01_25_12_00","2018_01_25_12_32","2018_01_25_13_10"};
  TString fname_wrapped[imax] = {"2018_01_26_10_03","2018_01_26_10_36","2018_01_26_11_07",
				 "2018_01_26_11_39","2018_01_26_12_11","2018_01_26_12_42",
				 "2018_01_26_13_14","2018_01_26_13_46","2018_01_26_14_18"};
  Double_t positions[imax] = {10.4,20,30,40,50,60,70,80,90};
  
  TFile *file;
  TFile *file_wrapped;
  TTree *tree;
  TTree *tree_wrapped;
  
  TH1F *hist;
  TH1F *hist_wrapped;
  
  TString selection;
  TString cut;
  
  TString path ="/home/lab/Workspace/";
  
  TCanvas *cch0 = new TCanvas("cch0","cch0",1000,1000);
  cch0->Divide(3,3);
  
  TCanvas *cch1 = new TCanvas("cch1","cch1",1000,1000);
  cch1->Divide(3,3);
  
  TCanvas *ctemp = new TCanvas("ctemp","ctemp",100,100);
  
  
  for(Int_t i=0; i<imax; i++){
    file = new TFile(path+fname[i]+"/results.root","READ");
    file_wrapped = new TFile(path+fname_wrapped[i]+"/results.root","READ");
    tree = (TTree*)file->Get("tree_ft");
    tree_wrapped = (TTree*)file_wrapped->Get("tree_ft");
    
    for(Int_t ii=0; ii<2; ii++){
     ctemp->cd();
     selection = Form("ch_%i.fPE>>h%i(1000,0,1000)",ii,ii); 
     cut = Form("ch_%i.fT0<590 && ch_%i.fPE>0",ii,ii);
     tree->Draw(selection,cut);
     hist = (TH1F*)gPad->GetPrimitive(Form("h%i",ii));
     selection = Form("ch_%i.fPE>>h%iwrapped(1000,0,1000)",ii,ii);
     tree_wrapped->Draw(selection,cut);
     hist_wrapped = (TH1F*)gPad->GetPrimitive(Form("h%iwrapped",ii));
     if(ii==0){
       cch0->cd(i+1);
       hist->DrawClone();
       hist_wrapped->SetLineColor(kRed);
       hist_wrapped->DrawClone("same");
    }
    else if(ii==1){
      cch1->cd(i+1);
      hist->DrawClone();
      hist_wrapped->SetLineColor(kRed);
      hist_wrapped->DrawClone("same");
    }
   }
    
  }
  ctemp->Close();
     
  
  return kTRUE;
}
