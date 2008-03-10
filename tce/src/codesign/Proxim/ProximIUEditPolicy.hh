/**
 * @file ProximIUEditPolicy.hh
 *
 * Declaration of ProximIUEditPolicy class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_IU_EDIT_POLICY
#define TTA_PROXIM_IU_EDIT_POLICY

#include "EditPolicy.hh"

/**
 * Edit policy for immediate units in the Proxim machine state window.
 */
class ProximIUEditPolicy : public EditPolicy {
public:
    ProximIUEditPolicy();
    virtual ~ProximIUEditPolicy();
    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;
};
#endif
