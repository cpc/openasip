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
 * @file ProximConstants.hh
 *
 * Declaration of ProximConstants class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef PROXIM_CONSTANTS_HH
#define PROXIM_CONSTANTS_HH

#include <wx/string.h>
#include <string>

// The wxWidgets reserved IDs are in the range wxID_LOWEST..wxID_HIGHEST
// For reference on reserved IDs, see 
// http://www.wxwidgets.org/manuals/2.6/wx_stdevtid.html#stdevtid

/**
 * Collection of Proxim constants shared by all classes of the application.
 */
class ProximConstants {
public:
    /// Menu command IDs.
    enum CommandID {
        // Command IDs for commands to be handled with CommandRegistry.
	COMMAND_FIRST = 15000, // Keep this as the first command id.
	COMMAND_OPEN_PROGRAM,
	COMMAND_OPEN_MACHINE,
	COMMAND_CD,
	COMMAND_CLEAR_CONSOLE,
	COMMAND_SET_BP,
	COMMAND_QUIT,
	COMMAND_ABOUT,

	COMMAND_RUN,
	COMMAND_STEPI,
	COMMAND_NEXTI,
	COMMAND_UNTIL,
	COMMAND_KILL,
	COMMAND_RESUME,
	COMMAND_NEW_REGISTER_WINDOW,
	COMMAND_NEW_PORT_WINDOW,
        COMMAND_SIMULATOR_SETTINGS,
        COMMAND_EDIT_OPTIONS,
        COMMAND_EXECUTE_FILE,
        COMMAND_HIGHLIGHT_TOP_EXEC_COUNTS,
        COMMAND_HIGHLIGHT_EXEC_PERCENTAGE,
        COMMAND_CLEAR_DISASSEMBLY_HIGHLIGHT,
	COMMAND_LAST, // Keep this as the last command id.

	// Toggle command ids
	COMMAND_TOGGLE_CONSOLE_WIN,
	COMMAND_TOGGLE_DISASM_WIN,
	COMMAND_TOGGLE_MEMORY_WIN,
	COMMAND_TOGGLE_CONTROL_WIN,
	COMMAND_TOGGLE_MACHINE_WIN,
        COMMAND_TOGGLE_CMD_HISTORY_WIN,
        COMMAND_TOGGLE_BREAKPOINT_WIN
    };

    /// IDs for single instance simulator windows.
    enum simulatorWindowID {
        ID_CONSOLE_WINDOW = 16000,
        ID_DISASSEMBLY_WINDOW,
        ID_MACHINE_STATE_WINDOW,
        ID_MEMORY_WINDOW,
        ID_BREAKPOINT_WINDOW,
        ID_CMD_HISTORY_WINDOW,
        ID_CONTROL_WINDOW
    };

    // Command names.
    // ---
    /// Name of the open program command.
    static const std::string COMMAND_NAME_OPEN_PROGRAM;
    /// Name of the open machine command.
    static const std::string COMMAND_NAME_OPEN_MACHINE;
    /// Name of the change directory command.
    static const std::string COMMAND_NAME_CD;
    /// Name of the clear console command.
    static const std::string COMMAND_NAME_CLEAR_CONSOLE;
    /// Name of the set breakpoint command.
    static const std::string COMMAND_NAME_SET_BP;
    /// Name of the quit command.
    static const std::string COMMAND_NAME_QUIT;
    /// Name of the about command.
    static const std::string COMMAND_NAME_ABOUT;
    /// Name of the run command.
    static const std::string COMMAND_NAME_RUN;
    /// Name of the step instruction command.
    static const std::string COMMAND_NAME_STEPI;
    /// Name of the next instruction command.
    static const std::string COMMAND_NAME_NEXTI;
    /// Name of the until command.
    static const std::string COMMAND_NAME_UNTIL;
    /// Name of the kill command.
    static const std::string COMMAND_NAME_KILL;
    /// Name of the resume command.
    static const std::string COMMAND_NAME_RESUME;
    /// Name of the new register window command.
    static const std::string COMMAND_NAME_NEW_REGISTER_WINDOW;
    /// Name of the new port window command.
    static const std::string COMMAND_NAME_NEW_PORT_WINDOW;
    /// Name of the simulator settings command.
    static const std::string COMMAND_NAME_SIMULATOR_SETTINGS;
    /// Name of the edit options command.
    static const std::string COMMAND_NAME_EDIT_OPTIONS;
    /// Name of the execute file command.
    static const std::string COMMAND_NAME_EXECUTE_FILE;

