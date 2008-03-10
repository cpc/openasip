/**
 * @file ProDeRFPortEditPolicy.hh
 *
 * Declaration of ProDeRFPortEditPolicy class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRODE_RF_PORT_EDIT_POLICY_HH
#define TTA_PRODE_RF_PORT_EDIT_POLICY_HH

#include "EditPolicy.hh"

class Request;
class ComponentCommand;

/**
 * Determines how a register file port EditPart acts when a Request is
 * performed on it.
 *
 * Converts a given Request to a Command if the EditPolicy supports
 * the Request.
 */
class ProDeRFPortEditPolicy : public EditPolicy {
public:
    ProDeRFPortEditPolicy();
    virtual ~ProDeRFPortEditPolicy();

    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;

private:
    /// Assignment not allowed.
    ProDeRFPortEditPolicy& operator=(ProDeRFPortEditPolicy& old);
    /// Copying not allowed.
    ProDeRFPortEditPolicy(ProDeRFPortEditPolicy& old);
};

#endif
