/**
 * @file MachineTestReporter.hh
 *
 * Declaration of MachineTestReporter class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 */

#ifndef TTA_MACHINE_TEST_REPORTER_HH
#define TTA_MACHINE_TEST_REPORTER_HH

#include <string>

#include "Socket.hh"

class MachineTester;

/**
 * Creates string error messages according to the error stated by
 * MachineTester.
 */
class MachineTestReporter {
public:
    static std::string socketSegmentConnectionError(
        const TTAMachine::Socket& socket,
        const TTAMachine::Segment& segment,
        const MachineTester& tester);
    static std::string socketPortConnectionError(
        const TTAMachine::Socket& socket,
        const TTAMachine::Port& port,
        const MachineTester& tester);
    static std::string bridgingError(
        const TTAMachine::Bus& sourceBus,
        const TTAMachine::Bus& destinationBus,
        const MachineTester& tester);
    static std::string socketDirectionSettingError(
        const TTAMachine::Socket& socket,
        TTAMachine::Socket::Direction,
        const MachineTester& tester);
};

#endif
