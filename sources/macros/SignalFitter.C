Bool_t SignalFitter(Int_t channel, Double_t xmin, Double_t xmax,Int_t Number){
  
  int mode=1; //Mode = 0 dark noise; Mode =1 crystal signals
  
  Int_t ipoints=1024;
  float x_file;
  int position=0;

  TFile *file = new TFile("results.root","READ");
  TTree *tree = (TTree*)file->Get("tree_ft");
  
  TString title = Form("channel_%i",channel);
  //TGraphErrors *gSignal= new TGraphErrors(Number*ipoints);
  TProfile *gSignal = new TProfile("pSignal",";t/ns;U/mV",Number*ipoints,0,ipoints,"");
  
  TString fname = Form("wave_%i.dat",channel);	//only binary files support
  ifstream input(fname, ios::binary); 
  
  DDSignal *sig = new DDSignal();
  TString bname = Form("ch_%i",channel);
  tree->SetBranchAddress(bname,&sig);
  Int_t nentries;

  nentries = tree->GetEntries();
  Int_t counter=0;
  
  Double_t baseline=0;
  
  Double_t mean_T0=0;
  Double_t firstT0=0;
 
  
  for(Long64_t i=0; i<nentries; i++) {
     tree->GetEntry(i);
     baseline=0;
     if(sig->GetPE()>xmin && sig->GetPE()<xmax){
	
	if(firstT0==0) firstT0=sig->GetT0();
	else if(abs(sig->GetT0()-firstT0)<1){
	  mean_T0+=sig->GetT0();
	  position=sizeof(x_file)*ipoints*i;
	  input.seekg(position);
	  for(Int_t ii=0; ii<50; ii++){
	    input.read((char*)&x_file,sizeof(x_file));
	    baseline+=x_file/4.096;    
	  }
	  baseline=baseline/50;
	  input.seekg(position);
	  for(Int_t ii=0; ii<ipoints; ii++){
	    input.read((char*)&x_file,sizeof(x_file));
	    //gSignal->SetPoint(ii+counter*ipoints,ii,x_file/4.096-baseline);
	    //gSignal->SetPointError(ii+counter*ipoints,1/sqrt(12),sqrt(x_file/4.096-baseline));
	    gSignal->Fill(ii,x_file/4.096-baseline);
	  }
	  if(counter<Number-1)counter++;
	  else break;
	}
     } 
  }
  mean_T0=mean_T0/(counter+1);
  
  TF1* exp_fall;
  TF1* exp_rise;
  TFitResultPtr Fit_fall;
  TFitResultPtr Fit_rise;
  TCanvas *can = new TCanvas(title,title,800,800);
    
  
  
  if (mode==0){
    cout << "Start point of Fit" << mean_T0 << endl;
  
    exp_fall = new TF1("fall","expo",mean_T0+10,mean_T0+150);
    exp_rise = new TF1("rise","expo",mean_T0-4,mean_T0);
  
    Fit_fall = gSignal->Fit("fall","RS");
    Fit_fall->Print();
    Fit_rise = gSignal->Fit("rise","RS");
    Fit_rise->Print();  
  
    gSignal->GetXaxis()->SetRangeUser(50,300);
    gSignal->GetYaxis()->SetRangeUser(0,10);
    gSignal->Draw("");
    exp_fall->Draw("same");
    exp_rise->Draw("same");
  
  
    cout << "RiseTime of the Signal fo MPPC " << 1/exp_rise->GetParameter(1) << "+/-" << sqrt(exp_rise->GetParError(1)/exp_rise->GetParameter(1)/exp_rise->GetParameter(1))  << endl;
  
    cout << "FallTime of the Signal fo MPPC " << 1/exp_fall->GetParameter(1) << "+/-" << sqrt(exp_fall->GetParError(1)/exp_fall->GetParameter(1)/exp_fall->GetParameter(1)) << endl;
  
    cout << counter+1 << " signals plotted" << endl;
  }
  else if(mode==1){
    cout << "Start point of Fit " << mean_T0 << endl;
  
    exp_fall = new TF1("fall","expo",mean_T0+100,mean_T0+400);
    exp_rise = new TF1("rise","expo",mean_T0-2,mean_T0+8);
  
    Fit_fall = gSignal->Fit("fall","RS");
    Fit_fall->Print();
    Fit_rise = gSignal->Fit("rise","RS");
    Fit_rise->Print();  
  
    gSignal->GetXaxis()->SetRangeUser(50,700);
    gSignal->Draw("AP");
    exp_fall->Draw("same");
    exp_rise->Draw("same");
  
  
    cout << "RiseTime of the Signal fo MPPC " << 1/exp_rise->GetParameter(1) << "+/-" << exp_rise->GetParError(1)/exp_rise->GetParameter(1)/exp_rise->GetParameter(1)  << endl;
  
    cout << "FallTime of the Signal fo MPPC " << 1/exp_fall->GetParameter(1) << "+/-" << exp_fall->GetParError(1)/exp_fall->GetParameter(1)/exp_fall->GetParameter(1) << endl;
  
    cout << counter+1 << " signals plotted" << endl;
    
  }
  return kTRUE;
}