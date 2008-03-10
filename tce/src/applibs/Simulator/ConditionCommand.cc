/**
 * @file ConditionCommand.cc
 *
 * Implementation of ConditionCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ConditionCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimValue.hh"
#include "StopPointManager.hh"
#include "TclConditionScript.hh"

#include <iostream>

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
ConditionCommand::ConditionCommand() : 
    SimControlLanguageCommand("condition") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
ConditionCommand::~ConditionCommand() {
}

/**
 * Executes the "condition" command.
 *
 * Modifies conditions of breakpoints.
 *
 * @param arguments The handle of the breakpoint of which condition to modify.
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 * @todo Use the count for the step.
 */
bool 
ConditionCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    if (!checkArgumentCount(arguments.size() - 1, 1, 1)) {
        return false;
    } 

    if (!checkSimulationStopped()) {
        if (!checkSimulationInitialized()) {
            return false;
        }
    }

    if (arguments.size() == 2) {
        if  (!checkIntegerArgument(arguments[1])) {
            return false;
        }
        const int breakpointHandle = arguments[1].integerValue();
        try {
            StopPointManager& stopPointManager =
                simulatorFrontend().stopPointManager();
            stopPointManager.stopPointWithHandleConst(breakpointHandle);

            TclConditionScript condition(NULL, "");
            if (!askConditionFromUser(condition)) {
                return false;
            }
            assert(condition.script().size() > 0);
            if (condition.script().at(0) == "1") {
                stopPointManager.removeCondition(breakpointHandle);
            } else {
                stopPointManager.setCondition(breakpointHandle, condition);
            }
            printBreakpointInfo(breakpointHandle);
        } catch (const InstanceNotFound&) {
            interpreter()->setError(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_BREAKPOINT_NOT_FOUND).str());
            return false;
        }
        

    }
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
ConditionCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_CONDITION).str();
}
