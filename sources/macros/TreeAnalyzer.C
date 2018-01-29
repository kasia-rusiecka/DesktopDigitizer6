#include "DDSignal.hh"

TH1F* hADC[2];
TH1F* hTDC[2];
TH1F* hQDC[2];
TH1F* hAmean;
TH1F* hQmean;
TH1F* hT1mT2;
TH2F* hA1vsA2;
TH2F* hT1vsT2;
TH2F* hQ1vsQ2;


void SetupSpectra(void){
  Int_t col[2] = {2,4};
  TString name, titles;
  for(int i=0; i<2; i++){
    name.Form("hADC_ch%i",i);
    titles.Form("%s; Voltage / mV; counts",name.Data());
    hADC[i] = new TH1F(name,name,1000, 0, 1000);
    hADC[i]->SetTitle(titles);
    hADC[i]->SetLineColor(col[i]);
  }
  for(int i=0; i<2; i++){
    name.Form("hQDC_ch%i",i);
    titles.Form("%s; QDC / mV; counts",name.Data());
    hQDC[i] = new TH1F(name,name,4096, 0, 409600);	//!!
    hQDC[i]->SetTitle(titles);
    hQDC[i]->SetLineColor(col[i]);
  }
  for(int i=0; i<2; i++){
    name.Form("hTDC_ch%i",i);
    titles.Form("%s; TDC / ns; counts",name.Data());
    hTDC[i] = new TH1F(name,name,1024, 0, 1024);
    hTDC[i]->SetTitle(titles);
    hTDC[i]->SetLineColor(col[i]);
  }
  //----------------------
  hT1mT2 = new TH1F("hT0mT1","hT0mT1",2048,-1024,1024);
  hT1mT2->SetTitle("T0 - T1; T0-T1 / ns; counts");
  //----------------------
  hA1vsA2 = new TH2F("hA0vsA1","hA0vsA1",1000,0,1000,1000,0,1000);
  hA1vsA2->SetTitle("A0 vs A1; A1 / mV; A0 / mV");
  //----------------------
  hT1vsT2 = new TH2F("hT0vsT1","hT0vsT1",1024,0,1024,1024,0,1024);
  hT1vsT2->SetTitle("T0 vs T1; T1 / ns; T0 / ns");
  //----------------------
  hQ1vsQ2 = new TH2F("hQ0vsQ1","hQ0vsQ1",4096,0,409600,4096,0,409600);
  hQ1vsQ2->SetTitle("Q0 vs Q1; Q1 / mV; Q0 / mV");
  //----------------------
  hQmean = new TH1F("hQmean","hQmean",4096,0,409600);
  hQmean->SetTitle("sqrt(Q0*Q1); sqrt(Q0*Q1) / mV; counts");
  //----------------------
  hAmean = new TH1F("hAmean","hAmean",4096,0,4096);
  hAmean->SetTitle("sqrt(A0*A1); sqrt(A0*A1) / mV; counts");
}

TCanvas* PresentResults(void){
   gStyle->SetOptStat(111111);
   TCanvas* can = new TCanvas("can","can",1000,1000);
   can->Divide(3,3);
   can->cd(1);
   gPad->SetGrid(1,1);
   hADC[0]->Draw();
   hADC[1]->Draw("same");
   gPad->BuildLegend();
   can->cd(2);
   gPad->SetGrid(1,1);
   hTDC[0]->Draw();
   hTDC[1]->Draw("same");
   gPad->BuildLegend();
   can->cd(3);
   gPad->SetGrid(1,1);
   hQDC[0]->Draw();
   hQDC[1]->Draw("same");
   gPad->BuildLegend();
   can->cd(4);
   gPad->SetGrid(1,1);
   hT1vsT2->Draw("colz");
   gPad->SetLogz(1);
   can->cd(5);
   gPad->SetGrid(1,1);
   hA1vsA2->Draw("colz");
   gPad->SetLogz(1);
   can->cd(6);
   gPad->SetGrid(1,1);
   hQ1vsQ2->Draw("colz");
   gPad->SetLogz(1);
   can->cd(7);
   gPad->SetGrid(1,1);
   hT1mT2->Draw("");
   can->cd(8);
   gPad->SetGrid(1,1);
   hAmean->Draw("");
   can->cd(9);
   gPad->SetGrid(1,1);
   hQmean->Draw("");

   return can;
}

Bool_t TreeAnalyzer(TString name = "results.root"){

//Reset ROOT and connect tree file
   gROOT->Reset();

   SetupSpectra();
   TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(name);
   if (!f) {
      f = new TFile(name);
   }
   TTree* treeptr = (TTree*)f->Get("tree_ft");

//Declaration of leaves types
   DDSignal* fch0 = NULL;
   DDSignal* fch1 = NULL;


   // Set branch addresses.
   treeptr->SetBranchAddress("ch_0",&fch0);
   treeptr->SetBranchAddress("ch_1",&fch1);

   Long64_t nentries = treeptr->GetEntries();

   for(Long64_t i=0; i<nentries; i++) {
     treeptr->GetEntry(i);
     //treeptr->Show(i);
     //fch0->Print(); 
     hADC[0]->Fill(fch0->GetAmplitude());
     hADC[1]->Fill(fch1->GetAmplitude());
     hTDC[0]->Fill(fch0->GetT0());
     hTDC[1]->Fill(fch1->GetT0());
     hQDC[0]->Fill(fch0->GetCharge());
     hQDC[1]->Fill(fch1->GetCharge());
     hT1mT2->Fill(fch0->GetT0()-fch1->GetT0());
     hAmean->Fill(sqrt(fch0->GetAmplitude()*fch1->GetAmplitude()));
     hQmean->Fill(sqrt(fch0->GetCharge()*fch1->GetCharge()));
     hT1vsT2->Fill(fch1->GetT0(),fch0->GetT0());
     hA1vsA2->Fill(fch1->GetAmplitude(),fch0->GetAmplitude());
     hQ1vsQ2->Fill(fch1->GetCharge(),fch0->GetCharge());
   }

   PresentResults();
   return kTRUE;
}
