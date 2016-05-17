#if defined(__MAKECINT__) || defined(__MAKECLING__)
#include <TFile.h>
#include <TTree.h>
#include <HkEvent.h>
#include <HkTrack.h>
#endif
void Generate(Int_t nEvents=10,Int_t nTracks = 100,const char *fname="hk.root", Int_t idStart=0) {

  TFile *f = TFile::Open(fname,"RECREATE"); 
  TTree *tree = new TTree("hkTree","Hk Tree");
  HkEvent *ev = new HkEvent(0);
  tree->Branch("hkEvent", &ev);
  
  for (Int_t iEvent=idStart; iEvent < nEvents+idStart; iEvent++) {
    ev->SetID(iEvent);
    ev->BuildVertexRandom();
    HkTrack *t = 0;
    for (Int_t iTrack=0;iTrack<nTracks;iTrack++) {
      t = ev->AddTrack();
      t->BuildRandom();
    }

    tree->Fill();
    ev->Clear();
  }
  
  f->Write("",TObject::kOverwrite);
  f->Close();
}
