/**
 * @file ProDeSRPortEditPolicy.hh
 *
 * Declaration of ProDeSRPortEditPolicy class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRODE_SR_PORT_EDIT_POLICY_HH
#define TTA_PRODE_SR_PORT_EDIT_POLICY_HH

#include "EditPolicy.hh"

class Request;
class ComponentCommand;

/**
 * Determines how a special register port EditPart acts when a Request is
 * performed on it.
 *
 * Converts a given Request to a Command if the EditPolicy supports
 * the Request.
 */
class ProDeSRPortEditPolicy : public EditPolicy {
public:
    ProDeSRPortEditPolicy();
    virtual ~ProDeSRPortEditPolicy();

    virtual ComponentCommand* getCommand(Request* request);
    virtual bool canHandle(Request* request) const;

private:
    /// Assignment not allowed.
    ProDeSRPortEditPolicy& operator=(ProDeSRPortEditPolicy& old);
    /// Copying not allowed.
    ProDeSRPortEditPolicy(ProDeSRPortEditPolicy& old);
};

#endif
