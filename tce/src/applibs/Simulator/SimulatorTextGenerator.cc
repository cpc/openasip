/**
 * @file SimulatorTextGenerator.cc
 *
 * Implementation of the SimulatorTextGenerator class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "SimulatorTextGenerator.hh"

/**
 * The Constructor.
 *
 * Initializes string templates to the text generator.
 */
SimulatorTextGenerator::SimulatorTextGenerator() {
    addText(
        Texts::TXT_NO_ADF_DEFINED_IN_PCF, 
        "No ADF defined in the processor configuration file.");
    addText(Texts::TXT_ILLEGAL_ADF_FILE, "Illegal machine file.");
    addText(Texts::TXT_REGISTER_NOT_FOUND, "Register not found.");
    addText(Texts::TXT_REGISTER_FILE_NOT_FOUND, "Register file not found.");
    addText(Texts::TXT_IMMEDIATE_UNIT_NOT_FOUND, "Immediate unit not found.");
    addText(Texts::TXT_FU_NOT_FOUND, "Function unit not found.");
    addText(Texts::TXT_FUPORT_NOT_FOUND, "Function unit port not found.");
    addText(Texts::TXT_BUS_NOT_FOUND, "Bus not found.");
    addText(
        Texts::TXT_INTERP_HELP_PROG, 
        "Loads the simulated program. TPEF file name is given as " 
        "argument.");
    addText(
        Texts::TXT_INTERP_HELP_MACH, 
        "Loads the simulated processor. ADF file name is given as " 
        "argument.");
    addText(
        Texts::TXT_INTERP_HELP_CONF, 
        "Loads the simulated processor using a processor configuration " 
        "file. PCF file name is given as argument.");
    addText(Texts::TXT_INTERP_HELP_QUIT, "Quits the simulator immediately.");
    addText(
        Texts::TXT_INTERP_HELP_DISASSEMBLE,
        "Prints a range of instructions of the loaded program.\n\n"
        "When two arguments are given, the first argument specifies "
        "the first address of the range to display, and the second "
        "argument specifies the last address (not displayed).\n\n"
        "If only one argument, is given, then the function that "
        "contains the given address is disassembled. The argument can also be "
        "a procedure name. If no argument is "
        "given, the default memory range is the function surrounding "
        "the program counter of the selected frame.");

    addText(
        Texts::TXT_INTERP_HELP_RUN,
        "Starts simulation of the program currently loaded into the "
        "simulator.\n\nThe program can be loaded by 'prog' command.\n\n" 
        "Simulation runs until either a breakpoint is encountered or "
        "after the last instruction of the first executed procedure is "
        "executed.");

    addText(
        Texts::TXT_INTERP_HELP_STEPI,
        "Advances simulation to the next machine instruction, stepping "
        "into the first instruction of a new procedure if the instruction "
        "is a function call.\n\n"
        "The count argument gives the number of machine instruction "
        "to simulate, default is to step one instruction.");

    addText(
        Texts::TXT_INTERP_HELP_COMMANDS,
        "Displays the last N commands in the command history log. "
        "If the argument is omitted, 10 last commands are printed.");

    addText(
        Texts::TXT_INTERP_HELP_NEXTI,
        "Advances simulation to the next machine instructions in current "
        "procedure.\n\n"

        "If the instruction contains a function call, simulation proceeds "
        "until control returns from it, to the instruction past the function "
        "call. The argument gives the number of machine instruction to "
        "simulate. Default is one.");

    addText(
        Texts::TXT_INTERP_HELP_UNTIL,
        "Continue running until the program location specified by "
        "the given argument is reached.\n\n"
        "Any valid argument that applies "
        "to command 'bp' is also a valid argument for until.");

    addText(
        Texts::TXT_INTERP_HELP_X,
        "Prints the data in memory starting at specified address.\n\n"

        "Parameter /a can be used to select the address space if there are "
        "multiple address spaces in the target machine.\n\n"

        "\t/a [address space name]\n\n"

        "The optional parameteres /n and /u specify how much memory to "
        "display.\n\n"
        
        "\t/n\n\n"
        
        "Repeat count: how many data words (counting by units u) to "
        "display. If omitted, defaults to 1.\n\n"

        "\t/u\n\n"


        "Unit size: `b' (MAU, a byte in byte-addressed memories), `h' "
        "(double MAU), `w' (quadruple word, a `word' in byte-adressed "
        "32-bit architectures). For example, 'x /a data /n 8 /u w 1024' "
        "displays 8 words starting from address 1024 in address space "
        "'data'.\n\n"

        "If addr is omitted, then the first address past the last address "
        "displayed by the previous x command is implied. If the value of n, "
        "f, or u is not specified, the value given in the most recent x "
        "command is maintained. ");

    addText(
        Texts::TXT_CLI_ONLINE_HELP, 
        "The interactive simulation can be controlled by using "
        "simulator control language, which is based on the Toolset "
        "Control Language (Tcl). It extends the predefined set of Tcl "
        "commands with a set of commands that are used to control "
        "simulation.\n\nAll basic properties of Tcl (expression evaluation, "
        "parameter substitution rules, operators, and so on) are "
        "supported. ");

    addText(
        Texts::TXT_CMD_LINE_HELP,
        "Usage: ttasim [options]\n\n"
        "In case program file is given and no script is given using '-e', "
        "executes the simulation until the simulated program ends. Use "
        "'help' in the interactive/debugging mode to get help and listing " 
        "of commands of simulator control language.");

    addText(
        Texts::TXT_ILLEGAL_PROGRAM_IU_STATE_NOT_FOUND,
	    "Instruction referring to a nonexisting immediate unit.");

    addText(
        Texts::TXT_ILLEGAL_PROGRAM_RF_STATE_NOT_FOUND,
	    "Instruction referring to a nonexisting register file.");

    addText(
        Texts::TXT_ILLEGAL_PROGRAM_PORT_STATE_NOT_FOUND,
	    "Instruction referring to a nonexisting function unit port.");

    addText(
        Texts::TXT_ILLEGAL_PROGRAM_BUS_STATE_NOT_FOUND,
	    "Instruction referring to a nonexisting bus.");

    addText(
        Texts::TXT_ILLEGAL_PROGRAM_UNKNOWN_MOVE_TERMINAL_TYPE,
	    "Move with an unsupported terminal type found.");

    addText(
        Texts::TXT_ILLEGAL_PROGRAM_OPCODE_NOT_FOUND,
	    "A move without an opcode writing to an opcode setting port " 
	    "found.");

    addText(
        Texts::TXT_OPERATION_X_NOT_FOUND,
        "Operation definition for %s not found.");

    addText(
        Texts::TXT_INTERP_SIMULATION_NOT_INITIALIZED, 
        "Simulation not initialized.");

    addText(
        Texts::TXT_INTERP_SIMULATION_ALREDY_RUNNING, 
        "Simulation already running.");

    addText(
        Texts::TXT_INTERP_SIMULATION_NOT_RUNNING, 
        "No simulation running.");

    addText(
        Texts::TXT_INTERP_SIMULATION_NOT_ENDED,
        "Simulation has not ended.");

    addText(
        Texts::TXT_INTERP_HELP_SETTING,
        "Modify and inspect simulation settings.\n\n"
        "If no arguments are given, all simulator settings and their "
        "current values are listed. If one argument is given, it's expected "
        "to be a simulator setting, of which current value is printed. "
        "If two arguments are given, the first argument tells the name of "
        "the setting to modify, and second argument the new value to give "
        "for the setting.");

    addText(
        Texts::TXT_INTERP_HELP_RESUME,
        "Resume simulation of the program until the simulation is finished "
        "or a breakpoint is reached.\n\n" 
        "The optional argument gives the number of times the continue command "
        "is repeated, that is, the number of times breakpoints should be "
        "ignored.");

    addText(
        Texts::TXT_INTERP_HELP_HELP,
        "Prints a help message for the given command.\n\n"
        "If no command is given, prints general help and a listing of "
        "available commands.");

    addText(
        Texts::TXT_INTERP_HELP_INFO,
        "Prints info about simulation. Subcommand defines the type of info "
        "printed.\n\n"

        "Supported subcommands:\n\n"

        "\tbusses [busname]\n\n"

        "Prints the names of all the busses and their segments of the loaded "
        "machine. If name of the bus is given, prints only its segments.\n\n"

        "\tbreakpoints [num]\n\n"

        "Prints a table of all breakpoints. Each breakpoint is printed in "
        "a separate line. If argument is given, only the breakpoint with "
        "handle num is printed.\n\n"

        "\tfunits\n\n"

        "Prints the names of all of the function units in the loaded "
        "machine.\n\n"

        "\timmediates [iunit] [regname]\n\n"

        "Prints the value of register regname in immediate unit iunit.\n\n"

        "\tiunits\n\n"

        "Prints the names of all of the immediate units in the loaded "
        "machine.\n\n"

        "\tports funit [portname]\n\n"

        "Prints values of all ports in the given function unit. If "
        "port name is given, prints only the value of the given port.\n\n"

        "If regname is omitted, values of all registers of the specified "
        "register file are displayed. In sequential simulation, only regname "
        "can be given. If no arguments are given while in sequential "
        "simulation, all registers and their values written so far by the "
        "program are printed.\n\n"
        
        "\tproc cycles\n\n"

        "Displays the total execution cycle count.\n\n"

        "\tproc mapping\n\n"

        "Displays information of address spaces of the loaded processor.\n\n"

        "\tproc stats\n\n"

        "Displays processor utilization data.\n\n"

        "\tprogram\n\n"

        "Displays information about the status of the program: whether it "
        "is loaded or running, why it stopped.\n\n"

        "\tprogram is_instruction_reference [ins_addr] [move_index]\n\n"

        "Returns 1 if the source of the given move refers to an instruction "
        "address, 0 otherwise.\n\n"

        "\tregfiles\n\n"

        "Prints the names of all of the register files in the loaded "
        "machine.\n\n"

        "\tregisters [regfile] [regname]\n\n"

        "Prints the value of register regname in register file regfile.\n\n"

        "\tsegments [busname]\n\n"

        "Prints the value in the given bus. Segments are not yet supported.\n\n"
                
        "\tstats executed_operations\n\n"
                
        "Prints the total count of executed operations.\n\n"
                
        "\tstats register_reads\n\n"
                
        "Prints the total count of register reads.\n\n"
                
        "\tstats register_writes\n\n"
                
        "Prints the total count of register writes.");
        
    addText(
        Texts::TXT_INTERP_HELP_BREAK,
        "Creates a new breakpoint.\n\n"

        "A breakpoint stops the simulation whenever the simulator reaches "
        "a certain point in the program. It is possible to add a condition "
        "to a breakpoint, to control when the simulator must stop with "
        "increased precision.\n\n"
        
        "Possible arguments to the command:\n\n"
        
        "\t[none]\n\n"

        "Sets a breakpoint to the address following the current instruction "
        "address.\n\n"

        "\taddress\n\n"
        
        "Sets a breakpoint at address. Address can be a label or an "
        "absolute instruction address.\n\n"

        "\targs if\n\n"

        "Sets a conditional breakpoint. The arguments args are the same as "
        "for unconditional breakpoints. The expression condition is evaluated "
        "each time the breakpoint is reached, and the simulation is stopped "
        "only when the condition evaluates as true. Simulator prompts for "
        "the condition expression after user has entered this command.");

    addText(
        Texts::TXT_INTERP_HELP_WATCH,
        "Creates a new watch.\n\n"

        "A watch stops the simulation whenever the given expression changes "
        "its value. Interpreter prompts for the expression after this command "
        "is entered. It is possible to add a condition to a watch, to control "
        "when the simulator must stop with increased precision.\n\n");

    addText(
        Texts::TXT_INTERP_HELP_TBREAK,
        "Creates a new temporary breakpoint.\n\n"

        "Sets a temporary breakpoint, which is automatically deleted "
        "after the first time it stops the simulation. The arguments are "
        "the same as for the 'bp' command (see 'help bp').");

    addText(
        Texts::TXT_INTERP_HELP_CONDITION,
        "Specifies a condition under which the given breakpoint stops "
        "simulation.\n\n"
        "Simulator evaluates the entered condition expression whenever "
        "the breakpoint is reached, and stops simulation only if the "
        "expression evaluates as true (nonzero). Simulator checks expr "
        "for syntactic correctness after the expression is entered.\n\n"

        "When condition is an empty expression, any condition attached "
        "to the breakpoint is removed, thus the breakpoint becomes "
        "an ordinary unconditional breakpoint.");

    addText(
        Texts::TXT_INTERP_HELP_IGNORE,
        "Sets the number of times the breakpoint must be ignored when "
        "reached.\n\n"
        "First argument is the handle of the breakpoint of which condition "
        "to modify, second argument is the new ignore count. A count value "
        "zero means that the breakpoint will stop simulation next time it "
        "is reached.");

    addText(
        Texts::TXT_INTERP_HELP_DELETEBP, 
        "Deletes the specified breakpoint(s).\n\n"

        "If no arguments are given, deletes all breakpoints currently set, "
        "asking first for comfirmation.");

    addText(
        Texts::TXT_CONFIRM_DELETE_ALL_BREAKPOINTS,
        "Are you sure you want to delete all the breakpoints? ");

    addText(
        Texts::TXT_CONFIRM_KILL_SIMULATION,
        "Are you sure you want to restart simulation? ");

    addText(
        Texts::TXT_INTERP_HELP_ENABLEBP, 
        "Enables the specified breakpoint(s).\n\n"

        "If the second argument is 'once', the breakpoint(s) will be "
        "automatically disabled after reached once. If the second argument "
        "is 'delete', the breakpoint(s) will be automatically deleted after "
        "reached once.");

    addText(
        Texts::TXT_INTERP_HELP_DISABLEBP, 
        "Disables the specified breakpoint(s).\n\n"

        "A disabled breakpoint has no effect, but all its options "
        "(ignore-counts, conditions, etc.) are remembered in case the "
        "breakpoint is enabled again.");


    addText(Texts::TXT_INTERP_SETTING_EXECTRACE,             
            "Writing of the basic execution trace.");

    addText(Texts::TXT_INTERP_SETTING_BUSTRACE,             
            "Writing of the bus trace.");

    addText(
        Texts::TXT_INTERP_SETTING_FU_CONFLICT_DETECTION,
        "Function unit resource conflict detection (disable for speedup).");

    addText(
        Texts::TXT_INTERP_SETTING_RFTRACKING,             
        "Concurrent register file access tracking (shown in 'info "
        "proc stats').");

    addText(
        Texts::TXT_INTERP_SETTING_PROCEDURE_TRANSFER_TRACKING,             
        "Tracking of program procedure execution paths.");

    addText(
        Texts::TXT_INTERP_SETTING_MEMORY_ACCESS_TRACKING,
        "Tracking of memory reads and writes.");

    addText(
        Texts::TXT_INTERP_SETTING_UTILIZATION_SAVING,
        "Save processor utilization data to trace database after simulation.");

    addText(
        Texts::TXT_INTERP_SETTING_PROFILE_SAVING,
        "Save program profile data to trace database after simulation.");

    addText(
        Texts::TXT_INTERP_SETTING_HISTORY_FILENAME,
        "File to store the command history log in.");

    addText(
        Texts::TXT_INTERP_SETTING_HISTORY_SAVE,
        "Saving of command history to a file.");

    addText(
        Texts::TXT_INTERP_SETTING_HISTORY_SIZE,
        "Maximum size of the command history log.");

    addText(
        Texts::TXT_INTERP_SETTING_NEXT_INSTRUCTION_PRINTING,
        "Printing of the next executed instruction when simulation stops.");               

    addText(
        Texts::TXT_INTERP_SETTING_PARSE_ERROR,
        "Invalid setting value. Should be %s.");

    addText(
        Texts::TXT_INTERP_HELP_COMMANDS_AVAILABLE, 
        "Simulator control language commands available "
        "(use help [command] to get command specific help):");
    
    addText(
        Texts::TXT_SIMULATION_TIME_STATISTICS,
        "Printing of the time statistics for the last simulation command.");

    addText(
        Texts::TXT_SIMULATION_TIME_STATISTICS,
        "Printing of the time statistics for the last command ran "
        "(run, until, nexti)."); 

    addText(Texts::TXT_STATUS_STOPPED, "Program stopped at address %d.");
    addText(Texts::TXT_STATUS_FINISHED, "Simulation finished.");            
    addText(
        Texts::TXT_STATUS_INITIALIZED,
        "Loaded program not being simulated.");
    addText(Texts::TXT_STATUS_NOT_INITIALIZED, "Program not loaded.");

    addText(Texts::TXT_STOPREASON_BREAKPOINT, "Stopped at breakpoint %d.");
    addText(
        Texts::TXT_STOPREASON_DELETED_BREAKPOINT, 
        "Stopped at a breakpoint that has since been deleted.");

    addText(
        Texts::TXT_STOPREASON_UNTIL, 
        "Stopped after reaching the address defined with "
        "an 'until' or a 'nexti' commmand.");

    addText(
        Texts::TXT_INTERP_HELP_KILL,
        "Terminate the simulation.\n\n"

        "The program being simulated remains loaded and the simulation "
        "can be restarted from the beginning by means of command 'run'. "
        "Simulator will prompt the user for comfirmation before "
        "terminating the simulation.");

    addText(Texts::TXT_STOPREASON_STEPPING, "Stopped after being stepped.");
    addText(
        Texts::TXT_STOPREASON_USERREQUEST, 
        "Stopped after a stop request from the user.");

    addText(
        Texts::TXT_STOPREASON_RUNTIME_ERROR,
        "Stopped because of a runtime error in the simulated program.");

    addText(Texts::TXT_LABEL_NOT_FOUND, "Label not found.");
    addText(
        Texts::TXT_ILLEGAL_ADDRESS_EXPRESSION, 
        "Illegal address expression.");

    addText(Texts::TXT_BREAKPOINT_NOT_FOUND, "Breakpoint not found.");

    addText(Texts::TXT_INTERP_ENTER_CONDITION_PROMPT, "enter condition: ");
    addText(Texts::TXT_INTERP_ENTER_EXPRESSION_PROMPT, "enter expression: ");
    addText(
        Texts::TXT_EXPRESSION_EMPTY, "Expression script must not be empty.");
    addText(Texts::TXT_ADDRESS_OUT_OF_RANGE, "Address out of range.");
    addText(Texts::TXT_ADDRESS_SPACE_NOT_FOUND, "Address space not found.");
    addText(
        Texts::TXT_NOT_RUNNING_PARALLEL_SIMULATION,
        "Not running parallel simulation.");

    addText(Texts::TXT_UNABLE_TO_LOAD_PROGRAM, "Unable to load program.");
    addText(Texts::TXT_UNABLE_TO_LOAD_MACHINE, "Unable to load machine.");
    addText(
        Texts::TXT_NO_ADDRESS_SPACE_GIVEN, "No address space given (use /a).");
    addText(Texts::TXT_NO_PROGRAM_LOADED, "No program loaded.");
    addText(Texts::TXT_AUTOMATIC_FINISH_IMPOSSIBLE,
            "Unable to detect the program exit point. Simulation cannot be "
            "finished automatically. Use 'until' instead of 'run'.");
    addText(Texts::TXT_TRACE_SETTINGS_CHANGED_WARNING,
            "Warning! Trace settings have been changed. "
            "Please reload the program and/or the machine.");
}

/**
 * The Destructor.
 */
SimulatorTextGenerator::~SimulatorTextGenerator() {
}