    // Simulator Control Language commands.
    // ---
    /// Command for loading a new program in the simulator.
    static const std::string SCL_LOAD_PROGRAM;
    /// Command for loading a new  machine in the simulator.
    static const std::string SCL_LOAD_MACHINE;
    /// Command for loading a new machine configuration file in the simulator.
    static const std::string SCL_LOAD_CONF;
    /// Command for setting breakpoints in the simulator control language.
    static const std::string SCL_SET_BREAKPOINT;
    /// Command for setting temporary breakpointss in the sim.con.language.
    static const std::string SCL_SET_TEMP_BP;
    /// Command for adding watches in the simulator control language.
    static const std::string SCL_ADD_WATCH;
    /// Command for setting stop point condition in simulator control language.
    static const std::string SCL_SET_CONDITION;
    /// Command for setting stop point ignore count.
    static const std::string SCL_SET_IGNORE_COUNT; 
    /// Command for quiting the simulation.
    static const std::string SCL_QUIT;
    /// Command for running until specified instruciton is encoutnered.
    static const std::string SCL_RUN_UNTIL;
    /// Command for stepping an instruction.
    static const std::string SCL_STEP_INSTRUCTION;
    /// Command for advancing simulation to the next instruction in the prog.
    static const std::string SCL_NEXT_INSTRUCTION;
    /// Command for displaying register file info.
    static const std::string SCL_INFO_REGISTERS;
    /// Command for displaying immediate unit info.
    static const std::string SCL_INFO_IMMEDIATES;

    /// Command for configuring simulator settings.
    static const std::string SCL_SET;

    /// Name of the execution trace setting.
    static const std::string SCL_SETTING_EXEC_TRACE;
    /// Name of the bus trace setting.
    static const std::string SCL_SETTING_BUS_TRACE;
    /// Name of the procedure transfer trace setting.
    static const std::string SCL_SETTING_PROCEDURE_TRANSFER_TRACE;
    /// Name of the rf access trace setting.
    static const std::string SCL_SETTING_RF_TRACE;
    /// Name of the fu conflict detection setting.
    static const std::string SCL_SETTING_FU_CONFLICT_DETECTION;
    /// Name of the profile data saving setting.
    static const std::string SCL_SETTING_PROFILE_DATA_SAVING;
    /// Name of the utilization data saving setting.
    static const std::string SCL_SETTING_UTILIZATION_DATA_SAVING;
    /// Name of the fu next insturction printing setting.
    static const std::string SCL_SETTING_NEXT_INSTRUCTION_PRINTING;
    /// Name of the simulation time statistics setting.
    static const std::string SCL_SETTING_SIMULATION_TIME_STATISTICS;
    /// Name of the input history saving setting,
    static const std::string SCL_SETTING_HISTORY_SAVE;
    /// Name of the input history size setting.
    static const std::string SCL_SETTING_HISTORY_SIZE;
    /// Name of the input history file name setting.
    static const std::string SCL_SETTING_HISTORY_FILE;

    /// Command delimeter for the simulator commands.
    static const std::string SCL_DELIM;

    // Machine window labels.
    /// Label precing function unit names in the machine state window.
    static const std::string MACH_WIN_FU_LABEL;
    /// Label precing function unit port names in the machine state window.
    static const std::string MACH_WIN_FU_PORT_LABEL;
    /// Label precing port names in the machine state window.
    static const std::string MACH_WIN_PORT_LABEL;
    /// Label precing socket names in the machine state window.
    static const std::string MACH_WIN_SOCKET_LABEL;
    /// Label precing register file names in the machine state window.
    static const std::string MACH_WIN_RF_LABEL;
    /// Label precing bus names in the machine state window.
    static const std::string MACH_WIN_BUS_LABEL;
    /// Label precing immediate unit names in the machine state window.
    static const std::string MACH_WIN_IU_LABEL;
    /// Label for the global control unit in the machine state window.
    static const std::string MACH_WIN_GCU_LABEL;
    /// Label for the machine state window bus value.
    static const std::string MACH_WIN_BUS_VALUE_LABEL;
    ///  Label for the machine state window bus write count..
    static const std::string MACH_WIN_BUS_WRITES_LABEL;
    ///  Label for the machine state window socket write count.
    static const std::string MACH_WIN_SOCKET_WRITES_LABEL;

    /// Execution count title for the machine state window.
    static const std::string MACH_WIN_EXECUTION_COUNT_LABEL;

    /// Trigger count label for the machine state window.
    static const std::string MACH_WIN_TRIGGER_COUNT_LABEL;

    /// Title for the machine state window ports list.
    static const std::string MACH_WIN_PORTS_TITLE;
    /// Title for the machine state window register file register list.
    static const std::string MACH_WIN_REGISTERS_TITLE;
    /// Title for the machine state window immediate unit immediates list.
    static const std::string MACH_WIN_IMMEDIATES_TITLE;

    /// Execution count title for the machine state window.
    static const wxString MACH_WIN_DETAILS_TITLE;
    /// Execution count title for the machine state window.
    static const wxString MACH_WIN_UTILIZATION_TITLE;

    /// Configuration file top-level element name.
    static const std::string CONFIGURATION_NAME;

    /// Application title.
    static const std::string PROXIM_TITLE;
    /// Version string.
    static const std::string PROXIM_VERSION;    
    /// Copyright string
    static const wxString PROXIM_COPYRIGHT;
};

#endif
