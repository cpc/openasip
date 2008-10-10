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
 * @file ConflictDetectionCodeGenerator.cc
 *
 * Definition of ConflictDetectionCodeGenerator class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "ConflictDetectionCodeGenerator.hh"
#include "CompiledSimSymbolGenerator.hh"
#include "Application.hh"
#include "Machine.hh"
#include "FunctionUnit.hh"
#include "Operation.hh"
#include "Machine.hh"
#include "HWOperation.hh"

#include <string>
#include <sstream>

using std::string;
using std::endl;
using namespace TTAMachine;

/**
 * The constructor.
 * 
 * Gets the conflict detection settings and initializes the generator
 * @param machine The machine model
 * 
 */
ConflictDetectionCodeGenerator::ConflictDetectionCodeGenerator(
    const TTAMachine::Machine& machine,
    const CompiledSimSymbolGenerator& symbolGenerator, 
    bool conflictDetectionEnabled) :
    machine_(machine), conflictDetectionEnabled_(conflictDetectionEnabled),
    symbolGen_(symbolGenerator) {
    
    if (!conflictDetectionEnabled) {
        return;
    }
    
    string conflictDetectionSetting;
    
    const char* SETTING = getenv("TTASIM_CONFLICT_DETECTOR");
    if (SETTING == NULL) {
        conflictDetectionSetting = "AFSA"; // by default
    } else {
        conflictDetectionSetting = SETTING;
    }

    if (conflictDetectionSetting == "CRT") {
        conflictDetectorType_ = "ReservationTableFUResourceConflictDetector";
        conflictDetectorMethod_ = "issueOperationInline";
        conflictDetectorAdvanceCycle_ = "advanceCycle";
        conflictDetectorExtraInitMethod_ = "";
    } else if (conflictDetectionSetting == "AFSA") {
        conflictDetectorType_ = "FSAFUResourceConflictDetector";
        conflictDetectorMethod_ = "issueOperationInline";
        conflictDetectorAdvanceCycle_ = "advanceCycle";
        conflictDetectorExtraInitMethod_ = "initializeAllStates";
    } else if (conflictDetectionSetting == "LFSA") {
        conflictDetectorType_ = "FSAFUResourceConflictDetector";
        conflictDetectorMethod_ = "issueOperationLazyInline";
        conflictDetectorAdvanceCycle_ = "advanceCycle";
        conflictDetectorExtraInitMethod_ = "";
    } else if (conflictDetectionSetting == "DCM") {
        conflictDetectorType_ = "DCMFUResourceConflictDetector";
        conflictDetectorMethod_ = "issueOperationInline";
        conflictDetectorAdvanceCycle_ = "advanceCycle";
        conflictDetectorExtraInitMethod_ = "";
    } else {
        Application::logStream()
            << "Unknown CONFLICT_DETECTION_SETTING: '"
            << conflictDetectionSetting << "'" << endl;
        conflictDetectionEnabled_ = false;
        return;
    }

    // initialize the detectors
    Application::logStream() 
        << "Using conflict detector " << conflictDetectorType_ << std::endl;
    
    const Machine::FunctionUnitNavigator& fus = 
        machine_.functionUnitNavigator();
    for (int i = 0; i < fus.count(); ++i) {
        const FunctionUnit& fu = *fus.item(i);
        if (!fu.needsConflictDetection()) { // no need for conflict detection?
            continue;
        } else {
            conflictDetectors_[fu.name()] =
                symbolGen_.conflictDetectorSymbol(fu);
        }
    }
    
    // No conflict detection is needed
    if (conflictDetectors_.empty()) {
        conflictDetectionEnabled_ = false;
    }
}

/**
 * The destructor
 */
ConflictDetectionCodeGenerator::~ConflictDetectionCodeGenerator() {
}

/**
 * Generates code for including the conflict detectors
 * @return The generated code for including the conflict detectors
 */
string
ConflictDetectionCodeGenerator::includes() {
    
    if (!conflictDetectionEnabled_)
        return "";
    
    std::stringstream ss;
    ss << "#include \"FSAFUResourceConflictDetector.hh\"" << endl << endl;
    return ss.str();
}

