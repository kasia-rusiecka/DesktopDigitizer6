Bool_t Oscnoise(void){

vector <TGraph*> graphs(50);
TH1F* histos = new TH1F("Noisehisto","Noisehisto",200,-5,5);    
Double_t x =0;
Double_t y =0;

TString name;
TString path ="/home/kasia/workspace/Oscilloscope/26.01.2018/";

TH1D* sigma = new TH1D("sigma","sigma", 200,0,1);
  for(int i = 0;i< 50; i++){
    
    name = Form ("noise_%i_3.csv",i),
    
    graphs[i] = new TGraph(path+name,"%lg%lg",",");
    for(int j=0;j<graphs[i]->GetN();j++){
      graphs[i]->GetPoint(j,x,y);
      histos->Fill(y*1000);
    }
    sigma->Fill(histos->GetRMS());
    histos->Reset();
  }
  
  TCanvas* can = new TCanvas("can","can",1000,1000);
  //histos->Draw();	
  sigma->Draw("");
  return kTRUE;
}
