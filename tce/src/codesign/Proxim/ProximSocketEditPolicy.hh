/**
 * @file ProximSocketEditPolicy.hh
 *
 * Declaration of ProximSocketEditPolicy class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_SOCKET_EDIT_POLICY
#define TTA_PROXIM_SOCKET_EDIT_POLICY

#include "EditPolicy.hh"

/**
 * Edit policy for Proxim machine state window sockets.
 */
class ProximSocketEditPolicy : public EditPolicy {
public:
    ProximSocketEditPolicy();
    virtual ~ProximSocketEditPolicy();
    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;
};
#endif
