/**
 * @file EditPolicyFactory.hh
 *
 * Declaration of EditPolicyFactory class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_EDIT_POLICY_FACTORY_HH
#define TTA_EDIT_POLICY_FACTORY_HH

class EditPolicy;

/**
 * Base class for EditPolicyFactories, which are utilized by the
 * EditPartFactory class to create EditPolicies.
 *
 * EditPolicies of MachineCanvas components can be defined by deriving
 * a factory from this class, and passing it to the EditPartFactory as the
 * EditPolicyFactory.
 */
class EditPolicyFactory {
public:
    EditPolicyFactory();
    virtual ~EditPolicyFactory();

    virtual EditPolicy* createFUEditPolicy();
    virtual EditPolicy* createRFEditPolicy();
    virtual EditPolicy* createIUEditPolicy();
    virtual EditPolicy* createGCUEditPolicy();

    virtual EditPolicy* createFUPortEditPolicy();
    virtual EditPolicy* createRFPortEditPolicy();
    virtual EditPolicy* createIUPortEditPolicy();
    virtual EditPolicy* createSRPortEditPolicy();
    virtual EditPolicy* createPortEditPolicy();

    virtual EditPolicy* createBusEditPolicy();
    virtual EditPolicy* createSocketEditPolicy();
    virtual EditPolicy* createSegmentEditPolicy();
    virtual EditPolicy* createBridgeEditPolicy();
};

#include "EditPolicyFactory.icc"

#endif
