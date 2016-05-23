#include "HkTaskExample.h"

ClassImp(HkTaskExample)

    HkTaskExample::HkTaskExample(const char *name, const char *title)
    : HkTask(name, title) {}

HkTaskExample::~HkTaskExample() {}

void HkTaskExample::Exec(Option_t * /*option*/) {}
