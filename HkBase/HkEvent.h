#ifndef HkEvent_cxx
#define HkEvent_cxx

#include <TObject.h>
#include <TClonesArray.h>
#include "HkTrack.h"

class HkEvent : public TObject {

public:
  HkEvent();
  HkEvent(Long64_t id, Double_t vx = 0.0, Double_t vy = 0.0, Double_t vz = 0.0);
  virtual ~HkEvent();

  Long64_t GetID() const { return fID; }
  Double_t GetVx() const { return fVx; }
  Double_t GetVy() const { return fVy; }
  Double_t GetVz() const { return fVz; }

  void SetID(Long64_t id) { fID = id; }
  void SetVx(Double_t vx) { fVx = vx; }
  void SetVy(Double_t vy) { fVy = vy; }
  void SetVz(Double_t vz) { fVz = vz; }

  Long64_t GetNTrack() const { return fNTracks; }
  HkTrack *GetTrack(Long64_t i) { return (HkTrack *)fTracks->At(i); }
  HkTrack *AddTrack();

  virtual void Print(Option_t *option = "") const;
  virtual void Clear(Option_t *option = "");

  void BuildVertexRandom();

private:
  Long64_t fID;          // ID of event
  Double_t fVx;          // Vertex x
  Double_t fVy;          // Vertex y
  Double_t fVz;          // Vertex z
  Int_t fNTracks;        // Number of tracks
  TClonesArray *fTracks; //->Array with all tracks

  ClassDef(HkEvent, 1)
};

#endif
