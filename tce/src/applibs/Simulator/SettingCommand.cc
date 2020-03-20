/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file SettingCommand.cc
 *
 * Implementation of SettingCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "SettingCommand.hh"

#include <iostream>
#include <iomanip>

#include "SimulatorToolbox.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreter.hh"
#include "MapTools.hh"
#include "StringTools.hh"
#include "FileSystem.hh"

/**
 * Setting action that sets the execution trace of simulation.
 */
class SetExecutionTrace {
public:

    /**
     * Sets the execution trace of the simulation.
     *
     * @param simFront SimulatorFrontend to set the execution trace for.
     * @param newValue Value to set.
     * @return True if setting was successful.
     */
    static bool execute(
        SimulatorInterpreter&, SimulatorFrontend& simFront, bool newValue) {
        simFront.setExecutionTracing(newValue);
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
        return true;
    }
};

/**
 * Setting action that sets the execution trace of simulation.
 */
class SetBusTrace {
public:

    /**
     * Sets the execution trace of the simulation.
     *
     * @param simFront SimulatorFrontend to set the bus trace for.
     * @param newValue Value to set.
     * @return True if setting was successful.
     */
    static bool execute(
        SimulatorInterpreter&, SimulatorFrontend& simFront, bool newValue) {
        simFront.setBusTracing(newValue);
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
        return true;
    }
};

/**
 * Setting action that sets the register file access tracking of simulation.
 */
class SetRFTracking {
public:

    /**
     * Sets the register file access tracking of simulation.
     *
     * @param simFront SimulatorFrontend to set the tracking for.
     * @param newValue Value to set.
     * @return True if setting was successful.
     */
    static bool execute(
        SimulatorInterpreter&, SimulatorFrontend& simFront, bool newValue) {
        simFront.setRFAccessTracing(newValue);
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
        return true;
    }
};

/**
 * Setting action that sets the profile data saving.
 */
class SetProfileSaving {
public:

