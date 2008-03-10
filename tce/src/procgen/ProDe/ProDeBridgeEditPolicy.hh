/**
 * @file ProDeBridgeEditPolicy.hh
 *
 * Declaration of ProDeBridgeEditPolicy class.
 *
 * @author Ari Metsähalme 2004 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#ifndef TTA_PRODE_BRIDGE_EDIT_POLICY_HH
#define TTA_PRODE_BRIDGE_EDIT_POLICY_HH

#include "EditPolicy.hh"

class Request;
class ComponentCommand;

/**
 * Determines how a bridge EditPart acts when a Request is performed
 * on it.
 *
 * Converts a given Request to a Command if the EditPolicy supports.
 * the Request.
 */
class ProDeBridgeEditPolicy : public EditPolicy {
public:
    ProDeBridgeEditPolicy();
    virtual ~ProDeBridgeEditPolicy();

    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;

private:
    /// Assignment not allowed.
    ProDeBridgeEditPolicy& operator=(ProDeBridgeEditPolicy& old);
    /// Copying not allowed.
    ProDeBridgeEditPolicy(ProDeBridgeEditPolicy& old);
};

#endif
