/**
 * @file FUPortImplementation.cc
 *
 * Implementation of FUPortImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <string>

#include "FUPortImplementation.hh"
#include "FUImplementation.hh"

using std::string;

namespace HDB {

/**
 * The constructor.
 *
 * Adds the port automatically to the given FUImplementation.
 *
 * @param name Name of the port.
 * @param architecturePort Name of the corresponding port in architecture.
 * @param widthFormula The formula for the width of the port.
 * @param loadPort Name of the load enable port.
 * @param guardPort Name of the guard port if the port is guarded.
 * @param parent The parent FU implementation.
 */
FUPortImplementation::FUPortImplementation(
    const std::string& name,
    const std::string& architecturePort,
    const std::string& widthFormula,
    const std::string& loadPort,
    const std::string& guardPort,
    FUImplementation& parent) :
    PortImplementation(name, loadPort), 
    architecturePort_(architecturePort), widthFormula_(widthFormula), 
    guardPort_(guardPort) {

    parent.addArchitecturePort(this);
}


/**
 * The destructor.
 */
FUPortImplementation::~FUPortImplementation() {
}


/**
 * Sets the corresponding port in the FU architecture.
 *
 * @param name Name of the port.
 */
void
FUPortImplementation::setArchitecturePort(const std::string& name) {
    architecturePort_ = name;
}


/**
 * Returns the name of the corresponding port in the FU architecture.
 *
 * @return The name of the port.
 */
std::string
FUPortImplementation::architecturePort() const {
    return architecturePort_;
}


/**
 * Sets the name of the guard port if the port is guarded.
 *
 * @param name Name of the guard port.
 */
void
FUPortImplementation::setGuardPort(const std::string& name) {
    guardPort_ = name;
}


/**
 * Returns the name of the guard port if the port is guarded.
 *
 * @return The name of the guard port.
 */
std::string
FUPortImplementation::guardPort() const {
    return guardPort_;
}


/**
 * Sets the width formula of the port.
 *
 * @param formula The new formula.
 */
void
FUPortImplementation::setWidthFormula(const std::string& formula) {
    widthFormula_ = formula;
}


/**
 * Returns the formula for the width of the port.
 *
 * @return The formula.
 */
std::string
FUPortImplementation::widthFormula() const {
    return widthFormula_;
}
}
