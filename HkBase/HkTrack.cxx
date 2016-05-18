#include <TString.h>
#include <TRandom.h>
#include <TMath.h>
#include "HkTrack.h"

ClassImp(HkTrack);

HkTrack::HkTrack() : TObject(), fPx(0.0), fPy(0.0), fPz(0.0), fCharge(0) {

  //  Printf("Creating HkTrack ...");
  gRandom->SetSeed(0);
}

HkTrack::~HkTrack() {
  //  Printf("Destroying HkTrack ...");
}

void HkTrack::BuildRandom() {

  Double_t px, py;
  gRandom->Rannor(px, py);
  fPx = px;
  fPy = py;
  fPz = TMath::Sqrt(px * px + py * py);

  // Generate charge
  fCharge = (gRandom->Integer(2) > 0) ? 1 : -1;
}
void HkTrack::Print(Option_t * /*option*/) const {
  Printf("ch=%d px=%.3f py=%.3f pz=%.3f", fCharge, fPx, fPy, fPz);
}
void HkTrack::Clear(Option_t *) {
  fCharge = 0;
  fPx = 0;
  fPy = 0;
  fPz = 0;
}
