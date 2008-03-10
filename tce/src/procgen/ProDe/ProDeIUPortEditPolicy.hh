/**
 * @file ProDeIUPortEditPolicy.hh
 *
 * Declaration of ProDeIUPortEditPolicy class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRODE_IU_PORT_EDIT_POLICY_HH
#define TTA_PRODE_IU_PORT_EDIT_POLICY_HH

#include "EditPolicy.hh"

class Request;
class ComponentCommand;

/**
 * Determines how an immediate unit port EditPart acts when a Request is
 * performed on it.
 *
 * Converts a given Request to a Command if the EditPolicy supports
 * the Request.
 */
class ProDeIUPortEditPolicy : public EditPolicy {
public:
    ProDeIUPortEditPolicy();
    virtual ~ProDeIUPortEditPolicy();

    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;

private:
    /// Assignment not allowed.
    ProDeIUPortEditPolicy& operator=(ProDeIUPortEditPolicy& old);
    /// Copying not allowed.
    ProDeIUPortEditPolicy(ProDeIUPortEditPolicy& old);
};

#endif
