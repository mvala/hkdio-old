#ifndef HkTaskExample_cxx
#define HkTaskExample_cxx

#include <HkTask.h>

class HkTaskExample : public HkTask {

public:
  HkTaskExample(const char *name = "rsn", const char *title = "");
  virtual ~HkTaskExample();

  virtual void Exec(Option_t *option);

private:
  ClassDef(HkTaskExample, 1)
};

#endif
