/**
 * @file SimulatorConstants.hh
 *
 * Definitions of constants and types used in simulator engine and its 
 * user interfaces.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef HH_SIMULATOR_CONSTANTS
#define HH_SIMULATOR_CONSTANTS

#include "BaseType.hh"
//#include <stdint.h>

/// The command prompt of the simulator interpreter.
#define SIM_COMMAND_PROMPT "(ttasim) "

/// The command used to quit the command line interface.
#define SIM_INTERP_QUIT_COMMAND "quit"

/// The initialization script file name.
#define SIM_INIT_FILE_NAME ".ttasim-init"

/// The default command log file name.
#define SIM_DEFAULT_COMMAND_LOG ".ttasim-history"

/// The current version of the Command Line Interface application.
#define SIM_CLI_VERSION "1.0"

/// The full title name of the CLI application
#define SIM_CLI_TITLE "ttasim - TCE Simulator command line interface"

/// A type for storing clock cycle counts.
/// @note was uint64_t, changed due to unnecessary header overhead
typedef long long ClockCycleCount;

/// Alias for ClockCycleCount.
typedef ClockCycleCount CycleCount;

/// The reasons to stop simulation.
enum StopReason {
    SRE_AFTER_STEPPING, ///< Stopped after stepping the given count.
    SRE_AFTER_UNTIL,    ///< Stopped after running to the wanted 
    SRE_AFTER_TIMEOUT,  ///< Stopped after simulation timeout
    ///  instruction.
    SRE_BREAKPOINT,     ///< Stopped because of at least one breakpoint
    SRE_USER_REQUESTED, ///< User requested the simulation to stop 
                        ///  explicitly, e.g., by pressing ctrl-c in the CLI.
    SRE_RUNTIME_ERROR   ///< A fatal runtime error occured in the simulated
                        ///  program.
};

#endif
