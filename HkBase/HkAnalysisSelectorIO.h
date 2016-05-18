#ifndef HkAnalysisSelectorIO_H
#define HkAnalysisSelectorIO_H

#include <TH1D.h>
#include <TH2D.h>
#include <TSelector.h>

class TTree;
class HkEvent;

class HkAnalysisSelectorIO : public TSelector {

public:
  HkAnalysisSelectorIO(TTree *tree = 0);
  virtual ~HkAnalysisSelectorIO();

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
  TTree *fChain; //! pointer to the analyzed TTree or TChain
  HkEvent *fEvent;
  TH1D *fHistPx;
  TH1D *fHistPy;
  TH1D *fHistPz;
  TH2D *fHistPxPy;

  ClassDef(HkAnalysisSelectorIO, 1)
};

#endif
