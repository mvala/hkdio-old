#ifndef HkTaskRsn_cxx
#define HkTaskRsn_cxx

#include <HkTask.h>

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

  virtual void Exec(Option_t *option);

private:
  /// \cond CLASSIMP
  ClassDef(HkTaskRsn, 1);
  /// \endcond
};

#endif
