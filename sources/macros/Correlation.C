Bool_t Correlation(int fibre){
   
  const int imax = 9;
  TString path = "/home/lab/Workspace/";
  Double_t position[imax];
  TString fname[imax];
  Double_t position_1[imax] = {10.3, 20.0, 30.0, 40.0,50.0, 60.0, 70.0, 80.0, 90.0};
  TString fname_1[imax] = {"2018_01_20_13_56","2018_01_20_14_43", "2018_01_20_15_30",
			 "2018_01_22_9_29","2018_01_20_13_07", "2018_01_22_10_16",
			 "2018_01_22_11_02", "2018_01_22_11_51", "2018_01_22_12_40"};
  Double_t position_2[imax] = {10.2, 20.0, 30.0, 40.0,50.0, 60.0, 70.0, 80.0, 90.0};
  TString fname_2[imax] = {"2018_01_22_16_59","2018_01_22_17_54", "2018_01_22_18_42",
			 "2018_01_23_9_54","2018_01_23_10_46", "2018_01_23_11_38",
			 "2018_01_23_12_27", "2018_01_23_14_56", "2018_01_23_15_46"};
  Double_t position_3[imax] = {10.4, 20.0, 30.0, 40.0,50.0, 60.0, 70.0, 80.0, 90.0};
  TString fname_3[imax] = {"2018_01_25_9_20","2018_01_25_9_52", "2018_01_25_10_23",
			 "2018_01_25_10_54","2018_01_25_8_47", "2018_01_25_11_26",
			 "2018_01_25_12_00", "2018_01_25_12_32", "2018_01_25_13_10"};
  
    
    
  

   for(Int_t i =0;i<imax;i++){
      if(fibre==1){
	position[i]=position_1[i];
	fname[i]=fname_1[i];
      }
      else if(fibre ==2){
	position[i]=position_2[i];
	fname[i]=fname_2[i];
      }      
      else if(fibre ==3){
	position[i]=position_3[i];
	fname[i]=fname_3[i];
      }
   }
   
  TFile *file;
  TTree *tree;
  TH1D* hratio[imax];
  TH2D* hcor[imax];
  
  Double_t mean=0;
  Double_t sigma=0;
    
  TString selection;
  TString cut;
  TString treename = "tree_ft";
  
  TCanvas* can1 = new TCanvas("Correlations","Correlations",1000,1000);
  can1->Divide(3,3);
    
  
   for(Int_t i=0; i<imax; i++){
    file = new TFile(path+fname[i]+"/results.root","READ");
    
    tree = (TTree*)file->Get(treename);
    
    TCanvas* temp2 = new TCanvas("temp2","temp2",100,100);
    selection= "log(ch_0.fPE/ch_1.fPE)>>hratio(500,-2.5,2.5)";
    cut=Form("ch_%i.fT0<590 && ch_%i.fPE>0 && ch_%i.fT0<590 && ch_%i.fPE>0",0,0,1,1);
    tree->Draw(selection,cut);
    hratio[i] = (TH1D*)gPad->GetPrimitive("hratio");
    if(i==4 && fibre==1) hratio[i]->Scale(3./4.);
    if(i==1 && fibre==2) hratio[i]->Scale(45./52.);
    if(i==4 && fibre==2) hratio[i]->Scale(45./50.);
    if(i==8 && fibre==2) hratio[i]->Scale(45./48.);
      
    temp2->Close();
    
    mean = hratio[i]->GetMean();
    sigma = hratio[i]->GetRMS();
    
    can1->cd(i+1);
    selection= Form("ch_0.fPE:ch_1.fPE>>h%i(1000,0,1000,1000,0,1000)",i);
//     cut=Form("ch_0.fT0<590 && ch_0.fPE>0  && ch_1.fT0<590 && ch_1.fPE>0 && log(ch_0.fPE/ch_1.fPE)<%f && log(ch_0.fPE/ch_1.fPE)>%f" mean+sigma, mean-sigma);
    cut="ch_0.fT0<590 && ch_0.fPE>0  && ch_1.fT0<590 && ch_1.fPE>0 ";
    tree->Draw(selection,cut,"COLZ");
    hcor[i] = (TH2D*)gPad->GetPrimitive(Form("h%i",i));
   }
return kTRUE; 
}
    
    