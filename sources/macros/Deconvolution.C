Bool_t Deconvolution(Int_t channel, Int_t Number){
  
  
  TString path = "/home/lab/Workspace";
  
  TString Responsename ="/2018_01_22_15_37";
  TString Signalname ="/2018_01_25_8_47";
  
  Int_t ipoints=1024;
  float x_file;
  int position=0;

  TFile *Responsefile = new TFile(path+Responsename+"/results.root","READ");
  TTree *Responsetree = (TTree*)Responsefile->Get("tree_ft");
  
  TFile *Signalfile = new TFile(path+Signalname+"/results.root","READ");
  TTree *Signaltree = (TTree*)Signalfile->Get("tree_ft");
  
  TProfile *Response = new TProfile("Response",";t/ns;U/mV",Number*ipoints,0,ipoints,"");
  
  TProfile *Signal = new TProfile("Signal",";t/ns;U/mV",Number*ipoints,0,ipoints,"");
  
  TString waveResponse = path+Responsename+Form("/wave_%i.dat",channel);
  TString waveSignal = path+Signalname+Form("/wave_%i.dat",channel);	//only binary files support
  ifstream inputResponse(waveResponse, ios::binary); 
  ifstream inputSignal(waveSignal, ios::binary); 
  
  DDSignal *sigResponse = new DDSignal();
  TString bnameResponse = Form("ch_%i",channel);
  Responsetree->SetBranchAddress(bnameResponse,&sigResponse);
  
  DDSignal *sigSignal = new DDSignal();
  TString bnameSignal = Form("ch_%i",channel);
  Signaltree->SetBranchAddress(bnameSignal,&sigSignal);
  
  Int_t nentries;

  
  nentries = Responsetree->GetEntries();
  Int_t counter=0;
  Double_t xmin =0.99;
  Double_t xmax =1.01;
  
  Double_t baseline=0;
  
  Double_t firstT0=0;
 
  
  for(Long64_t i=0; i<nentries; i++) {
     Responsetree->GetEntry(i);
     baseline=0;
     if(sigResponse->GetPE()>xmin && sigResponse->GetPE()<xmax){
	
	if(firstT0==0) firstT0=sigResponse->GetT0();
	else if(abs(sigResponse->GetT0()-firstT0)<1){
	  position=sizeof(x_file)*ipoints*i;
	  inputResponse.seekg(position);
	  for(Int_t ii=0; ii<50; ii++){
	    inputResponse.read((char*)&x_file,sizeof(x_file));
	    baseline+=x_file/4.096;    
	  }
	  baseline=baseline/50;
	  inputResponse.seekg(position);
	  for(Int_t ii=0; ii<ipoints; ii++){
	    inputResponse.read((char*)&x_file,sizeof(x_file));
	    Response->Fill(ii,x_file/4.096-baseline);
	  }
	  if(counter<Number-1)counter++;
	  else break;
	}
     } 
  }
  
  
  nentries = Signaltree->GetEntries();
  counter=0;
  xmin =180;
  xmax =210;
  
  baseline=0;
  
  firstT0=0;
 
  
  for(Long64_t i=0; i<nentries; i++) {
     Signaltree->GetEntry(i);
     baseline=0;
     if(sigSignal->GetPE()>xmin && sigSignal->GetPE()<xmax){
	
	if(firstT0==0) firstT0=sigSignal->GetT0();
	else if(abs(sigSignal->GetT0()-firstT0)<1){
	  position=sizeof(x_file)*ipoints*i;
	  inputSignal.seekg(position);
	  for(Int_t ii=0; ii<50; ii++){
	    inputSignal.read((char*)&x_file,sizeof(x_file));
	    baseline+=x_file/4.096;    
	  }
	  baseline=baseline/50;
	  inputSignal.seekg(position);
	  for(Int_t ii=0; ii<ipoints; ii++){
	    inputSignal.read((char*)&x_file,sizeof(x_file));
	    Signal->Fill(ii,x_file/4.096-baseline);
	  }
	  if(counter<Number-1)counter++;
	  else break;
	}
     } 
  }
  
  TCanvas* can0 = new TCanvas("can0","can0",1000,1000);
  can0->Divide(1,2);
  can0->cd(1);
  Response->Draw("");
  can0->cd(2);
  Signal->Draw("");
  
  Signaltree->Draw("ch_1.fPE>>h(1024,0,1024)","ch_1.fPE>0 &&ch_1.fT0<590");
  TH1D* test = (TH1D*) gPad->GetPrimitive("h");
  
  Double_t source[ipoints];
  Double_t response[ipoints];

  TRandom* rnd = new TRandom();
  
  
  
  
  for(int i=0;i<ipoints;i++){
   source[i]=test->GetBinContent(i+1);
//    response[i]=Response->GetBinContent(i+1); 
   response[i]=rnd->Gaus(0,5); 
  }
  
  
  
  TSpectrum* Spec = new TSpectrum();
  
  Spec->Deconvolution(source,response,ipoints,100,1,1);
  
  TH1D* Deconvolved = new TH1D("Deconvolved","Deconvolved",ipoints,0, ipoints);
  for(int i=0;i<ipoints;i++){
    Deconvolved->SetBinContent(i+1,source[i]);
    
  }
  
  TCanvas* can1 = new TCanvas("can1","can1",1000,1000);
  Deconvolved->Draw();
  
  return kTRUE; 
}