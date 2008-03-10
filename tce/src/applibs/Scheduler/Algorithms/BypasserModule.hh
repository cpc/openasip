#ifndef TCE_BYPASSER_MODULE_HH
#define TCE_BYPASSER_MODULE_HH

#include "HelperSchedulerModule.hh"

class SoftwareBypasser;

class BypasserModule : public HelperSchedulerModule {
public:
    virtual SoftwareBypasser& bypasser() = 0;
    virtual ~BypasserModule();
};

#endif
