/**
 * @file RFPortCheck.hh
 *
 * Declaration of RFPortCheck class.
 *
 * @author Pekka J‰‰skel‰inen 2008 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef RF_PORT_CHECK_HH
#define RF_PORT_CHECK_HH

#include "MachineCheck.hh"

/**
 * Checks that the ports of the register files are sensible.
 *
 * Currently checks only that there is at least one input port in the
 * register file.
 */
class RFPortCheck : public MachineCheck {
public:
    RFPortCheck();
    virtual ~RFPortCheck();

    virtual bool check(
        const TTAMachine::Machine& mach,
        MachineCheckResults& results) const;
};

#endif
