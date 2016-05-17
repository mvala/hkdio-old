#ifndef HkTrack_cxx
#define HkTrack_cxx

#include <TObject.h>

class HkTrack : public TObject {

public:
  HkTrack();
  virtual ~HkTrack();

  Double_t GetPx() const { return fPx; }
  Double_t GetPy() const { return fPy; }
  Double_t GetPz() const { return fPz; }
  Int_t GetCharge() const { return fCharge; }

  void SetPx(Double_t px) { fPx = px; }
  void SetPy(Double_t py) { fPy = py; }
  void SetPz(Double_t pz) { fPz = pz; }
  void SetCharge(Int_t ch) { fCharge = ch; }

  void BuildRandom();

  virtual void Print(Option_t *option = "") const;
  virtual void Clear(Option_t *option = "");

private:
  Double_t fPx;  // Momentum x
  Double_t fPy;  // Momentum y
  Double_t fPz;  // Momentum z
  Int_t fCharge; // Charge

  ClassDef(HkTrack, 1)
};

#endif
