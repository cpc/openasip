/**
 * @file MachineCheck.hh
 *
 * Declaration of MachineCheck class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_CHECK_HH
#define TTA_MACHINE_CHECK_HH

#include "MachineCheckResults.hh"

namespace TTAMachine {
    class Machine;
}


/**
 * Base class for checks to perform on a machine.
 */
class MachineCheck {
public:
    virtual std::string shortDescription() const;
    virtual std::string description() const;

    virtual bool check(
        const TTAMachine::Machine& mach, MachineCheckResults& results)
        const = 0;

    virtual bool canFix(const TTAMachine::Machine& mach) const;
    virtual std::string fix(TTAMachine::Machine& mach) const
        throw (InvalidData);

    virtual ~MachineCheck();

protected:
    MachineCheck(const std::string& shortDesc_);

private:
    /// Short description of the check.
    const std::string& shortDesc_;
};

#endif
