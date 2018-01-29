Bool_t Calibration(Int_t ch=0){                                                                                                                                                                                                                                                                                                                                                
  
  Int_t rebin=4;
 
  TString fname;
  if(ch==0) fname = "/home/lab/Workspace/2018_01_19_17_17/results.root";
  else if(ch==1) fname = "/home/lab/Workspace/2018_01_22_15_37/results.root";
  TFile *file = new TFile(fname,"READ");
  
  TTree *tree = (TTree*)file->Get("tree");
  
  TCanvas *can = new TCanvas("can","can",1000,800);
  can->SetLogy();
  
  TH1D* a= new TH1D("a","a",2800/rebin,0,1400);
  a->SetLineColor(2);
  a->Sumw2();
  TH1D* b= new TH1D("b","b",2800/rebin,0,1400);
  b->SetLineColor(3);
  b->Sumw2();
  TH1D* c= new TH1D("c","c",2800/rebin,0,1400);
  c->SetLineColor(4);
  c->Sumw2();
  TH1D* d= new TH1D("d","d",2800/rebin,0,1400);
  d->SetLineColor(5);
  d->Sumw2();
  TH1D* e= new TH1D("e","e",2800/rebin,0,1400);
  e->SetLineColor(6);
  e->Sumw2();
  TH1D* f= new TH1D("f","f",2800/rebin,0,1400);
  f->SetLineColor(1);
  f->Sumw2();
    
  TF1* gaus_1;
  TF1* gaus_2;
  TF1* gaus_3;
  TF1* gaus_sum;
  TFitResultPtr fit;
  TFitResultPtr fit1;
  TFitResultPtr fit2;
  TFitResultPtr fit3;
  
  if(ch==0){
   tree->Draw("ch_0.fCharge>>a","ch_0.fAmp>3 && ch_0.fAmp<6.1 && ch_0.fT0<590");
   tree->Draw("ch_0.fCharge>>b","ch_0.fAmp>7.3 && ch_0.fAmp<10.1 && ch_0.fT0<590","same");
   tree->Draw("ch_0.fCharge>>c","ch_0.fAmp>11.5 && ch_0.fAmp<13.6 && ch_0.fT0<590","same");
   tree->Draw("ch_0.fCharge>>d","ch_0.fAmp>15.9 && ch_0.fAmp<17.8 && ch_0.fT0<590","same");
   tree->Draw("ch_0.fCharge>>e","ch_0.fAmp>19.6 && ch_0.fAmp<22.2 && ch_0.fT0<590","same");
   tree->Draw("ch_0.fCharge>>f","ch_0.fT0<590","same");
   
   gaus_1= new TF1("gaus_1","gaus",75,272);
   gaus_2= new TF1("gaus_2","gaus",272,470);
   gaus_3= new TF1("gaus_3","gaus",470,653);
   gaus_sum= new TF1("gaus_sum","gaus(0)+gaus(3)+gaus(6)",0,653);
  
   gaus_1->SetParameters(1e4,200,50);
   gaus_2->SetParameters(1e3,400,50);
   gaus_3->SetParameters(1e1,600,50);
  
   fit1 = a->Fit("gaus_1","RS");
   fit2 = b->Fit("gaus_2","RS");
   fit3 = c->Fit("gaus_3","RS"); 
   
  gaus_sum->SetParameters(gaus_1->GetParameter(0),gaus_1->GetParameter(1),gaus_1->GetParameter(2),
			  gaus_2->GetParameter(0),gaus_2->GetParameter(1),gaus_2->GetParameter(2),
	                  gaus_3->GetParameter(0),gaus_3->GetParameter(1),gaus_3->GetParameter(2));


   gaus_sum->SetParLimits(1,gaus_1->GetParameter(1)-2*gaus_1->GetParError(1),gaus_1->GetParameter(1)+2*gaus_1->GetParError(1));
   gaus_sum->SetParLimits(4,gaus_2->GetParameter(1)-2*gaus_2->GetParError(1),gaus_2->GetParameter(1)+2*gaus_2->GetParError(1));
   gaus_sum->SetParLimits(7,gaus_3->GetParameter(1)-2*gaus_3->GetParError(1),gaus_3->GetParameter(1)+2*gaus_3->GetParError(1));

   gaus_sum->SetParLimits(2,gaus_1->GetParameter(2)-2*gaus_1->GetParError(2),gaus_1->GetParameter(2)+2*gaus_1->GetParError(2));
   gaus_sum->SetParLimits(5,gaus_2->GetParameter(2)-2*gaus_2->GetParError(2),gaus_2->GetParameter(2)+2*gaus_2->GetParError(2));
   gaus_sum->SetParLimits(8,gaus_3->GetParameter(2)-2*gaus_3->GetParError(2),gaus_3->GetParameter(2)+2*gaus_3->GetParError(2));

  
   fit = f->Fit("gaus_sum","RS");
  }
  else if(ch==1){
   tree->Draw("ch_1.fCharge>>a","ch_1.fAmp>3.5 && ch_1.fAmp<6.0 && ch_1.fT0<590");
   tree->Draw("ch_1.fCharge>>b","ch_1.fAmp>7.2 && ch_1.fAmp<9.3 && ch_1.fT0<590","same");
   tree->Draw("ch_1.fCharge>>c","ch_1.fAmp>10.8 && ch_1.fAmp<13.1 && ch_1.fT0<590","same");
   tree->Draw("ch_1.fCharge>>d","ch_1.fAmp>15.9 && ch_1.fAmp<17.9 && ch_1.fT0<590","same");
   tree->Draw("ch_1.fCharge>>f","ch_1.fT0<590","same");
    
   gaus_1= new TF1("gaus_1","gaus",119,280);
   gaus_2= new TF1("gaus_2","gaus",280,458);
   gaus_3= new TF1("gaus_3","gaus",458,650);
   gaus_sum= new TF1("gaus_sum","gaus(0)+gaus(3)+gaus(6)",0,650);
  
   gaus_1->SetParameters(1e4,200,50);
   gaus_2->SetParameters(1e3,400,50);
   gaus_3->SetParameters(1e1,600,50);
  
   fit1 = a->Fit("gaus_1","RS");
   fit2 = b->Fit("gaus_2","RS");
   fit3 = c->Fit("gaus_3","RS");
  
   gaus_sum->SetParameters(gaus_1->GetParameter(0),gaus_1->GetParameter(1),gaus_1->GetParameter(2),
			  gaus_2->GetParameter(0),gaus_2->GetParameter(1),gaus_1->GetParameter(2),
	                  gaus_3->GetParameter(0),gaus_3->GetParameter(1)+50,gaus_1->GetParameter(2));
  
   gaus_sum->SetParLimits(1,0,gaus_2->GetParameter(1)-20);
   gaus_sum->SetParLimits(4,gaus_1->GetParameter(1)+20,gaus_3->GetParameter(1)-20);
   gaus_sum->SetParLimits(7,gaus_2->GetParameter(1)+100,700);

   fit = f->Fit("gaus_sum","RS");
  }
  
  cout << "Fit 1: chi2 = " << fit1->Chi2() << "\t NDF = " << fit1->Ndf() << endl;
  cout << "Fit 2: chi2 = " << fit2->Chi2() << "\t NDF = " << fit2->Ndf() << endl;  
  cout << "Fit 3: chi2 = " << fit3->Chi2() << "\t NDF = " << fit3->Ndf() << endl;    
  
  Double_t diff1 = gaus_2->GetParameter(1) - gaus_1->GetParameter(1);
  Double_t diff2 = gaus_3->GetParameter(1) - gaus_2->GetParameter(1);
  Double_t mean = (diff1+diff2)/2.;
  
  cout << diff1 << "\t" << diff2 << "\t" << mean << endl;
  cout << fit->Chi2() << "\t" << fit->Ndf() << endl;
  
  TCanvas* can_lin = new TCanvas("can_lin","can_lin",800,1000);
  
  TGraphErrors * lin = new TGraphErrors(3);
  
  lin->SetPoint(0,1,gaus_1->GetParameter(1));
  lin->SetPoint(1,2,gaus_2->GetParameter(1));
  lin->SetPoint(2,3,gaus_3->GetParameter(1));
  
  lin->SetPointError(0,0,gaus_1->GetParError(1));
  lin->SetPointError(1,0,gaus_2->GetParError(1));
  lin->SetPointError(2,0,gaus_3->GetParError(1));
  
  lin->Fit("pol1");
  
  lin->Draw("AP");
  
  return kTRUE;
}
