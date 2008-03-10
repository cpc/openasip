/**
 * @file SimulatorInterpreter.hh
 *
 * Declaration of SimulatorInterpreter class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMULATOR_INTERPRETER
#define TTA_SIMULATOR_INTERPRETER

#include <set>

#include "TclInterpreter.hh"
#include "SimulatorInterpreterContext.hh"


/**
 * Implementation of the interpreter for the Simulator Control Language.
 *
 * Uses TCL as the base interpreter class and adds the custom commands of the
 * Simulator Control Language to it.
 *
 */
class SimulatorInterpreter : public TclInterpreter {
public:
    SimulatorInterpreter(
        int argc, 
        char* argv[], 
        SimulatorInterpreterContext& context,
        LineReader& reader);
    virtual ~SimulatorInterpreter();

    bool isQuitCommandGiven() const;
    void setQuitCommandGiven();

private:
    bool quitted_;

};

#endif
