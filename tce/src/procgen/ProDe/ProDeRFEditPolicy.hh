/**
 * @file ProDeRFEditPolicy.hh
 *
 * Declaration of ProDeRFEditPolicy class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#ifndef TTA_PRODE_RF_EDIT_POLICY_HH
#define TTA_PRODE_RF_EDIT_POLICY_HH


#include "EditPolicy.hh"

class Request;
class ComponentCommand;

/**
 * Determines how a register file EditPart acts when a Request is performed
 * on it.
 *
 * Converts a given Request to a Command if the EditPolicy supports
 * the Request.
 */
class ProDeRFEditPolicy : public EditPolicy {
public:
    ProDeRFEditPolicy();
    virtual ~ProDeRFEditPolicy();

    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;

private:
    /// Assignment not allowed.
    ProDeRFEditPolicy& operator=(ProDeRFEditPolicy& old);
    /// Copying not allowed.
    ProDeRFEditPolicy(ProDeRFEditPolicy& old);
};

#endif
