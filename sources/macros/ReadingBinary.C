#include <fstream>
#include <iostream>
using namespace std;

Bool_t WriteBinary(void){
 
  ofstream binary_output("test_1.dat", ios::binary);
  ofstream ascii_output("test_1.txt");
  
  double num = 2128.084229;
  double temp;
  
  for(Int_t i=0; i<5; i++){
    temp=num+i;
    ascii_output << Form("%.6f",temp) << endl;
    binary_output.write((const char*)&temp, sizeof temp);
  }
  
  return kTRUE;
}

Bool_t ReadingBinary(Int_t index = 0){
  
  Int_t imin = index*1024;
  Int_t imax = imin+1024;
  ifstream binary_input("../data/test/wave_1.dat", ios::binary);
  ifstream ascii_input("../data/test/wave_1.txt");
  
  TCanvas *can = new TCanvas("can","can",1000,500);
  can->Divide(2,1);
  
  TGraph *gBinary = new TGraph(1024);
  TGraph *gAscii = new TGraph(1024);
  
  float binary_number;
  float ascii_number;
  
  for(Int_t i=0; i<imax; i++){
    binary_input.read((char*)&binary_number,sizeof(binary_number));
    ascii_input >> ascii_number;
    if(i<imin) continue;
    gBinary->SetPoint(i-imin,i-imin,binary_number);
    gAscii->SetPoint(i-imin,i-imin,ascii_number);
    //cout << Form("%.6f",binary_number) << "\t" << Form("%.6f",ascii_number) << endl;
  }
  
  gBinary->SetMarkerStyle(8);
  gBinary->SetTitle("Binary - signal");
  gAscii->SetMarkerStyle(8);
  gAscii->SetTitle("ASCII - signal");
  
  can->cd(1);
  gBinary->Draw("APC");
  can->cd(2);
  gAscii->Draw("APC");
  
  binary_input.close();
  ascii_input.close();
  
  return kTRUE;
}

Bool_t CountNumbers(void){
 
  ifstream binary_input("wave_1.dat", ios::binary);
  int i = 0;
  float binary_number;
  
  while(!binary_input.eof()){
    binary_input.read((char*)&binary_number,sizeof binary_number);
    i++;
  }
  
  cout << "This file has " << i << " numbers in it" << endl;
  
  return kTRUE;
}