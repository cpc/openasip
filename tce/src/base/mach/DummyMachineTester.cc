/**
 * @file DummyMachineTester.cc
 *
 * Implementation of DummyMachineTester class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "DummyMachineTester.hh"

/**
 * The constructor.
 *
 * @param Machine The machine to be tested.
 */
DummyMachineTester::DummyMachineTester(const TTAMachine::Machine& machine) :
    MachineTester(machine) {
}


/**
 * The destructor.
 */
DummyMachineTester::~DummyMachineTester() {
}


/**
 * Always returns true.
 *
 * @return True.
 */
bool
DummyMachineTester::canConnect(
    const TTAMachine::Socket&,
    const TTAMachine::Segment&) {

    return true;
}


/**
 * Always returns true.
 *
 * @return True.
 */
bool
DummyMachineTester::canConnect(
    const TTAMachine::Socket&,
    const TTAMachine::Port&) {

    return true;
}


/**
 * Always returns true.
 *
 * @return True.
 */
bool
DummyMachineTester::canBridge(
    const TTAMachine::Bus&,
    const TTAMachine::Bus&) {

    return true;
}


/**
 * Always returns true.
 *
 * @return True.
 */
bool
DummyMachineTester::canSetDirection(
    const TTAMachine::Socket&,
    TTAMachine::Socket::Direction) {

    return true;
}
