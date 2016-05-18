#include <TString.h>
#include <TRandom.h>
#include <TMath.h>
#include "HkTrack.h"

/// \cond CLASSIMP
ClassImp(HkTrack);
/// \endcond

HkTrack::HkTrack() : TObject(), fPx(0.0), fPy(0.0), fPz(0.0), fCharge(0) {
  ///
  /// A constructor
  ///

  gRandom->SetSeed(0);
}

HkTrack::~HkTrack() {
  ///
  /// A destructor
  ///
}

void HkTrack::BuildRandom() {
  ///
  /// Building random event
  ///

  Double_t px, py;
  gRandom->Rannor(px, py);
  fPx = px;
  fPy = py;
  fPz = TMath::Sqrt(px * px + py * py);

  // Generate charge
  fCharge = (gRandom->Integer(2) > 0) ? 1 : -1;
}

void HkTrack::Print(Option_t * /*option*/) const {
  ///
  /// Printing track info
  ///

  Printf("ch=%d px=%.3f py=%.3f pz=%.3f", fCharge, fPx, fPy, fPz);
}

void HkTrack::Clear(Option_t *) {
  ///
  /// Reseting track to default values
  ///

  fCharge = 0;
  fPx = 0;
  fPy = 0;
  fPz = 0;
}
