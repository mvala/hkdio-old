#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include "HkEvent.h"

#include "HkAnalysisSelector.h"

/// \cond CLASSIMP
ClassImp(HkAnalysisSelector);
/// \endcond

HkAnalysisSelector::HkAnalysisSelector(TTree * /*tree*/)
    : TSelector(), fChain(0), fEvent(0), fTaskMgr(0) {
  ///
  /// Default constructor
  ///
}

HkAnalysisSelector::~HkAnalysisSelector() {
  ///
  /// Destructor
  ///
  delete fEvent;
}

void HkAnalysisSelector::Init(TTree *tree) {
  ///
  /// Intialize tree
  ///
  if (!tree)
    return;
  fChain = tree;
  fChain->SetBranchAddress("hkEvent", &fEvent);
}

Bool_t HkAnalysisSelector::Notify() {
  ///
  /// Notify is called when file is changed
  ///
  if (fChain->GetCurrentFile())
    Printf("File : %s", fChain->GetCurrentFile()->GetName());
  return kTRUE;
}

void HkAnalysisSelector::Begin(TTree * /*tree*/) {
  ///
  /// First user Function called on client
  ///
}

void HkAnalysisSelector::SlaveBegin(TTree * /*tree*/) {
  ///
  /// First user Function called on proof worker
  ///

  TString option = GetOption();
}

Bool_t HkAnalysisSelector::Process(Long64_t entry) {
  ///
  /// Process event with ID=entry
  ///

  GetEntry(entry);

  Printf("Event ID=%lld", fEvent->GetID());

  return kTRUE;
}

void HkAnalysisSelector::SlaveTerminate() {
  ///
  /// Last user Function called on proof worker
  ///
}

void HkAnalysisSelector::Terminate() {
  ///
  /// Last user Function called on client
  ///
}

Int_t HkAnalysisSelector::GetEntry(Long64_t entry, Int_t getall) {
  ///
  /// Gets entry
  ///
  return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0;
}
