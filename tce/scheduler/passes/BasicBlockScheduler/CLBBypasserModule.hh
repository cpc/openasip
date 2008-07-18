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
    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;

private:
    static const std::string LOOKBACK_DISTANCE_OPTION;
    static const std::string KILL_DEAD_RESULTS_OPTION;

    CycleLookBackSoftwareBypasser* clbs_;
    int bypassDistance_;
    bool killDeadResults_;
};

#endif
