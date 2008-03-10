/**
 * @file ProDeFUPortEditPolicy.hh
 *
 * Declaration of ProDeFUPortEditPolicy class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRODE_FU_PORT_EDIT_POLICY_HH
#define TTA_PRODE_FU_PORT_EDIT_POLICY_HH

#include "EditPolicy.hh"

class Request;
class ComponentCommand;

/**
 * Determines how a function unit port EditPart acts when a Request is
 * performed on it.
 *
 * Converts a given Request to a Command if the EditPolicy supports
 * the Request.
 */
class ProDeFUPortEditPolicy : public EditPolicy {
public:
    ProDeFUPortEditPolicy();
    virtual ~ProDeFUPortEditPolicy();

    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;

private:
    /// Assignment not allowed.
    ProDeFUPortEditPolicy& operator=(ProDeFUPortEditPolicy& old);
    /// Copying not allowed.
    ProDeFUPortEditPolicy(ProDeFUPortEditPolicy& old);
};

#endif
