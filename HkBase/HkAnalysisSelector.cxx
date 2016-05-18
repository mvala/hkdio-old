#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include "HkEvent.h"

#include "HkAnalysisSelector.h"

ClassImp(HkAnalysisSelector)

    HkAnalysisSelector::HkAnalysisSelector(TTree * /*tree*/)
    : TSelector(), fChain(0), fEvent(0), fHistPx(0), fHistPy(0), fHistPz(0),
      fHistPxPy(0) {}

HkAnalysisSelector::~HkAnalysisSelector() { delete fEvent; }

void HkAnalysisSelector::Init(TTree *tree) {
  if (!tree)
    return;
  fChain = tree;
  fChain->SetBranchAddress("hkEvent", &fEvent);
}

Bool_t HkAnalysisSelector::Notify() {
  if (fChain->GetCurrentFile())
    Printf("File : %s", fChain->GetCurrentFile()->GetName());
  return kTRUE;
}

void HkAnalysisSelector::Begin(TTree * /*tree*/) {}

void HkAnalysisSelector::SlaveBegin(TTree * /*tree*/) {
  TString option = GetOption();

  fHistPx = new TH1D("hPx", "p_{x}", 100, -5.0, 5.0);
  fOutput->Add(fHistPx);
  fHistPy = new TH1D("hPy", "p_{y}", 100, -5.0, 5.0);
  fOutput->Add(fHistPy);
  fHistPz = new TH1D("hPz", "p_{z}", 100, -5.0, 5.0);
  fOutput->Add(fHistPz);

  fHistPxPy =
      new TH2D("hPxPy", "p_{x} vs p_{y}", 100, -5.0, 5.0, 100, -5.0, 5.0);
  fOutput->Add(fHistPxPy);
}

Bool_t HkAnalysisSelector::Process(Long64_t entry) {

  GetEntry(entry);

  // Printf("ID=%lld", fEvent->GetID());

  HkTrack *t;
  for (Int_t iTrack = 0; iTrack < fEvent->GetNTrack(); iTrack++) {
    t = fEvent->GetTrack(iTrack);
    fHistPx->Fill(t->GetPx());
    fHistPy->Fill(t->GetPy());
    fHistPz->Fill(t->GetPz());
    fHistPxPy->Fill(t->GetPx(), t->GetPy());
  }

  return kTRUE;
}

void HkAnalysisSelector::SlaveTerminate() {}

void HkAnalysisSelector::Terminate() {
  fHistPx = dynamic_cast<TH1D *>(fOutput->FindObject("hPx"));
  fHistPy = dynamic_cast<TH1D *>(fOutput->FindObject("hPy"));
  fHistPz = dynamic_cast<TH1D *>(fOutput->FindObject("hPz"));
  fHistPxPy = dynamic_cast<TH2D *>(fOutput->FindObject("hPxPy"));

  if ((fHistPx) && (fHistPy) && (fHistPz) && (fHistPxPy)) {
    TCanvas *c = new TCanvas("cP", "Momentum distributions");
    c->Divide(2, 2);
    c->cd(1);
    fHistPx->Draw();
    c->cd(2);
    fHistPy->Draw();
    c->cd(3);
    fHistPz->Draw();
    c->cd(4);
    fHistPxPy->Draw();
  }
}

Int_t HkAnalysisSelector::GetEntry(Long64_t entry, Int_t getall) {
  return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0;
}
