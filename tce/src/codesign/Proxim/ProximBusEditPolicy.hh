/**
 * @file ProximBusEditPolicy.hh
 *
 * Declaration of ProximBusEditPolicy class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_BUS_EDIT_POLICY
#define TTA_PROXIM_BUS_EDIT_POLICY

#include "EditPolicy.hh"

/**
 * Edit policy for Proxim machine state window buses.
 */
class ProximBusEditPolicy : public EditPolicy {
public:
    ProximBusEditPolicy();
    virtual ~ProximBusEditPolicy();
    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;
};
#endif
