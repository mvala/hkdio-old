#ifndef HkAnalysisSelector_H
#define HkAnalysisSelector_H

#include <TH1D.h>
#include <TH2D.h>
#include <TSelector.h>

class TTask;
class TTree;
class HkEvent;

///
/// \class HkAnalysisSelector
///
/// \brief Task selector
///	\author Martin Vala <mvala@cern.ch>
///

class HkAnalysisSelector : public TSelector {

public:
  HkAnalysisSelector(TTree *tree = 0);
  virtual ~HkAnalysisSelector();

  /// Returns selector version
  virtual Int_t Version() const { return 2; }
  virtual void Begin(TTree * /*tree*/);
  virtual void SlaveBegin(TTree *tree);
  virtual void Init(TTree *tree);
  virtual Bool_t Notify();
  virtual Bool_t Process(Long64_t entry);
  virtual Int_t GetEntry(Long64_t entry, Int_t getall = 0);
  virtual void SlaveTerminate();
  virtual void Terminate();

private:
  /// Pointer to the analyzed TTree or TChain
  TTree *fChain; //!

  HkEvent *fEvent; ///< Current Event
  TTask *fTaskMgr; ///< Task Manager

  /// \cond CLASSIMP
  ClassDef(HkAnalysisSelector, 1);
  /// \endcond
};

#endif
