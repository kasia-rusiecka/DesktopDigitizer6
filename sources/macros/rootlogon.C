{
  if(!gSystem->Load("libDesktopDigitizer6.so"))
    cout<<"libDesktopDigitizer6.so loaded..."<<endl;
  else
    cout<<"Problems with loading libDesktopDigitizer6.so"<<endl;
  gROOT->LoadMacro("SignalsViewer.C");
}
