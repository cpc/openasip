/**
 * @file ProDePortEditPolicy.hh
 *
 * Declaration of ProDePortEditPolicy class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#ifndef TTA_PRODE_PORT_EDIT_POLICY_HH
#define TTA_PRODE_PORT_EDIT_POLICY_HH

#include "EditPolicy.hh"

class Request;
class ComponentCommand;

/**
 * Determines how a port EditPart acts when a Request is performed
 * on it.
 *
 * Converts a given Request to a Command if the EditPolicy supports
 * the Request.
 */
class ProDePortEditPolicy : public EditPolicy {
public:
    ProDePortEditPolicy();
    virtual ~ProDePortEditPolicy();

    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;

private:
    ComponentCommand* createConnectCmd(Request* request);
    /// Assignment not allowed.
    ProDePortEditPolicy& operator=(ProDePortEditPolicy& old);
    /// Copying not allowed.
    ProDePortEditPolicy(ProDePortEditPolicy& old);
};

#endif
