Double_t xmin=0;
Double_t xmax=0;

Double_t fun(Double_t *x, Double_t *par){
 
 if(x[0]>xmin && x[0]<xmax) TF1::RejectPoint();
 return par[0] + par[1]*x[0] + par[2]*x[0]*x[0] + par[3]*x[0]*x[0]*x[0];
}

Double_t * SubtractBackground(TH1F *spec, TH1F* &bgs, Int_t counter, Int_t channel, Int_t fibre){
  Double_t xminL;
  Double_t xmaxL;
  Double_t xminR;
  Double_t xmaxR;
  
  if(fibre==1){
    if(channel==0){
      xminL = 175-counter*10;
      xmaxL = 215-counter*10;
      xminR = 325-counter*10;
      xmaxR = 450-counter*10;
    
      xmin=xmaxL;
      xmax=xminR;
    }
  
    if(channel==1){
      xminL = 100+counter*10;
      xmaxL = 150+counter*10;
      xminR = 220+counter*10;
      xmaxR = 400+counter*10;
    
      xmin=xmaxL;
      xmax=xminR;
    }
  }
  else if(fibre==2){
    if(channel==0){
      xminL = 175-counter*10;
      xmaxL = 215-counter*10;
      xminR = 325-counter*10;
      xmaxR = 450-counter*10;
    
      xmin=xmaxL;
      xmax=xminR;
    }
    
    if(channel==1){
      xminL = 140+counter*10;
      xmaxL = 170+counter*10;
      xminR = 260+counter*10;
      xmaxR = 400+counter*10;
      
      xmin=xmaxL;
      xmax=xminR;
    }
  }
  else if(fibre==3){
    if(channel==0){
      xminL = 175-counter*10;
      xmaxL = 215-counter*10;
      xminR = 325-counter*10;
      xmaxR = 450-counter*10;
    
      xmin=xmaxL;
      xmax=xminR;
    }
    
    if(channel==1){
      xminL = 140+counter*10;
      xmaxL = 170+counter*10;
      xminR = 260+counter*10;
      xmaxR = 400+counter*10;
      
      xmin=xmaxL;
      xmax=xminR;
    }
  }
  else if(fibre==4){
    if(channel==0){
      xminL = 175-counter*10;
      xmaxL = 215-counter*10;
      xminR = 325-counter*10;
      xmaxR = 450-counter*10;
    
      xmin=xmaxL;
      xmax=xminR;
    }
    
    if(channel==1){
      xminL = 140+counter*10;
      xmaxL = 170+counter*10;
      xminR = 260+counter*10;
      xmaxR = 400+counter*10;
      
      xmin=xmaxL;
      xmax=xminR;
    }
  }
  else if(fibre==5){
    if(channel==0){
      xminL = 175-counter*10;
      xmaxL = 215-counter*10;
      xminR = 325-counter*10;
      xmaxR = 450-counter*10;
    
      xmin=xmaxL;
      xmax=xminR;
    }
    
    if(channel==1){
      xminL = 140+counter*10;
      xmaxL = 170+counter*10;
      xminR = 260+counter*10;
      xmaxR = 400+counter*10;
      
      xmin=xmaxL;
      xmax=xminR;
    }
  }
  
  TF1 *bgfun = new TF1("bgfun",fun,xminL,xmaxR,4);
  
  TH1F *bg = (TH1F*)spec->Clone("bg");
  //bg->Reset();
  
  
  bgs =new TH1F("bgs","bgs",spec->GetNbinsX(),0,spec->GetNbinsX());
  //bgs->Reset();
  TCanvas* temp=new TCanvas("temp","temp",50,50);
  spec->Fit("bgfun","R");

  
  Int_t nbins = spec->GetXaxis()->GetNbins();
  Double_t x = 0.;
  Double_t y = 0.;
  
  for(Int_t i=1; i<nbins+1; i++){
    x = spec->GetBinCenter(i);
    if(x>xminL && x<xmaxR){
     y = spec->GetBinContent(i)-bgfun->Eval(x);
     if(y<0) y=0;
     bgs->SetBinContent(i,y);
     bg->SetBinContent(i,bgfun->Eval(x));
    }
    else{
      bgs->SetBinContent(i,0);
      bg->SetBinContent(i,0);
    }
  }

  bgs->Sumw2();
  bg->Sumw2();

  
  TF1* fun_gaus = new TF1("fungaus","gaus",xmaxL,xminR);

  bgs->Fit("fungaus","R");
  temp->Close();
 
  Double_t* result= new Double_t[4];
  result[1]=fun_gaus->GetParError(1);
  result[0]=fun_gaus->GetParameter(1);
  result[3]=fun_gaus->GetParError(2);
  result[2]=fun_gaus->GetParameter(2);
  
  return result;
}