    /**
     * Sets the profile data saving.
     *
     * @param simFront SimulatorFrontend to set the profile data saving for.
     * @param newValue Value to set.
     * @return True if setting was successful.
     */
    static bool execute(
        SimulatorInterpreter&, SimulatorFrontend& simFront, bool newValue) {
        simFront.setProfileDataSaving(newValue);
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
 * Setting action that sets the FU resource conflict detection.
 */
class SetFUConflictDetection {
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
        static const DataObject defaultValue_("1");
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
 * Setting action that sets the printing of the next executed instruction 
 * when simulation stops.
 */
class SetNextInstructionPrinting {
public:

    /**
     * Sets the next instruction printing.
     *
     * @param simFront SimulatorFrontend to set the detection for.
     * @param newValue Value to set.
     * @return True if setting was successful.
     */
    static bool execute(
        SimulatorInterpreter&, SimulatorFrontend& simFront, bool newValue) {
        simFront.setNextInstructionPrinting(newValue);
        return true;
    }

    /**
     * Returns the default value of this setting.
     *
     * @return The default value.
     */
    static const DataObject& defaultValue() {
        static const DataObject defaultValue_("1");
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
 * Setting action that sets the simulation time statistics.
 * 
 */
class SetSimulationTimeStatistics {
public:
    /** Sets the time statistics.
     * 
     * @param simFront SimulatorFrontend to set the time statistics for.
     * @param newValue Value to setting
     * @return True if setting was successful.
     */ 
    static bool execute(
        SimulatorInterpreter&, SimulatorFrontend& simFront, bool newValue) {
        simFront.setSimulationTimeStatistics(newValue);
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
 * Setting action that sets the simulation timeout
 * 
 */
class SetSimulationTimeout {
public:
    /** Sets the simulation timeout in seconds
     * 
     * @param simFront SimulatorFrontend to set the timeout for.
     * @param newValue Value to setting
     * @return True if setting was successful.
     */ 
    static bool execute(
        SimulatorInterpreter&, 
        SimulatorFrontend& simFront, 
        unsigned int newValue) {
        simFront.setTimeout(newValue);
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
 * Setting action that sets the static compilation flag
 * 
 */
class SetStaticCompilation {
public:
    /** Sets the static compilation flag
     * 
     * @param simFront SimulatorFrontend to set the flag for.
     * @param newValue Value to setting
     * @return True if setting was successful.
     */ 
    static bool execute(
        SimulatorInterpreter&, 
        SimulatorFrontend& simFront, 
        unsigned int newValue) {
        simFront.setStaticCompilation(newValue);
        return true;
    }
                            
    /**
     * Returns the default value of this setting.
     *
     * @return The default value.
     */
    static const DataObject& defaultValue() {
        static const DataObject defaultValue_("1");
        return defaultValue_;
    }
    
    /**
     * Should the action warn if program & machine exist and value was changed
     * 
     * @return boolean value on whether or not to warn
     */
    static bool warnOnExistingProgramAndMachine() {
        return true;
    }
};

/**
 * Setting action that sets the utilization data saving.
 */
class SetUtilizationSaving {
public:

    /**
     * Sets the utilization data saving.
     *
     * @param simFront SimulatorFrontend to set the utilization data saving 
     *                 for.
     * @param newValue Value to set.
     * @return True if setting was successful.
     */
    static bool execute(
        SimulatorInterpreter&, SimulatorFrontend& simFront, bool newValue) {
        simFront.setUtilizationDataSaving(newValue);
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
 * Setting action that sets the command history file name.
 */
class SetHistoryFilename {
public:

    /**
     * Sets the command history file name.
     *
     * @param interpreter To set the history file name for.
     * @param newValue Value to set.
     * @return True if setting was successful.
     */
    static bool execute(
        SimulatorInterpreter& interpreter,
        SimulatorFrontend&, 
        const std::string& newValue) {
        if (!(FileSystem::fileIsCreatable(newValue) ||
              FileSystem::fileIsWritable(newValue)) || 
            FileSystem::fileIsDirectory(newValue)) {
            interpreter.lineReader()->outputStream() 
                << "Could not open file for writing." << std::endl;
            return false;
        }
        interpreter.lineReader()->setInputHistoryLog(newValue);
        return true;
    }

    /**
     * Returns the default value of this setting.
     *
     * @return The default value.
     */
    static const DataObject& defaultValue() {
        static const DataObject defaultValue_(SIM_DEFAULT_COMMAND_LOG);
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
 * Setting action that sets the command history file name.
 */
class SetHistorySave {
public:

    /**
     * Makes command history to be saved to a file.
     *
     * @param interpreter Used to set the setting.
     * @param newValue Value to set.
     * @return True if setting was successful.
     */
    static bool execute(
        SimulatorInterpreter& interpreter,
        SimulatorFrontend&, 
        bool newValue) {
        interpreter.lineReader()->setSaveInputHistoryToFile(newValue);
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
 * Setting action that sets the command history file name.
 */
class SetHistorySize {
public:

    /**
     * Sets the command history maximum length.
     *
     * @param interpreter To use to set the setting.
     * @param newValue Value to set.
     * @return True if setting was successful.
     */
    static bool execute(
        SimulatorInterpreter& interpreter,
        SimulatorFrontend&, 
        unsigned int newValue) {
        interpreter.lineReader()->setInputHistoryLength(newValue);
        return true;
    }

    /**
     * Returns the default value of this setting.
     *
     * @return The default value.
     */
    static const DataObject& defaultValue() {
        static const DataObject defaultValue_(DEFAULT_INPUT_HISTORY_SIZE);
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
 * Setting action that sets the procedure execution transfer tracing.
 */
class SetProcedureTransferTracing {
public:

    /**
     * Sets the procedure execution transfer tracing.
     *
     * @param interpreter To use to set the setting.
     * @param newValue Value to set.
     * @return True if setting was successful.
     */
    static bool execute(
        SimulatorInterpreter&, SimulatorFrontend& simFront, bool newValue) {
        simFront.setProcedureTransferTracing(newValue);
        return true;
    }

    /**
     * Returns the default value of this setting.
     *
     * @return The default value.
     */
    static const DataObject& defaultValue() {
        static DataObject defaultValue_("0");
        return defaultValue_;
    }
    
    /**
     * Should the action warn if program & machine exist and value was changed
     * 
     * @return boolean value on whether or not to warn
     */
    static bool warnOnExistingProgramAndMachine() {
        return true;
    }
};

/**
 * Setting action that sets the memory access tracking.
 */
class SetMemoryAccessTracking {
public:

    /**
     * Sets the memory access tracking.
     *
     * @param interpreter To use to set the setting.
     * @param newValue Value to set.
     * @return True if setting was successful.
     */
    static bool execute(
        SimulatorInterpreter&, SimulatorFrontend& simFront, bool newValue) {
        simFront.setMemoryAccessTracking(newValue);
        return true;
    }

    /**
     * Returns the default value of this setting.
     *
     * @return The default value.
     */
    static const DataObject& defaultValue() {
        static DataObject defaultValue_("0");
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
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
SettingCommand::SettingCommand() : 
    SimControlLanguageCommand("setting") {

    settings_["execution_trace"] =
        new TemplatedSimulatorSetting<BooleanSetting, SetExecutionTrace>(
            SimulatorToolbox::textGenerator().text(
                Texts::TXT_INTERP_SETTING_EXECTRACE).str());

    settings_["bus_trace"] =
        new TemplatedSimulatorSetting<BooleanSetting, SetBusTrace>(
            SimulatorToolbox::textGenerator().text(
                Texts::TXT_INTERP_SETTING_BUSTRACE).str());

    settings_["rf_tracking"] =
        new TemplatedSimulatorSetting<BooleanSetting, SetRFTracking>(
            SimulatorToolbox::textGenerator().text(
                Texts::TXT_INTERP_SETTING_RFTRACKING).str());

    settings_["history_filename"] =
        new TemplatedSimulatorSetting<StringSetting, SetHistoryFilename>(
            SimulatorToolbox::textGenerator().text(
                Texts::TXT_INTERP_SETTING_HISTORY_FILENAME).str());

    settings_["history_save"] =
        new TemplatedSimulatorSetting<BooleanSetting, SetHistorySave>(
            SimulatorToolbox::textGenerator().text(
                Texts::TXT_INTERP_SETTING_HISTORY_SAVE).str());

    settings_["history_size"] =
        new TemplatedSimulatorSetting<PositiveIntegerSetting, SetHistorySize>(
            SimulatorToolbox::textGenerator().text(
                Texts::TXT_INTERP_SETTING_HISTORY_SIZE).str());

    settings_["procedure_transfer_tracking"] =
        new TemplatedSimulatorSetting<
            BooleanSetting, SetProcedureTransferTracing>(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_INTERP_SETTING_PROCEDURE_TRANSFER_TRACKING).
                str());

    settings_["memory_access_tracking"] =
        new TemplatedSimulatorSetting<
            BooleanSetting, SetMemoryAccessTracking>(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_INTERP_SETTING_MEMORY_ACCESS_TRACKING).
                str());
    
    settings_["utilization_data_saving"] =
        new TemplatedSimulatorSetting<
            BooleanSetting, SetUtilizationSaving>(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_INTERP_SETTING_UTILIZATION_SAVING).
                str());

    settings_["profile_data_saving"] =
        new TemplatedSimulatorSetting<
            BooleanSetting, SetProfileSaving>(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_INTERP_SETTING_PROFILE_SAVING).
                str());

    settings_["fu_conflict_detection"] =
        new TemplatedSimulatorSetting<
            BooleanSetting, SetFUConflictDetection>(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_INTERP_SETTING_FU_CONFLICT_DETECTION).
                str());

    settings_["next_instruction_printing"] =
        new TemplatedSimulatorSetting<
            BooleanSetting, SetNextInstructionPrinting>(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_INTERP_SETTING_NEXT_INSTRUCTION_PRINTING).
                str());
                
    settings_["simulation_time_statistics"] =
        new TemplatedSimulatorSetting<
            BooleanSetting, SetSimulationTimeStatistics>(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_SIMULATION_TIME_STATISTICS).str());
                
    settings_["simulation_timeout"] =
        new TemplatedSimulatorSetting<
            PositiveIntegerSetting, SetSimulationTimeout>(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_SIMULATION_TIMEOUT).str());
                
    settings_["static_compilation"] =
        new TemplatedSimulatorSetting<
            BooleanSetting, SetStaticCompilation>(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_STATIC_COMPILATION).str());            
}

/**
 * Destructor.
 *
 * Does nothing.
 */
SettingCommand::~SettingCommand() {
    MapTools::deleteAllValues(settings_);
}

/**
 * Executes the "setting" command.
 *
 * Command that allows changing and listing simulator's settings.
 * If no arguments are given, all simulator settings are listed in
 * predefined oreder. If one argument is given, it's expected to be
 * a simulator setting, of which current value is printed. If two
 * arguments are given, the first argument tells the name of the setting
 * to modify, and second argument the new value.
 *
 * @param arguments Read the comment.
 * @return True in case arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 * @todo Use the count for the step.
 */
bool
SettingCommand::execute(const std::vector<DataObject>& arguments) {
    const int argumentCount = arguments.size() -1;

    if (!checkArgumentCount(argumentCount, 0, 2)) {
        return false;
    } 
    
    // no arguments given, list all the settings and their values, 
    // value types and explanations
    if (argumentCount == 0) {
        SettingMap::iterator i = settings_.begin();
     
        while (i != settings_.end()) {
            outputStream().flags(std::ios_base::left);
            outputStream() 
                << std::setw(30) << (*i).first 
                << std::setw(20) << (*i).second->valueAsString() << " "
                << (*i).second->description() << std::endl;
            ++i;
        }
        return true;
    }

    SettingMap::iterator settingPosition = 
        settings_.find(
            StringTools::stringToLower(arguments.at(1).stringValue()));
    if (settingPosition == settings_.end()) {
        interpreter()->setError(
            SimulatorToolbox::textGenerator().text(
                Texts::TXT_NO_SUCH_SETTING).str());
        return false;
    }

    SimulatorSetting& setting = *((*settingPosition).second);
                       
    // two arguments given, modify the setting     
    if (arguments.size() - 1 == 2) {
        try {
            if (!setting.setValue(
                    *dynamic_cast<SimulatorInterpreter*>(interpreter()), 
                    simulatorFrontend(), arguments.at(2))) {
                return false;
            }
        } catch (const Exception& e) {
            interpreter()->setError(e.errorMessage());
            return false;
        }
    }

    DataObject* result = new DataObject();
    result->setString(setting.valueAsString());
    interpreter()->setResult(result);
    return true;
}

/**
 * Returns the help text for this command.
 * 
 * Help text is searched from SimulatorTextGenerator.
 *
 * @return The help text.
 * @todo Use SimulatorTextGenerator to get the help text.
 */
std::string 
SettingCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_SETTING).str();
}
