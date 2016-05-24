/// \file Analyse.C
/// \brief Analysis macro of task selector
///

#if defined(__MAKECINT__) || defined(__MAKECLING__)
#include <TChain.h>
#include <HkAnalysisSelector.h>
#endif
void Analyse(Int_t nTasks = 10, const char *fname = "hk.root",
             Long64_t nEvents = 0, Long64_t idStart = 0, Long64_t nSkip = 1) {
  TChain *ch = new TChain("hkTree");
  ch->AddFile(fname);

  HkAnalysisSelector *sel = new HkAnalysisSelector();

  HkTaskInput *tm = new HkTaskInput("hkTaskMgr", "Hk task manager");

  HkTaskRsn *tRsn;
  for (Int_t i = 0; i < nTasks; i++) {
    tRsn = new HkTaskRsn(TString::Format("rsn%05d", i).Data());
    tRsn->SetNSkip(nSkip);
    tm->Add(tRsn);
  }

  sel->SetTaskManager(tm);

  if (nEvents)
    ch->Process(sel, "", nEvents);
  else
    ch->Process(sel);
}
