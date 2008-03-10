#ifndef TCE_DS_FILLER_MODULE 
#define TCE_DS_FILLER_MODULE

#include "HelperSchedulerModule.hh"

class CopyingDelaySlotFiller;

class DSFillerModule : public HelperSchedulerModule {
public:
    virtual CopyingDelaySlotFiller& filler() = 0;
    virtual ~DSFillerModule();
};

#endif
