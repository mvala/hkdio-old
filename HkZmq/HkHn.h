#ifndef HkHn_cxx
#define HkHn_cxx

#include <Rtypes.h>
#include <map>
#include <string.h>
#include <zyre.h>

using namespace std;

///
/// \class HkHn
///
/// \brief hkdio histogram
///	\author Martin Vala <mvala@cern.ch>
///
class THnSparse;
class HkHn {

public:
  HkHn();
  virtual ~HkHn();

  virtual void Print(bool verbose = false);

  void SetFiller(int id, string name) { fFillers[id] = name; }
  void SetBin(int id, string name) { fBins[id] = name; }

  void SetHistogram(THnSparse *h) { fHist = h; }
  THnSparse *GetHistogram() { return fHist; }

  virtual Int_t Fill(Int_t size, Double_t *val, Double_t w = 1.0);
  virtual Int_t FillFast(Double_t *val, Double_t w = 1.0);

  bool Start();
  bool Stop();

  static void StartFiller(zsock_t *pipe, void *args);
  static void StartBin(zsock_t *pipe, void *args);
  static void SetDiscovery(string url) { fDiscoveryUrl = url; }

private:
  map<int, string> fFillers;
  map<int, string>::iterator fFillIter;
  map<int, string> fBins;
  map<int, string>::iterator fBinIter;
  THnSparse *fHist;
  zlist_t *fListOfActors;
  static string fDiscoveryUrl;
};

#endif
