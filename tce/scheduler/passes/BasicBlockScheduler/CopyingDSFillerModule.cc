/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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
