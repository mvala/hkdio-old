#include <THnSparse.h>
#include "HkEvent.h"
#include "HkTrack.h"
#include "HkTaskRsn.h"

/// \cond CLASSIMP
ClassImp(HkTaskRsn);
/// \endcond

HkTaskRsn::HkTaskRsn(const char *name, const char *title)
    : HkTask(name, title), fEvent(0), fFillTmp(0), fSigBg(0), fBgLikePP(0),
      fBgLikeMM(0), fBgEventMixPM(0), fNSkip(0) {
  ///
  /// Default constructor
  ///
}

HkTaskRsn::~HkTaskRsn() {
  ///
  /// Destructor
  ///
  delete[] fFillTmp;
}

void HkTaskRsn::Init(Option_t *option) {
  ///
  /// Initialize histograms for resonance study
  ///

  Int_t nBins = 3;
  Int_t bins[nBins] = {100, 20, 200};
  Double_t min[nBins] = {0., 0., 0.};
  Double_t max[nBins] = {10., 10., 200.};

  if (!fFillTmp)
    fFillTmp = new Double_t[nBins];

  fSigBg = new THnSparseD("hSigBg", "SigBg", nBins, bins, min, max);
  fOutput->Add(fSigBg);
  fBgLikePP = new THnSparseD("hSigBg", "SigBg", nBins, bins, min, max);
  fOutput->Add(fBgLikePP);
  fBgLikeMM = new THnSparseD("hSigBg", "SigBg", nBins, bins, min, max);
  fOutput->Add(fBgLikeMM);
  //  fBgEventMixPM = new THnSparseD("hSigBg", "SigBg", nBins, bins, min, max);
  //  fOutput->Add(fBgEventMixPM);

  // Do init for sub tasks
  HkTask::Init(option);
}

void HkTaskRsn::Exec(Option_t * /*option*/) {
  ///
  /// Main function to process resonance study
  ///

  if (!fEvent) {
    HkTask *input = fParent;
    fEvent = (HkEvent *)input->GetOutput()->At(0);
  }

  //  Printf("\t%s %p", GetName(), fEvent);

  Long64_t nTracks = fEvent->GetNTrack();
  Long64_t i, j;
  HkTrack *t1, *t2;
#pragma omp parallel for private(i, j, t1, t2) firstprivate(nTracks)
  for (i = 0; i < nTracks; i++) {
    if (i % fNSkip > 0)
      continue;
    t1 = (HkTrack *)fEvent->GetTrack(i);
    //#pragma omp parallel for
    for (j = 0; j < nTracks; j++) {
      if (i == j)
        continue;
      if (j % fNSkip > 0)
        continue;

      t2 = (HkTrack *)fEvent->GetTrack(j);
      //      Printf("\tpx1=%f px2=%f", t1->GetPx(), t2->GetPx());
      fFillTmp[0] = t1->GetPx() + t2->GetPx();
      fFillTmp[1] = t1->GetPy() + t2->GetPy();
      fFillTmp[2] = nTracks;
      //#pragma omp critical
      //      {
      if (t1->GetCharge() == 1 && t2->GetCharge() == -1)
        fSigBg->Fill(fFillTmp);
      if (t1->GetCharge() == 1 && t2->GetCharge() == 1)
        fBgLikePP->Fill(fFillTmp);
      if (t1->GetCharge() == -1 && t2->GetCharge() == -1)
        fBgLikeMM->Fill(fFillTmp);
      //      }
    }
  }
}

void HkTaskRsn::Finish(Option_t * /*option*/) {
  Printf("name=%s", GetName());
  if (fSigBg)
    fSigBg->Print();
}
