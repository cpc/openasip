/**
 * @file ProDeSocketEditPolicy.hh
 *
 * Declaration of ProDeSocketEditPolicy class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#ifndef TTA_PRODE_SOCKET_EDIT_POLICY_HH
#define TTA_PRODE_SOCKET_EDIT_POLICY_HH

#include "EditPolicy.hh"

class Request;
class ComponentCommand;

/**
 * Determines how a socket EditPart acts when a Request is performed
 * on it.
 *
 * Converts a given Request to a Command if the EditPolicy supports.
 * the Request.
 */
class ProDeSocketEditPolicy : public EditPolicy {
public:
    ProDeSocketEditPolicy();
    virtual ~ProDeSocketEditPolicy();

    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;

private:
    ComponentCommand* createConnectCmd(Request* request);
    /// Assignment not allowed.
    ProDeSocketEditPolicy& operator=(ProDeSocketEditPolicy& old);
    /// Copying not allowed.
    ProDeSocketEditPolicy(ProDeSocketEditPolicy& old);
};

#endif
