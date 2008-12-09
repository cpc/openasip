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
 * @file SimulatorTextGenerator.hh
 *
 * Declaration of the SimulatorTextGenerator class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-.cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIM_TEXT_GENERATOR_HH
#define TTA_SIM_TEXT_GENERATOR_HH

#include "TextGenerator.hh"

/**
 * Provides common texts for simulator user interfaces.
 */
class SimulatorTextGenerator : public Texts::TextGenerator {
public:
    SimulatorTextGenerator();
    virtual ~SimulatorTextGenerator();
    virtual void generateCompiledSimTexts();
};

namespace Texts {
    /// Ids for the generated texts.
    enum {        
        TXT_NO_ADF_DEFINED_IN_PCF = Texts::LAST__,
        TXT_ILLEGAL_ADF_FILE,
        TXT_REGISTER_NOT_FOUND,
        TXT_REGISTER_FILE_NOT_FOUND,
        TXT_IMMEDIATE_UNIT_NOT_FOUND,
        TXT_FU_NOT_FOUND,
        TXT_BUS_NOT_FOUND,
        TXT_FUPORT_NOT_FOUND,
        TXT_INTERP_HELP_PROG, 
        ///< Help text for command "prog" of the CLI.
        TXT_INTERP_HELP_MACH, 
        ///< Help text for command "mach" of the CLI.
        TXT_INTERP_HELP_CONF, 
        ///< Help text for command "conf" of the CLI.
        TXT_INTERP_HELP_QUIT, 
        ///< Help text for command "quit" of the CLI.
        TXT_INTERP_HELP_DISASSEMBLE, 
        ///< Help text for command "disassemble" of the CLI.
        TXT_INTERP_HELP_RUN,
        ///< Help text for command "run" of the CLI.
        TXT_INTERP_HELP_STEPI,
        ///< Help text for command "stepi" of the CLI.
        TXT_INTERP_HELP_COMMANDS,
        ///< Help text for command "commands" of the CLI.
        TXT_INTERP_HELP_UNTIL,
        ///< Help text for command "stepi" of the CLI.
        TXT_INTERP_HELP_SETTING,
        ///< Help text for command "setting" of the CLI.
        TXT_INTERP_HELP_RESUME,
        ///< Help text for command "resume" of the CLI.
        TXT_INTERP_HELP_HELP,
        ///< Help text for command "help" of the CLI.
        TXT_INTERP_HELP_INFO,
        ///< Help text for command "info" of the CLI.
        TXT_INTERP_HELP_BREAK,
        ///< Help text for command "bp" of the CLI.
        TXT_INTERP_HELP_WATCH,
        ///< Help text for command "watch" of the CLI.
        TXT_INTERP_HELP_TBREAK,
        ///< Help text for command "tbp" of the CLI.
        TXT_INTERP_HELP_CONDITION,
        ///< Help text for command "condition" of the CLI.
        TXT_INTERP_HELP_IGNORE,
        ///< Help text for command "ignore" of the CLI.
        TXT_INTERP_HELP_DELETEBP,
        ///< Help text for command "deletebp" of the CLI.
        TXT_INTERP_HELP_ENABLEBP,
        ///< Help text for command "enablebp" of the CLI.
        TXT_INTERP_HELP_DISABLEBP,
        ///< Help text for command "disablebp" of the CLI.
        TXT_INTERP_HELP_NEXTI,
        ///< Help text for command "nexti" of the CLI.
        TXT_INTERP_HELP_KILL,
        ///< Help text for command "kill" of the CLI.
        TXT_INTERP_HELP_X,
        ///< Help text for command "x" of the CLI.
        TXT_CLI_ONLINE_HELP, 
        ///< Online help text.
        TXT_CMD_LINE_HELP,
        TXT_ILLEGAL_PROGRAM_IU_STATE_NOT_FOUND,
        ///< Text of the exception thrown when Immediate Unit state could not
        /// be found and there is instruction referring to it.
        TXT_ILLEGAL_PROGRAM_UNKNOWN_MOVE_TERMINAL_TYPE,
        ///< Text of the exception thrown when the terminal type of a move is
        /// not regocnized.
        TXT_ILLEGAL_PROGRAM_RF_STATE_NOT_FOUND,
        ///< Text of the exception thrown when RegisterFileState could not
        /// be found and there is instruction referring to it.
        TXT_ILLEGAL_PROGRAM_PORT_STATE_NOT_FOUND,
        ///< Text of the exception thrown when FUPortState could not
        /// be found and there is instruction referring to it.
        TXT_ILLEGAL_PROGRAM_BUS_STATE_NOT_FOUND,
        ///< Text of the exception thrown when FUPortState could not
        /// be found and there is instruction referring to it.
        TXT_ILLEGAL_PROGRAM_OPCODE_NOT_FOUND,
        ///< Text of the exception thrown when a port to be written is an
        /// opcode setting port and no opcode is found in the move.
        TXT_OPERATION_X_NOT_FOUND,
        TXT_INTERP_SIMULATION_NOT_INITIALIZED,
        ///< Text to be printed when simulation is not initialized and
        ///  it should be.
        TXT_INTERP_SIMULATION_ALREDY_RUNNING,
        ///< Text to be printed when simulation is already running and
        ///  it should not be.
        TXT_INTERP_SIMULATION_NOT_RUNNING,
        ///< Text to be printed when simulation is not running and
        ///  it should be.
        TXT_INTERP_SIMULATION_NOT_ENDED,
        ///< Text to be printed when simulation has not ended and
        ///  it should be.
        TXT_INTERP_SETTING_PARSE_ERROR,
        ///< In case user tried to set a setting with illegal parameter.
        TXT_INTERP_SETTING_EXECTRACE,
        TXT_INTERP_SETTING_BUSTRACE,
        TXT_INTERP_SETTING_RFTRACKING,
        TXT_INTERP_SETTING_HISTORY_FILENAME,
        TXT_INTERP_SETTING_NEXT_INSTRUCTION_PRINTING,
        TXT_INTERP_SETTING_HISTORY_SAVE,
        TXT_INTERP_SETTING_FU_CONFLICT_DETECTION,
        TXT_INTERP_SETTING_HISTORY_SIZE,
        ///< Description of the execution trace setting.
        TXT_SIMULATION_TIME_STATISTICS,
        ///< Description of the simulation time statistics setting
        TXT_SIMULATION_TIMEOUT,
        ///< Simulation timeout in seconds
        TXT_STATIC_COMPILATION,
        ///< Use static compilation when using compiled simulator
        TXT_INTERP_HELP_COMMANDS_AVAILABLE,
        ///< Description of the execution trace setting.
        TXT_STATUS_STOPPED,
        ///< Status string of stopped simulation.
        TXT_STATUS_FINISHED,
        ///< Status string of a finished simulation.
        TXT_STATUS_INITIALIZED,
        ///< Status string of initialized simulation (program loaded).
        TXT_STATUS_NOT_INITIALIZED,
        ///< Status string of non-initialized simulation (program not loaded).
        TXT_STOPREASON_BREAKPOINT,
        ///< Stop reason: breakpoint.
        TXT_STOPREASON_DELETED_BREAKPOINT,
        ///< Stop reason: temporary breakpoint.
        TXT_STOPREASON_UNTIL,
        ///< Stop reason: until.
        TXT_STOPREASON_TIMEOUT,
        ///< Stop reason: timeout                
        TXT_STOPREASON_STEPPING,
        ///< Stop reason: stepping. 
        TXT_STOPREASON_USERREQUEST,
        ///< Stop reason: user requested.
        TXT_STOPREASON_RUNTIME_ERROR,
        ///< Stop reason: user requested.
        TXT_LABEL_NOT_FOUND,
        TXT_ILLEGAL_ADDRESS_EXPRESSION,
        TXT_BREAKPOINT_NOT_FOUND,
        TXT_INTERP_ENTER_CONDITION_PROMPT,
        TXT_INTERP_ENTER_EXPRESSION_PROMPT,
        TXT_EXPRESSION_EMPTY,
        TXT_CONFIRM_DELETE_ALL_BREAKPOINTS,
        TXT_CONFIRM_KILL_SIMULATION,
        TXT_ADDRESS_OUT_OF_RANGE,
        TXT_ADDRESS_SPACE_NOT_FOUND,
        TXT_NOT_RUNNING_PARALLEL_SIMULATION,
        TXT_UNABLE_TO_LOAD_PROGRAM,
        TXT_UNABLE_TO_LOAD_MACHINE,
        TXT_NO_ADDRESS_SPACE_GIVEN,
        TXT_INTERP_SETTING_PROCEDURE_TRANSFER_TRACKING,
        TXT_INTERP_SETTING_MEMORY_ACCESS_TRACKING,
        TXT_INTERP_SETTING_UTILIZATION_SAVING,
        TXT_INTERP_SETTING_PROFILE_SAVING,
        TXT_NO_PROGRAM_LOADED,
        TXT_AUTOMATIC_FINISH_IMPOSSIBLE,
        TXT_STARTUP_SETTINGS_CHANGED_WARNING
    };

}

#endif
