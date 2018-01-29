// *****************************************
// *                                       *
// *           DesktopDigitizer            *
// *             DDSignal.hh               *
// *          Katarzyna Rusiecka           *
// * katarzyna.rusiecka@doctoral.uj.edu.pl *
// *          Created in 2017              *
// *                                       *
// *****************************************

#ifndef __DDSignal_H_
#define __DDSignal_H_ 1
#include "TObject.h"
#include <iostream>

class DDSignal : public TObject{
  
private:
  Float_t fAmp;		// amplitude
  Float_t fT0;		// t0
  Float_t fTOT;		// time over threshold
  Float_t fCharge;	// charge
  Float_t fPE;
  
public:
  DDSignal();
  DDSignal(Float_t amplitude, Float_t t0, Float_t tot, Float_t charge, Float_t pe);
  DDSignal(Float_t *parameters);
  ~DDSignal();
  
  void SetAmplitude(Float_t amplitude){ fAmp = amplitude; };
  void SetT0(Float_t t0){ fT0 = t0; };
  void SetTOT(Float_t tot){ fTOT = tot; };
  void SetCharge(Float_t charge){ fCharge = charge; };
  void SetPE(Float_t pe){ fPE = pe; };
  
  Float_t GetAmplitude(void){ return fAmp; };
  Float_t GetT0(void){ return fT0; };
  Float_t GetTOT(void){ return fTOT; };
  Float_t GetCharge(void){return fCharge; };
  Float_t GetPE(void){return fPE; };
  
  void SetAll(Float_t *parameters);
  void Clear(void);
  void Print(void);

  ClassDef(DDSignal,1)
  
};

#endif
