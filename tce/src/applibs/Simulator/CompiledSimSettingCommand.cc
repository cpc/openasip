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
