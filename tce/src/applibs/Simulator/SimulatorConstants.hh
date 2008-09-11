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
 * @file SimulatorConstants.hh
 *
 * Definitions of constants and types used in simulator engine and its 
 * user interfaces.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
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

/// Alias for ClockCycleCount.
typedef CycleCount ClockCycleCount;

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
