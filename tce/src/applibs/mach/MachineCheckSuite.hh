/**
 * @file MachineCheckSuite.hh
 *
 * Declaration of MachineCheckSuite class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_CHECK_SUITE_HH
#define TTA_MACHINE_CHECK_SUITE_HH

#include <vector>

#include "Exception.hh"

namespace TTAMachine {
    class Machine;
}

class MachineCheck;
class MachineCheckResults;

/**
 * Container for a group machine checks that are meant to be run as
 * a test suite.
 */
class MachineCheckSuite {
public:
    virtual ~MachineCheckSuite();

    int checkCount() const;
    const MachineCheck& check(int index)
        throw (OutOfRange);

    bool run(
        const TTAMachine::Machine& machine,
        MachineCheckResults& results);

protected:
    MachineCheckSuite();
    void addCheck(MachineCheck* check);

private:
    /// Checks contained in the suite.
    std::vector<MachineCheck*> checks_;

};

#endif
