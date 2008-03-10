/**
 * @file EditPolicy.hh
 *
 * Declaration of EditPolicy class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#ifndef TTA_EDIT_POLICY_HH
#define TTA_EDIT_POLICY_HH

class EditPart;
class Request;
class ComponentCommand;
 
/**
 * Determines how an EditPart acts when a Request is performed on it.
 *
 * Converts a given Request to a Command if the EditPolicy supports
 * the Request.
 */
class EditPolicy {
public:
    EditPolicy();
    virtual ~EditPolicy();

    EditPart* host() const;
    void setHost(EditPart* host);

    /**
     * Returns the Command corresponding to the type of the Request.
     *
     * @param request Request to be handled.
     * @return NULL if the Request cannot be handled.
     */
    virtual ComponentCommand* getCommand(Request* request) = 0;

    /**
     * Tells whether this EditPolicy is able to handle a certain type
     * of Request.
     *
     * @param request Request to be asked if it can be handled.
     * @return True if the Request can be handled, false otherwise.
     */
    virtual bool canHandle(Request* request) const = 0;

protected:
    /// Host EditPart of this EditPolicy.
    EditPart* host_;

private:
    /// Assignment not allowed.
    EditPolicy& operator=(EditPolicy& old);
    /// Copying not allowed.
    EditPolicy(EditPolicy& old);
};

#include "EditPolicy.icc"

#endif
