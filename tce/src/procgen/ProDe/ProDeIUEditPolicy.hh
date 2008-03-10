/**
 * @file ProDeIUEditPolicy.hh
 *
 * Declaration of ProDeIUEditPolicy class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#ifndef TTA_PRODE_IU_EDIT_POLICY_HH
#define TTA_PRODE_IU_EDIT_POLICY_HH

#include "EditPolicy.hh"

class Request;
class ComponentCommand;

/**
 * Determines how an immediate unit EditPart acts when a Request is performed
 * on it.
 *
 * Converts a given Request to a Command if the EditPolicy supports
 * the Request.
 */
class ProDeIUEditPolicy : public EditPolicy {
public:
    ProDeIUEditPolicy();
    virtual ~ProDeIUEditPolicy();

    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;

private:
    /// Assignment not allowed.
    ProDeIUEditPolicy& operator=(ProDeIUEditPolicy& old);
    /// Copying not allowed.
    ProDeIUEditPolicy(ProDeIUEditPolicy& old);
};

#endif
