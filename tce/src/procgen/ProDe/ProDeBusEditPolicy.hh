/**
 * @file ProDeBusEditPolicy.hh
 *
 * Declaration of ProDeBusEditPolicy class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#ifndef TTA_PRODE_BUS_EDIT_POLICY_HH
#define TTA_PRODE_BUS_EDIT_POLICY_HH

#include "EditPolicy.hh"

class Request;
class ComponentCommand;

/**
 * Determines how a bus EditPart acts when a Request is performed
 * on it.
 *
 * Converts a given Request to a Command if the EditPolicy supports.
 * the Request.
 */
class ProDeBusEditPolicy : public EditPolicy {
public:
    ProDeBusEditPolicy();
    virtual ~ProDeBusEditPolicy();

    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;

private:
    /// Assignment not allowed.
    ProDeBusEditPolicy& operator=(ProDeBusEditPolicy& old);
    /// Copying not allowed.
    ProDeBusEditPolicy(ProDeBusEditPolicy& old);
};

#endif
