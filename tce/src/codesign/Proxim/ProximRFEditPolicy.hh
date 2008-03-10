/**
 * @file ProximRFEditPolicy.hh
 *
 * Declaration of ProximRFEditPolicy class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_RF_EDIT_POLICY
#define TTA_PROXIM_RF_EDIT_POLICY

#include "EditPolicy.hh"

/**
 * Edit policy for register files in the Proxim machine state window.
 */
class ProximRFEditPolicy : public EditPolicy {
public:
    ProximRFEditPolicy();
    virtual ~ProximRFEditPolicy();
    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;
};
#endif
