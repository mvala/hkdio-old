#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include "HkEvent.h"

#include "HkAnalysisSelector.h"

ClassImp(HkAnalysisSelector)

    HkAnalysisSelector::HkAnalysisSelector(TTree * /*tree*/)
    : TSelector(), fChain(0), fEvent(0), fTaskMgr(0) {}

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
}

Bool_t HkAnalysisSelector::Process(Long64_t entry) {

  GetEntry(entry);

  Printf("Event ID=%lld", fEvent->GetID());

  return kTRUE;
}

void HkAnalysisSelector::SlaveTerminate() {}

void HkAnalysisSelector::Terminate() {}

Int_t HkAnalysisSelector::GetEntry(Long64_t entry, Int_t getall) {
  return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0;
}
