/**
 * @file ProximPortEditPolicy.hh
 *
 * Declaration of ProximPortEditPolicy class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_PORT_EDIT_POLICY
#define TTA_PROXIM_PORT_EDIT_POLICY

#include "EditPolicy.hh"

/**
 * Edit policy for Proxim machine state window ports.
 */
class ProximPortEditPolicy : public EditPolicy {
public:
    ProximPortEditPolicy();
    virtual ~ProximPortEditPolicy();
    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;
};
#endif
