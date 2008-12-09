/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file CompiledSimSettingCommand.cc
 *
 * Definition of CompiledSimSettingCommand class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "CompiledSimSettingCommand.hh"
#include "MapTools.hh"

/**
 * Setting action that sets the FU resource conflict detection.
 */
class CompiledSimSetFUConflictDetection {
public:

    /**
     * Sets the FU resource conflict detection.
     *
     * @param simFront SimulatorFrontend to set the detection for.
     * @param newValue Value to set.
     * @return True if setting was successful.
     */
    static bool execute(
        SimulatorInterpreter&, SimulatorFrontend& simFront, bool newValue) {
        simFront.setFUResourceConflictDetection(newValue);
        return true;
    }

    /**
     * Returns the default value of this setting.
     *
     * @return The default value.
     */
    static const DataObject& defaultValue() {
        static const DataObject defaultValue_("0");
        return defaultValue_;
    }
    
    /**
     * Should the action warn if program & machine exist and value was changed
     * 
     * @return boolean value on whether or not to warn
     */
    static bool warnOnExistingProgramAndMachine() {
        return false;
    }
};


/**
 * Default constructor
 */
CompiledSimSettingCommand::CompiledSimSettingCommand() :
    SettingCommand() {
    
    // Remove unsupported settings
    MapTools::deleteByKey(settings_, "rf_tracking");
    MapTools::deleteByKey(settings_, "bus_trace");
    MapTools::deleteByKey(settings_, "profile_data_saving");
    MapTools::deleteByKey(settings_, "utilization_data_saving");
    
    // Replace FU conflict detection setting
    MapTools::deleteByKey(settings_, "fu_conflict_detection");
    settings_["fu_conflict_detection"] =
        new TemplatedSimulatorSetting<
            BooleanSetting, CompiledSimSetFUConflictDetection>(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_INTERP_SETTING_FU_CONFLICT_DETECTION).str());
}

/**
 * Default destructor
 */
CompiledSimSettingCommand::~CompiledSimSettingCommand() {
}
