/**
 * @file ProximFUEditPolicy.hh
 *
 * Declaration of ProximFUEditPolicy class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_FU_EDIT_POLICY
#define TTA_PROXIM_FU_EDIT_POLICY

#include "EditPolicy.hh"

/**
 * Edit policy for proxim machine window function units.
 */
class ProximFUEditPolicy : public EditPolicy {
public:
    ProximFUEditPolicy();
    virtual ~ProximFUEditPolicy();
    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;
};
#endif
