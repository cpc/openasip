/**
 * @file ProDeEditPolicyFactory.hh
 *
 * Declaration of ProDeEditPolicyFactory class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRODE_EDIT_POLICY_FACTORY_HH
#define TTA_PRODE_EDIT_POLICY_FACTORY_HH

#include "EditPolicyFactory.hh"

/**
 * Factory for creating edit policies for ProDe machine window
 * edit parts.
 */
class ProDeEditPolicyFactory : public EditPolicyFactory {
public:
    ProDeEditPolicyFactory();
    virtual ~ProDeEditPolicyFactory();

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

#endif
