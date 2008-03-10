/**
 * @file DummyMachineTester.hh
 *
 * Declaration of DummyMachineTester class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_DUMMY_MACHINE_TESTER_HH
#define TTA_DUMMY_MACHINE_TESTER_HH

#include "MachineTester.hh"

/**
 * A dummy machine tester which doesn't test anything.
 */
class DummyMachineTester : public MachineTester {
public:
    DummyMachineTester(const TTAMachine::Machine& machine);
    virtual ~DummyMachineTester();

    virtual bool canConnect(
        const TTAMachine::Socket& socket, 
        const TTAMachine::Segment& segment);
    virtual bool canConnect(
        const TTAMachine::Socket& socket,
        const TTAMachine::Port& port);
    virtual bool canBridge(
        const TTAMachine::Bus& source,
        const TTAMachine::Bus& destination);
    virtual bool canSetDirection(
        const TTAMachine::Socket& socket, 
        TTAMachine::Socket::Direction direction);
};

#endif
