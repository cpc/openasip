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

SCHEDULER_PASS(CopyingDSFillerModule)
