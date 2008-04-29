/**
 *
 * @file CopyingDSFillerModule.cc
 * 
 * Implementation of CopyingDSFillerModule class.
 *
 * Helper scheduler module for CopyingDelaySlotFiller.
 * Creates a CopyingDelaySlotFiller which is the actual filler class.
 *
 * @author Heikki Kultala 2007 (heikki.kultala@tut.fi)
 * @note rating: red
 */
#include "CopyingDSFillerModule.hh"

#include "CopyingDelaySlotFiller.hh"

CopyingDSFillerModule::CopyingDSFillerModule() : filler_(NULL) {
}

CopyingDSFillerModule::~CopyingDSFillerModule() {
    if (filler_ != NULL) {
        delete filler_; filler_ = NULL;
    }
}

CopyingDelaySlotFiller&
CopyingDSFillerModule::filler() {
    if (filler_ == NULL) {
        filler_ = new CopyingDelaySlotFiller();
    }
    return *filler_;
}

/**
 * A short description of the module, usually the module name,
 * in this case "CopyingDSFillerModule".
 *
 * @return The description as a string.
 */   
std::string
CopyingDSFillerModule::shortDescription() const {
    return "Helper: CopyingDSFillerModule";
}

/**
 * Optional longer description of the Module.
 *
 * This description can include usage instructions, details of choice of
 * helper modules, etc.
 *
 * @return The description as a string.
 */
std::string
CopyingDSFillerModule::longDescription() const {
    std::string answer ="Helper: CopyingDSFillerModule.";    
    answer += " Implements delay slot filling algorithm.";
    return answer;
}

SCHEDULER_PASS(CopyingDSFillerModule)
