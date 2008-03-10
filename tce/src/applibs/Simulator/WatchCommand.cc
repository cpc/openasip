/**
 * @file WatchCommand.cc
 *
 * Implementation of WatchCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "WatchCommand.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimControlLanguageCommand.hh"
#include "StopPointManager.hh"
#include "Breakpoint.hh"
#include "ExpressionScript.hh"
#include "Watch.hh"

#include <iostream>

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
WatchCommand::WatchCommand() : SimControlLanguageCommand("watch") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
WatchCommand::~WatchCommand() {
}

/**
 * Executes the "watch" command.
 *
 * A watch stops the simulation whenever the given expression changes its
 * value. It is possible to add a condition to  a watch, to control when 
 * the Simulator must stop with increased precision. 
 *
 * @param arguments None. The expression is queried from the user.
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 */
bool 
WatchCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    if (!checkArgumentCount(arguments.size() - 1, 0, 0)) {
        return false;
    }

    ExpressionScript expression(NULL, "");
    if (!askExpressionFromUser(expression)) {
        return false;
    }

    Watch watch(simulatorFrontend(), expression);

    StopPointManager& stopPointManager = 
        simulatorFrontend().stopPointManager();
    return printBreakpointInfo(stopPointManager.add(watch));
}

/**
 * Returns the help text for this command.
 * 
 * Help text is searched from SimulatorTextGenerator.
 *
 * @return The help text.
 */
std::string 
WatchCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_WATCH).str();
}
