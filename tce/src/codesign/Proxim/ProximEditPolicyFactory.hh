/**
 * @file ProximEditPolicyFactory.hh
 *
 * Declration of ProximEditPolicyFactory class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_EDIT_POLICY_FACTORY
#define TTA_PROXIM_EDIT_POLICY_FACTORY


#include "EditPolicyFactory.hh"

/**
 * Factory which generates editpolicies for Proxim MachineWindow components.
 */
class ProximEditPolicyFactory : public EditPolicyFactory {
public:
    ProximEditPolicyFactory();
    virtual ~ProximEditPolicyFactory();
    virtual EditPolicy* createRFEditPolicy();
    virtual EditPolicy* createFUEditPolicy();
    virtual EditPolicy* createIUEditPolicy();
    virtual EditPolicy* createGCUEditPolicy();
    virtual EditPolicy* createSocketEditPolicy();
    virtual EditPolicy* createPortEditPolicy();
    virtual EditPolicy* createBusEditPolicy();
    virtual EditPolicy* createSegmentEditPolicy();
};

#endif
