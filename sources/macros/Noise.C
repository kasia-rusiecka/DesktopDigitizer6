Bool_t Noise(Int_t channel, Int_t offset, Int_t Number){
  
  TString path = "./";
  Int_t ipoints = 1024;
  Int_t counter = 1;
  float x;
  
  TString title = Form("Noise channel %i",channel);
  TString title1 = Form("Noise channel %i (sigma)",channel);
  TCanvas *can = new TCanvas(title,title,800,800);
  TCanvas *can1 = new TCanvas(title1,title1,800,800);
  TH1F *h0 = new TH1F("h0","h0",500,0,50);
  TH1F *h = new TH1F("h","h",1200,500,700);
  TH1F *hsigma = new TH1F("hsigma","hsigma",1000,1,3);
  
  TString fname = path+Form("wave_%i.dat",channel);	//only binary files support
  ifstream input(fname, ios::binary);
  
  float min= 100000;
  float max= -1000;
  
  for(Int_t j=0;j<Number;j++){
    min= 100000;
    max= -1000;
    for(Int_t i=0; i<ipoints; i++){
      input.read((char*)&x,sizeof(x));
      h->Fill(x/4.096);
      if(x<min){
	min=x;
	
      }
      else if(x>max){
	max=x;	
      }
     }
    h0->Fill((max-min)/4.096);
    hsigma->Fill(h->GetRMS());
    
   }
  
  can->SetLogy();
  h0->Draw("");
  TString name = Form("DC_channel_offset_%i",offset);
  h0->SetName(name);
  //h0->SaveAs(name+".root");
  
  TString name1 = Form("DC_channel_offset_%i_sigma",offset);
  can1->cd();
  hsigma->Draw();
  hsigma->SaveAs(name1+".root");
  
  return kTRUE;
}

Bool_t Plot(Int_t channel, Int_t Number){
  
  TString path = "./";
  Int_t ipoints = 1024;
  float x;
  
  TString title = Form("Signals channel %i",channel);
  TCanvas *can = new TCanvas(title,title,800,800);
  TGraphErrors *g = new TGraphErrors(Number*ipoints);
  
  TString fname = path+Form("wave_%i.dat",channel);	//only binary files support
  ifstream input(fname, ios::binary);
  
  float max=0;
  float base=0;
  
  
  for(Int_t i=0; i<50; i++){
      input.read((char*)&x,sizeof(x));
      base+=x;
    }
  base=base/50;
  input.clear();
  input.seekg(0, ios::beg);
    
  
  for(Int_t j=0;j<Number;j++){
    max=0;
    
   for(Int_t i=0; i<ipoints; i++){
      input.read((char*)&x,sizeof(x));
      g->SetPoint((j*ipoints)+i,i,x-base);
      g->SetPointError((j*ipoints)+i,1/sqrt(12),10);
      if(x>max) max=x;
    }  
  }
//  can->SetLogy();
  
   TF1* fun_pre= new TF1("fun_pre","[0]*(exp(x/[1]))",150,1000);
   fun_pre->SetParameter(0,max);
   fun_pre->SetParameter(1,-5);
 
   g->Fit("fun_pre","R");
//   
//   
//    TF1* fun_pre1= new TF1("fun_pre1","[0]*(1-exp(x/[1]))",110,120);
//    fun_pre1->SetParameter(0,max);
//    fun_pre1->SetParameter(1,-0.5);
//  
//   g->Fit("fun_pre1","R");
//  
//   TF1* fun= new TF1("fun","[0]*(exp(x/[1])-exp(x/[2]))",110,1000);
//   fun->SetParameter(0,max);
//   fun->SetParameter(1,-20);
//   fun->SetParameter(2,-0.1);
// 
//   g->Fit("fun","R");
//  
 
  g->Draw("AP");
  
  return kTRUE;
}
