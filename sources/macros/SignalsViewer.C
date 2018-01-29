#include <fstream>
using namespace std;

Double_t GetBaseLine(Int_t channel, TString path){
 
  Double_t baseLine = 0;
  TString fname = path+"results.root";
  
  TFile *file = new TFile(fname,"READ");
  TGraph *g = (TGraph*)file->Get("baseLines");
  Int_t npoints = g->GetN();
  Double_t *channels = g->GetX();
  Double_t *lines = g->GetY();
  
  for(Int_t i=0; i<npoints; i++){
   if(channels[i]==channel) baseLine = lines[i]; 
  }
  
  return baseLine;
}

Double_t CalculateBaseLine(Int_t channel, TString path){
 
  Int_t stat = gSystem->Load("libDesktopDigitizer6.so");
  cout << "Loading library status is... " << stat << endl;
  
  Double_t baseLine = 0.;
  Double_t sigma = 0.;
  
  DDTreeMaker *maker = new DDTreeMaker(path);
  maker->FindBaseLine(channel,kFALSE,baseLine,sigma);
  delete maker;
  
  return baseLine;
}

Bool_t SignalsViewer(Int_t channel0, Int_t channel1,Int_t ylimit, bool BL){
  
  TString path = "./";
  Int_t ipoints = 1024;
  Int_t counter = 1;
  float x;
  float y;
  
  TString title = Form("channel_%i_and_%i",channel0,channel1);
  TCanvas *can = new TCanvas(title,title,800,800);
  TH1F *h0 = new TH1F("h0","h0",ipoints,0,ipoints);
  TH1F *h1 = new TH1F("h1","h1",ipoints,0,ipoints);
  h1->SetLineColor(kRed);
  //h1->SetMarkerColor(kRed);
  
  TString fname0 = path+Form("wave_%i.dat",channel0);	//only binary files support
  ifstream input0(fname0, ios::binary);
  TString fname1 = path+Form("wave_%i.dat",channel1);	//only binary files support
  ifstream input1(fname1, ios::binary); 
  
  Double_t baseLine0;
  Double_t baseLine1;
  //Double_t baseLine = CalculateBaseLine(channel, path);
  
  while(!input0.eof()){
    baseLine0=0;
    baseLine1=0;
    for(Int_t i=0; i<ipoints; i++){
      input0.read((char*)&x,sizeof(x));
      input1.read((char*)&y,sizeof(y));

      h0->SetBinContent(i,x/4.096);
      h1->SetBinContent(i,y/4.096);
    
    }
    for(Int_t i=0; i<50; i++){
      baseLine0+=h0->GetBinContent(i);
      baseLine1+=h1->GetBinContent(i);
    }
    
    baseLine0=baseLine0/50;
    baseLine1=baseLine1/50;
    if(BL){
      for(Int_t i=0; i<ipoints; i++){
	h0->SetBinContent(i,h0->GetBinContent(i)-baseLine0);
	h1->SetBinContent(i,h1->GetBinContent(i)-baseLine1);
      }
    }
   
    gPad->SetGrid(1,1);
    h0->SetTitle(Form("Signal number %i",counter));
    if(ylimit!=0){
      h0->GetYaxis()->SetRangeUser(-10,ylimit);
      h1->GetYaxis()->SetRangeUser(-10,ylimit);
    }
    else{
      h0->GetYaxis()->SetRangeUser(h0->GetBinContent(20)-10,h0->GetBinContent(h0->GetMaximumBin())+20);
      h1->GetYaxis()->SetRangeUser(h1->GetBinContent(20)-10,h1->GetBinContent(h0->GetMaximumBin())+20);
    }
    h0->Draw();
    h1->Draw("same");
    can->Update();
    counter++;
    can->WaitPrimitive();
  }
  
  return kTRUE;
}

Bool_t SignalsViewer(Int_t channel, bool BL){
  
  TString path = "./";
  Int_t ipoints = 1024;
  Int_t counter = 1;
  float x;
  
  TString title = Form("channel_%i",channel);
  TCanvas *can = new TCanvas(title,title,800,800);
  TGraph *g = new TGraph(ipoints);
  
  TString fname = path+Form("wave_%i.dat",channel);	//only binary files support
  ifstream input(fname, ios::binary); 
  
  Double_t baseLine = GetBaseLine(channel, path);
  //Double_t baseLine = CalculateBaseLine(channel, path);
  
  while(!input.eof()){
    
    for(Int_t i=0; i<ipoints; i++){
      input.read((char*)&x,sizeof(x));
      if(BL){
	g->SetPoint(i,i,x/4.096-baseLine);
      }
      else{
	//g->SetPoint(i,i,x);
	g->SetPoint(i,i,x/4.096);
      }
    }
    
    can->WaitPrimitive();
    gPad->SetGrid(1,1);
    g->SetTitle(Form("Signal number %i",counter));
    g->Draw();
    can->Update();
    counter++;
  }
  
  return kTRUE;
}

