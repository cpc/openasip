/**
 * @file CompiledSimSettingCommand.cc
 *
 * Definition of CompiledSimSettingCommand class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
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