Bool_t Attenuation(TString Threshold,int fibre){
   
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
  Double_t position_4[imax] = {10.5, 20.0, 30.0, 40.0,50.0, 60.0, 70.0, 80.0, 90.0};
  TString fname_4[imax] = {"2018_01_25_14_05","2018_01_25_14_36", "2018_01_25_15_08",
			 "2018_01_25_15_39","2018_01_25_16_11", "2018_01_25_16_47",
			 "2018_01_25_17_19", "2018_01_25_17_50", "2018_01_26_8_27"};
  Double_t position_5[imax] = {10.4, 20.0, 30.0, 40.0,50.0, 60.0, 70.0, 80.0, 90.0};
  TString fname_5[imax] = {"2018_01_26_10_03","2018_01_26_10_36", "2018_01_26_11_07",
			 "2018_01_26_11_39","2018_01_26_12_11", "2018_01_26_12_42",
			 "2018_01_26_13_14", "2018_01_26_13_46", "2018_01_26_14_18"};
  
    
    
  

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
      else if(fibre ==4){
	position[i]=position_4[i];
	fname[i]=fname_4[i];
      }
      else if(fibre ==5){
	position[i]=position_5[i];
 	fname[i]=fname_5[i];
      }
   }
   
  TGraphErrors *gch0 = new TGraphErrors(imax);
  TGraphErrors *gch1 = new TGraphErrors(imax);
  
  TGraphErrors *gpos = new TGraphErrors(imax);
  gpos->SetMarkerStyle(8);
  Double_t PE0, PE1, ratio;
  
  TFile *file;
  TTree *tree;
  TH1F *h;
  TH1F *hratio[imax];
  TH1F *hlnscor[imax];
  TH2D* posvsratio=new TH2D("posvsratio",";Position at fibre;log(Q_0/Q_1)",9,5,95,500,-2.5,2.5);
  
  TH1F* Peaks;
  Double_t* fxPeaks;
  Double_t mean=0;
  Double_t sigma=0;
  TCanvas* canWaitPrimitive = new TCanvas("canWaitPrimitive","canWaitPrimitive",1000,800);
    
  TString selection;
  TString cut;
  TString treename = "tree_"+Threshold;
  
   for(Int_t i=0; i<imax; i++){
    file = new TFile(path+fname[i]+"/results.root","READ");
    
    tree = (TTree*)file->Get(treename);
    
    TCanvas* temp2 = new TCanvas("temp2","temp2",100,100);
    selection= Form("log(ch_%i.fPE/ch_%i.fPE)>>hratio(500,-2.5,2.5)",0,1);
    cut=Form("ch_%i.fT0<590 && ch_%i.fPE>0 && ch_%i.fT0<590 && ch_%i.fPE>0",0,0,1,1);
    tree->Draw(selection,cut);
    hratio[i] = (TH1F*)gPad->GetPrimitive("hratio");
    if(i==4 && fibre==1) hratio[i]->Scale(3./4.);
    if(i==1 && fibre==2) hratio[i]->Scale(45./52.);
    if(i==4 && fibre==2) hratio[i]->Scale(45./50.);
    if(i==8 && fibre==2) hratio[i]->Scale(45./48.);
      
    temp2->Close();
    
    mean = hratio[i]->GetMean();
    sigma = hratio[i]->GetRMS();
    
    for(Int_t ii=0;ii<2;ii++){
      selection= Form("ch_%i.fPE>>h%i(1000,0,1000)",ii,i);
      cut=Form("ch_%i.fT0<590 && ch_%i.fPE>0 && log(ch_0.fPE/ch_1.fPE)<%f && log(ch_0.fPE/ch_1.fPE)>%f",ii,ii, mean+sigma, mean-sigma);
      TCanvas* temp3 = new TCanvas("temp3","temp3",100,100);
      tree->Draw(selection,cut);
      h = (TH1F*)gPad->GetPrimitive(Form("h%i",i));
      temp3->Close();
      if(i==4 && fibre==1) h->Scale(3./4.);
      if(i==1 && fibre==2) h->Scale(45./52.);
      if(i==4 && fibre==2) h->Scale(45./50.);
      if(i==8 && fibre==2) h->Scale(45./48.);
      
     
      fxPeaks=SubtractBackground(h,Peaks,i,ii,fibre);      
      
      
      if(ii==0){
	gch0->SetPoint(i,position[i],fxPeaks[0]);
	PE0 = fxPeaks[0];
	gch0->SetPointError(i,0,fxPeaks[1]);
	cout << "Pos:" << position[i] << " Peak: " << fxPeaks[0] << endl;
      }
      
      else if(ii==1){
	gch1->SetPoint(i,position[i],fxPeaks[0]);
	PE1 = fxPeaks[0];
	gch1->SetPointError(i,0,fxPeaks[1]);
	cout << "Pos:" << position[i] << " Peak: " << fxPeaks[0] << endl;
      }
      canWaitPrimitive->cd();
      h->Draw();
      Peaks->Draw("same");
      canWaitPrimitive ->WaitPrimitive();
      


    }
    
    selection= Form("log(sqrt(ch_0.fPE/ch_1.fPE))>>hlnscor%i(100,-1,1)",i);
//     cut=Form("ch_0.fT0<590 && ch_0.fPE>0 && ch_1.fT0<590 && ch_1.fPE>0 &&log(ch_0.fPE/ch_1.fPE)<%f && log(ch_0.fPE/ch_1.fPE)>%f",mean+sigma, mean-sigma);
    cut="ch_0.fT0<590 && ch_0.fPE>0 && ch_1.fT0<590";
    
    TCanvas* temp4 = new TCanvas("temp4","temp4",100,100);
    tree->Draw(selection,cut);
    hlnscor[i] = (TH1F*)gPad->GetPrimitive(Form("hlnscor%i",i));
    temp4->Close();
    
    //ratio = TMath::Log(PE0/PE1);
    ratio = PE0/PE1;
    gpos->SetPoint(i,ratio,position[i]);
    
   }
  //TCanvas* tcan = new TCanvas("tcan","tcan",1000,800);
  //h->Draw();
  //Peaks->Draw("same");
  
  TGraphErrors *gsqrt = new TGraphErrors(imax);
  TGraphErrors *gscor = new TGraphErrors(imax);
  TGraphErrors *glnscor = new TGraphErrors(imax);
  
  Double_t x_0=0;
  Double_t y_0=0;
  Double_t x_1=0;
  Double_t y_1=0;
  
  TCanvas* can0 =new TCanvas("SRDistributionsAll","SRDistributionsAll",1000,800);
  TCanvas* can1 =new TCanvas("SRDistributionsSeperate","SRDistributionsSeperate",1000,800);
  can1->Divide(3,3);
  
  
  
  for(Int_t i=0;i<imax;i++){
    gch0->GetPoint(i,x_0,y_0);
    gch1->GetPoint(i,x_1,y_1);
    gsqrt->SetPoint(i,position[i],sqrt(y_0*y_1));
    gscor->SetPoint(i,position[i],2*y_1*sqrt(y_0/y_1));
    glnscor->SetPoint(i,position[i],log(sqrt(y_0/y_1)));
    can1->cd(i+1);
    hratio[i]->Draw("");
    can0->cd();
    hratio[i]->SetLineColor(i+1);
    hratio[i]->Draw("same");
    
    for(Int_t j=1;j<hratio[i]->GetNbinsX()+1;j++){
      posvsratio->Fill(position[i],hratio[i]->GetBinCenter(j),hratio[i]->GetBinContent(j));
    }
  }
  
  TCanvas* can2 = new TCanvas("Attenuation511","Attenuation511",1000,800);
  TF1* Ch0_fun = new TF1("fun_ch0","expo(0)",10,90);
  TF1* Ch1_fun; 
  
  gch0->Fit("fun_ch0","R");
  if(fibre==2){
    Ch1_fun = new TF1("fun_ch1","expo(0)",35,100);
    gch1->Fit("fun_ch1","R");
  }
  if(fibre==1){
    gch1->RemovePoint(3);
    Ch1_fun = new TF1("fun_ch1","expo(0)",10,90);
    gch1->Fit("fun_ch1","R");
  }
  if(fibre==3){
    Ch1_fun = new TF1("fun_ch1","expo(0)",10,90);
    gch1->Fit("fun_ch1","R");
  }
  if(fibre==4){
    Ch1_fun = new TF1("fun_ch1","expo(0)",10,90);
    gch1->Fit("fun_ch1","R");
  }
  if(fibre==5){
    Ch1_fun = new TF1("fun_ch1","expo(0)",10,90);
    gch1->Fit("fun_ch1","R");
  }
  
  gch0->SetMarkerStyle(21);
  gch0->GetYaxis()->SetRangeUser(180,280);
  gch0->SetTitle("Ch_0");
  gch0->GetXaxis()->SetTitle("mm on fibre");
  gch0->GetYaxis()->SetTitle("#P.E.");
  gch0->Draw("AP");
  gch1->SetMarkerColor(2);
  gch1->SetMarkerStyle(21);
  gch1->SetTitle("Ch_1");
  gch1->Draw("PSAME");
  gsqrt->SetMarkerColor(3);
  gsqrt->SetMarkerStyle(21);
  gsqrt->SetTitle("Sqrt(Ch_0*Ch_1)");
  gsqrt->Draw("PSAME");
  gscor->SetMarkerColor(4);
  gscor->SetMarkerStyle(21);
  gscor->SetTitle("Sqrt(Ch_0/Ch_1)");
  gscor->Draw("PSAME");
  can2->BuildLegend();
  
  TCanvas* can3 = new TCanvas("Signalratio511","Signalratio511",1000,800);
  can3->SetGrid(1,1);
  gpos->SetTitle("Signal ratio");
  gpos->GetYaxis()->SetTitle("position");
  gpos->GetXaxis()->SetTitle("Signal ratio");
   
  gpos->Draw("AP");
   
   
  
  cout << "Ch0: " << 1/Ch0_fun->GetParameter(1) << "+/-" << sqrt(Ch0_fun->GetParError(1)/Ch0_fun->GetParameter(1)/Ch0_fun->GetParameter(1)) << endl;
  
  cout << "Ch1: " << 1/Ch1_fun->GetParameter(1) << "+/-" << sqrt(Ch1_fun->GetParError(1)/Ch1_fun->GetParameter(1)/Ch1_fun->GetParameter(1)) << endl;
  
  TCanvas* can4 = new TCanvas("SignalratiovsPos","SignalratiovsPos",1000,800);
  posvsratio->Draw("COLZ");

  glnscor->Fit("pol1");
  
  TCanvas* can5 = new TCanvas("lnscor511","lnscor511",1000,800);
  glnscor->Draw("AP");

  
  TCanvas* can6 = new TCanvas("lnscor","lnscor",1000,800);
  can6->Divide(3,3);
  
  TGraphErrors * glnmbf = new TGraphErrors(imax);
  
  TF1* mygaus = new TF1("mygaus","gaus",-0.4,0.4);
  
  for(Int_t i=0;i<imax;i++){
   can6->cd(i+1);
   hlnscor[i]->Fit("mygaus","R");
   hlnscor[i]->Draw();
   glnmbf->SetPoint(i,position[i],mygaus->GetParameter(1));
  }

  
  TCanvas* can7 = new TCanvas("lnmbf","lnmbf",1000,800);
  glnmbf->Fit("pol1");
  glnmbf->Draw("AP");
  
  return kTRUE;
}