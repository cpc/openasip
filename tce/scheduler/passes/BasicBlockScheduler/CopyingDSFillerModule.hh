#ifndef TCE_COPYING_DS_FILLER_MODULE_HH
#define TCE_COPYING_DS_FILLER_MODULE_HH

#include "DSFillerModule.hh"

class CopyingDelaySlotFiller;

class CopyingDSFillerModule : public DSFillerModule {

public:
    CopyingDSFillerModule();
    ~CopyingDSFillerModule();
    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;

    CopyingDelaySlotFiller& filler();
private:
    CopyingDelaySlotFiller* filler_;
    
};

#endif