/**
 * Creates symbol declaration code for the given FU's conflict detectors
 * 
 * @param fu FU to create the conflict detector for
 * @return The generated symbol declaration code
 */
string 
ConflictDetectionCodeGenerator::symbolDeclaration(const FunctionUnit& fu) {
    
    if (!conflictDetectionEnabled_ || !fu.needsConflictDetection())
        return "";

    std::stringstream ss;
    ss << "\t" << conflictDetectorType_ << " " 
       << symbolGen_.conflictDetectorSymbol(fu) << ";" << endl;

    return ss.str();
}

/**
 * Generates exra initialization call for conflict detectors that require a one
 * 
 * @return The generated code for extra initialization call
 */
string 
ConflictDetectionCodeGenerator::extraInitialization() {
    std::stringstream ss;
    if (conflictDetectionEnabled_ && conflictDetectorExtraInitMethod_ != "") {
        for (ConflictDetectorObjectNameMap::const_iterator i = 
            conflictDetectors_.begin(); i != conflictDetectors_.end(); ++i) {
                ss << "\t" << i->second << "." 
                   << conflictDetectorExtraInitMethod_ << "();" << std::endl;
        }
    }
    return ss.str();
}

/**
 * Generates code for notifying of detected conflicts
 * 
 * @return Generated code for notifying of detected conflicts
 */
string
ConflictDetectionCodeGenerator::notifyOfConflicts() {
    
    if (!conflictDetectionEnabled_)
        return "";
    
    std::stringstream ss;
    ss << endl 
       << "\tif (conflictDetected_) {" << endl
       << "\thaltSimulation(__FILE__, __LINE__, __FUNCTION__,"
       << "\"Conflict detected!\");" << endl
       << "\t}" << endl;
    
    return ss.str();
}

/**
 * Generates code for notifying of detected conflicts
 * 
 * @return Generated code for notifying of detected conflicts
 */
string
ConflictDetectionCodeGenerator::updateSymbolDeclarations() {
    if (!conflictDetectionEnabled_)
        return "";
    
    std::stringstream ss;

    for (ConflictDetectorObjectNameMap::const_iterator i = 
        conflictDetectors_.begin(); i != conflictDetectors_.end(); ++i) {
    ss << "\t," << i->second
       << "(functionUnit(\"" << i->first << "\"))"
       << endl;
    }
    return ss.str();
}

/**
 * Generates code for a function that advances clocks of each conflict detector
 * 
 * @return A std::string containing generated code for the clock advances
 */
string
ConflictDetectionCodeGenerator::advanceClockCode() {
    
    if (!conflictDetectionEnabled_)
        return "";

    std::stringstream ss;
    // advance clock of all conflict detection models
    for (ConflictDetectorObjectNameMap::const_iterator i = 
        conflictDetectors_.begin(); i != conflictDetectors_.end(); ++i) {
        ss << "\t" << (*i).second << "." 
           << conflictDetectorAdvanceCycle_ << "();" << endl;
    }
    
    return ss.str();
}

/**
 * Generates code for detecting an FU pipeline resource conflicts.
 * 
 * @param op the triggered operation
 * @return A std::string containing generated code for the operation call
 */
string
ConflictDetectionCodeGenerator::detectConflicts(
    const TTAMachine::HWOperation& op) {

    if (!conflictDetectionEnabled_)
        return "";

    const TTAMachine::FunctionUnit& fu = *op.parentUnit();

    if (conflictDetectors_.find(fu.name()) == conflictDetectors_.end()) 
        return ""; // no conflict detection for the FU of the given operation

    int operationID = -1;
    for (int i = 0; i < fu.operationCount(); ++i) {
        if (fu.operation(i)->name() == op.name()) {
            operationID = i;
            break;
        }
    }
    std::stringstream ss;
    std::string detector = conflictDetectors_[fu.name()];
    ss << "engine.conflictDetected_ |= !engine." << detector << "." 
       << conflictDetectorMethod_ << "(" << operationID << ");" << endl;
    return ss.str();
}

/**
 * Returns a boolean describing if the conflict detection is enabled or not
 * 
 * @return True if the conflict detection is enabled
 */
bool 
ConflictDetectionCodeGenerator::conflictDetectionEnabled() const {
    return conflictDetectionEnabled_;
}

