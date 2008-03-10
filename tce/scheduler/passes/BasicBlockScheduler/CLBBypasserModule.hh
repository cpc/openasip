#ifndef TCE_CLB_BYPASSER_MODULE_HH
#define TCE_CLB_BYPASSER_MODULE_HH

//#include "HelperSchedulerModule.hh"

#include "BypasserModule.hh"

#include <map>
#include <string>

class CycleLookBackSoftwareBypasser;
class SoftwareBypasser;

class CLBBypasserModule : public BypasserModule {
    
public:
    CLBBypasserModule();
    virtual ~CLBBypasserModule();

    SoftwareBypasser& bypasser();
    void setOptions(const std::vector<ObjectState*>& options);

private:
    static const std::string LOOKBACK_DISTANCE_OPTION;
    int bypassDistance_;
    CycleLookBackSoftwareBypasser* clbs_;
};

#endif