Bool_t CutAndView(Int_t channel, TString mode, Double_t xmin, Double_t xmax,Int_t Number){
  
  Int_t ipoints=1024;
  float x;
  Int_t position=0;
 
  TFile *file = new TFile("results.root","READ");
  TTree *tree = (TTree*)file->Get("tree_ft");
  
  TString title = Form("channel_%i",channel);
  TCanvas *can = new TCanvas(title,title,800,800);
  vector<TH1F*> h(Number); 
  for(Int_t i=0; i<Number;i++){  
    h[i] = new TH1F(Form("h_%i",i),Form("h_%i",i),ipoints,0,ipoints);
  }
  
  TString fname = Form("wave_%i.dat",channel);	//only binary files support
  ifstream input(fname, ios::binary); 
  
  DDSignal *sig = new DDSignal();
  TString bname = Form("ch_%i",channel);
  tree->SetBranchAddress(bname,&sig);
  Int_t nentries;

  nentries = tree->GetEntries();
  Int_t counter=0;
  
  Double_t baseline=0;
 
  
  for(Long64_t i=0; i<nentries; i++) {
     tree->GetEntry(i);
     baseline=0;
     if(mode=="amp"){
      if(sig->GetAmplitude()>xmin && sig->GetAmplitude()<xmax){
	
	position=sizeof(x)*ipoints*i;
	input.clear();
	input.seekg(0, ios::beg);
	input.seekg(position);
	for(Int_t ii=0; ii<ipoints; ii++){
	  input.read((char*)&x,sizeof(x));
	  if(ii<50) baseline+=x/4.096;
	  if(Number==1){
	    h[0]->SetBinContent(ii+1,x/4.096);
	  }
	  else{
	    h[counter]->SetBinContent(ii+1,x/4.096);
	  }
	    
	}
	baseline=baseline/50;
	for(Int_t ii=0; ii<ipoints; ii++){
	 if(Number==1){
	    h[0]->SetBinContent(ii+1,h[0]->GetBinContent(ii+1)-baseline);
	  }
	  else{
	    h[counter]->SetBinContent(ii+1,h[counter]->GetBinContent(ii+1)-baseline);
	  }
	}
	
	
	if(Number==1){
	  h[0]->Draw("");
	  can->WaitPrimitive();
	}
	else{
	  h[counter]->Draw("Same");
	  if(counter <Number-1) counter++;
	  else break;
	}
      }
     }
     else if(mode=="charge"){
      if(sig->GetCharge()>xmin && sig->GetCharge()<xmax){
	input.seekg(sizeof(x)*ipoints*i);
	for(Int_t ii=0; ii<ipoints; ii++){
	  input.read((char*)&x,sizeof(x));
	  if(ii<50) baseline+=x/4.096;
	  if(Number==1){
	    h[0]->SetBinContent(ii+1,x/4.096);
	  }
	  else{
	    h[counter]->SetBinContent(ii+1,x/4.096);
	  }  
	}
	
	baseline=baseline/50;
	for(Int_t ii=0; ii<ipoints; ii++){
	 if(Number==1){
	    h[0]->SetBinContent(ii+1,h[0]->GetBinContent(ii+1)-baseline);
	  }
	  else{
	    h[counter]->SetBinContent(ii+1,h[counter]->GetBinContent(ii+1)-baseline);
	  }
	}
	
	if(Number==1){
	  h[0]->Draw("");
	  can->WaitPrimitive();
	}
	else{
	  h[counter]->Draw("Same");
	  if(counter <Number-1) counter++;
	  else break;
	}
      }
     }
     else if(mode=="t0"){
      if(sig->GetT0()>xmin && sig->GetT0()<xmax){
	input.seekg(sizeof(x)*ipoints*i);
	for(Int_t ii=0; ii<ipoints; ii++){
	  input.read((char*)&x,sizeof(x));
	  if(ii<50) baseline+=x/4.096;
	  if(Number==1){
	    h[0]->SetBinContent(ii+1,x/4.096);
	  }
	  else{
	    h[counter]->SetBinContent(ii+1,x/4.096);
	  }  
	}
	
	baseline=baseline/50;
	for(Int_t ii=0; ii<ipoints; ii++){
	 if(Number==1){
	    h[0]->SetBinContent(ii+1,h[0]->GetBinContent(ii+1)-baseline);
	  }
	  else{
	    h[counter]->SetBinContent(ii+1,h[counter]->GetBinContent(ii+1)-baseline);
	  }
	}
	
	if(Number==1){
	  h[0]->Draw("");
	  can->WaitPrimitive();
	}
	else{
	  h[counter]->Draw("Same");
	  if(counter <Number-1) counter++;
	  else break;
	}
      }
     }
     else if(mode=="PE"){
      if(sig->GetPE()>xmin && sig->GetPE()<xmax){
	input.seekg(sizeof(x)*ipoints*i);
	for(Int_t ii=0; ii<ipoints; ii++){
	  input.read((char*)&x,sizeof(x));
	  if(ii<50) baseline+=x/4.096;
	  if(Number==1){
	    h[0]->SetBinContent(ii+1,x/4.096);
	  }
	  else{
	    h[counter]->SetBinContent(ii+1,x/4.096);
	  }  
	}
	
	baseline=baseline/50;
	for(Int_t ii=0; ii<ipoints; ii++){
	 if(Number==1){
	    h[0]->SetBinContent(ii+1,h[0]->GetBinContent(ii+1)-baseline);
	  }
	  else{
	    h[counter]->SetBinContent(ii+1,h[counter]->GetBinContent(ii+1)-baseline);
	  }
	}
	
	if(Number==1){
	  h[0]->Draw("");
	  can->WaitPrimitive();
	}
	else{
	  h[counter]->Draw("Same");
	  if(counter <Number-1) counter++;
	  else break;
	}
      }
     }
     
  }
  
  return kTRUE;
}