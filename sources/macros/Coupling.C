Bool_t Coupling(void){
 
  const int imax = 5;
  TString path = "/home/lab/Workspace/";
  TString fnames[imax] = {"2018_01_20_13_07","2018_01_24_16_27","2018_01_24_17_25","2018_01_24_18_27","2018_01_25_8_47"};
  
  TFile *file;
  TTree *tree;
  TH1F *hist0[imax];
  TH1F *hist1[imax];
  
  TString fname;
  TString selection;
  TString cut;
  
  TCanvas *can = new TCanvas("can","can",1000,1500);
  can->Divide(2,1);
  
  for(Int_t i=0; i<imax; i++){
    fname = path+fnames[i]+"/results.root";
    file = new TFile(fname,"READ");
    tree = (TTree*)file->Get("tree_ft");
    for(Int_t ii=0; ii<2; ii++){
      TCanvas *tmp = new TCanvas("tmp","tmp",50,50);
      selection = Form("ch_%i.fPE>>h%i(1000,0,1000)",ii,ii);
      cut = Form("ch_%i.fT0<590 && ch_%i.fPE>0",ii,ii);
      tree->Draw(selection,cut);
      if(ii==0){
	hist0[i] = (TH1F*)gPad->GetPrimitive("h0");
        hist0[i]->SetLineColor(i+1);
	can->cd(1);
	hist0[i]->SetTitle(Form("coupling no %i;P.E.; Events/All events",i));
	hist0[i]->Scale(1./hist0[i]->GetEntries());
        hist0[i]->Draw("same");
      }
      else if(ii==1){
	hist1[i] = (TH1F*)gPad->GetPrimitive("h1");
        hist1[i]->SetLineColor(i+1);
	can->cd(2);
	hist1[i]->SetTitle(Form("coupling no %i;P.E.; Events/All events",i));
	hist1[i]->Scale(1./hist1[i]->GetEntries());
        hist1[i]->Draw("same");
      }
      tmp->Close();
    }
  }
  
  can->cd(1);
  gPad->BuildLegend();
  
  return kTRUE;
}
