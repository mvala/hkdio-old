#ifndef HkTaskRsn_cxx
#define HkTaskRsn_cxx

#include "HkEvent.h"
#include "HkTask.h"

class THnSparse;

///
/// \class HkTaskRsn
///
/// \brief Resonance task
///	\author Martin Vala <mvala@cern.ch>
///

class HkTaskRsn : public HkTask {

public:
  HkTaskRsn(const char *name = "HkTaskRsn",
            const char *title = "HkTaskRsn title");
  virtual ~HkTaskRsn();

  /// \fn void SetNSkip(Int_t nSkip)
  /// Sets number of skipped tracks
  ///
  /// \param nSkip Number of skipped tracks
  ///

  virtual void Init(Option_t *option);
  virtual void Exec(Option_t *option);

  void SetNSkip(Long64_t nSkip) { fNSkip = nSkip; }

private:
  /// Current event
  HkEvent *fEvent; //!
  /// Tmp value for filling histogram
  Double_t *fFillTmp; //!

  THnSparse *fSigBg;        ///< Signal Background
  THnSparse *fBgLikePP;     ///< Background like sign ++
  THnSparse *fBgLikeMM;     ///< Background like sign --
  THnSparse *fBgEventMixPM; ///< Background Event mixing +-
  Long64_t fNSkip;          ///< Skip N tracks (just for testing)

  /// \cond CLASSIMP
  ClassDef(HkTaskRsn, 1);
  /// \endcond
};

#endif
