/**
 * @file ProximConstants.cc
 *
 * Implementation of ProximConstants class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximConstants.hh"

const std::string ProximConstants::COMMAND_NAME_OPEN_PROGRAM = "Open Program";
const std::string ProximConstants::COMMAND_NAME_OPEN_MACHINE = "Open Machine";
const std::string ProximConstants::COMMAND_NAME_CD = "CD";
const std::string ProximConstants::COMMAND_NAME_CLEAR_CONSOLE =
    "Clear Console";
const std::string ProximConstants::COMMAND_NAME_SET_BP = "Set Breakpoint";
const std::string ProximConstants::COMMAND_NAME_QUIT = "Quit";
const std::string ProximConstants::COMMAND_NAME_ABOUT = "About";
const std::string ProximConstants::COMMAND_NAME_RUN = "Run";
const std::string ProximConstants::COMMAND_NAME_STEPI = "StepI";
const std::string ProximConstants::COMMAND_NAME_NEXTI = "NextI";
const std::string ProximConstants::COMMAND_NAME_UNTIL = "Until";
const std::string ProximConstants::COMMAND_NAME_KILL = "Kill";
const std::string ProximConstants::COMMAND_NAME_RESUME = "Resume";
const std::string ProximConstants::COMMAND_NAME_NEW_REGISTER_WINDOW =
    "New Register Window";
const std::string ProximConstants::COMMAND_NAME_NEW_PORT_WINDOW =
    "New Port Window";
const std::string ProximConstants::COMMAND_NAME_SIMULATOR_SETTINGS =
    "Simulator Settings";
const std::string ProximConstants::COMMAND_NAME_EDIT_OPTIONS = "Options";
const std::string ProximConstants::COMMAND_NAME_EXECUTE_FILE = "Execute File";

const std::string ProximConstants::SCL_LOAD_PROGRAM = "prog";
const std::string ProximConstants::SCL_LOAD_MACHINE = "mach";
const std::string ProximConstants::SCL_LOAD_CONF = "conf";
const std::string ProximConstants::SCL_SET_BREAKPOINT = "bp";
const std::string ProximConstants::SCL_SET_TEMP_BP = "tbp";
const std::string ProximConstants::SCL_ADD_WATCH = "watch";
const std::string ProximConstants::SCL_SET_CONDITION = "condition";
const std::string ProximConstants::SCL_SET_IGNORE_COUNT = "ignore";
const std::string ProximConstants::SCL_SET = "setting";
const std::string ProximConstants::SCL_DELIM = ";";
const std::string ProximConstants::SCL_QUIT = "quit";
const std::string ProximConstants::SCL_RUN_UNTIL = "until";
const std::string ProximConstants::SCL_NEXT_INSTRUCTION = "nexti";
const std::string ProximConstants::SCL_STEP_INSTRUCTION = "stepi";
const std::string ProximConstants::SCL_INFO_REGISTERS = "info registers";
const std::string ProximConstants::SCL_INFO_IMMEDIATES = "info immediates";

const std::string ProximConstants::SCL_SETTING_EXEC_TRACE = "execution_trace";
const std::string ProximConstants::SCL_SETTING_BUS_TRACE = "bus_trace";
const std::string ProximConstants::SCL_SETTING_PROCEDURE_TRANSFER_TRACE =
    "procedure_transfer_tracking";
const std::string ProximConstants::SCL_SETTING_RF_TRACE = "rf_tracking";
const std::string ProximConstants::SCL_SETTING_FU_CONFLICT_DETECTION =
    "fu_conflict_detection";

const std::string ProximConstants::SCL_SETTING_HISTORY_SAVE = "history_save";
const std::string ProximConstants::SCL_SETTING_HISTORY_SIZE = "history_size";
const std::string ProximConstants::SCL_SETTING_HISTORY_FILE =
    "history_filename";

const std::string ProximConstants::MACH_WIN_FU_LABEL = "Function unit: ";
const std::string ProximConstants::MACH_WIN_FU_PORT_LABEL =
    "Function Unit Port: ";

const std::string ProximConstants::MACH_WIN_PORT_LABEL = "Port: ";
const std::string ProximConstants::MACH_WIN_SOCKET_LABEL = "Socket: ";
const std::string ProximConstants::MACH_WIN_BUS_LABEL = "Bus: ";
const std::string ProximConstants::MACH_WIN_RF_LABEL = "Register file: ";
const std::string ProximConstants::MACH_WIN_IU_LABEL = "Immediate unit: ";
const std::string ProximConstants::MACH_WIN_GCU_LABEL = "Global Control Unit";
const std::string ProximConstants::MACH_WIN_TRIGGER_COUNT_LABEL =
    "Trigger count: ";
const std::string ProximConstants::MACH_WIN_BUS_VALUE_LABEL = "Value: ";
const std::string ProximConstants::MACH_WIN_BUS_WRITES_LABEL = "Bus writes: ";
const std::string ProximConstants::MACH_WIN_SOCKET_WRITES_LABEL = 
    "Socket writes: ";

const std::string ProximConstants::MACH_WIN_EXECUTION_COUNT_LABEL =
    "Operation executions: ";

const std::string ProximConstants::MACH_WIN_PORTS_TITLE = "Ports:";
const std::string ProximConstants::MACH_WIN_REGISTERS_TITLE = "Registers:";
const std::string ProximConstants::MACH_WIN_IMMEDIATES_TITLE =
    "Immediate registers:";

const wxString ProximConstants::MACH_WIN_DETAILS_TITLE =
    _T("Component details:");
const wxString ProximConstants::MACH_WIN_UTILIZATION_TITLE =
    _T("Component utilization:");


const std::string ProximConstants::CONFIGURATION_NAME = "proxim-configuration";
const std::string ProximConstants::PROXIM_TITLE = "TTA Processor Simulator";
const std::string ProximConstants::PROXIM_VERSION = "?.?";

const wxString ProximConstants::PROXIM_COPYRIGHT = 
    _T("Portions of this program are copyright\n"
       "\n"
       "Xerces (c) 1999-2005 The Apache Software\n" 
       "wxWidgets (c) 1998 Julian Smart, Robert Roebling \n"
       "Tcl (c) 1998-1999 by Scriptics Corporation\n" 
       "Editline (c) 1997 The NetBSD Foundation, Inc.\n"
       "Boost C++ Libraries (c) 2003 Beman Dawes, David Abrahams\n" 
       "\n" 
       "For more information see User manual chapter \"Copyright notices\""
      );
