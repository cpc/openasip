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
