/**
 * @file ProximEditPolicyFactory.cc
 *
 * Implementation of ProximEditPolicyFactory class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximEditPolicyFactory.hh"
#include "ProximRFEditPolicy.hh"
#include "ProximFUEditPolicy.hh"
#include "ProximIUEditPolicy.hh"
#include "ProximSocketEditPolicy.hh"
#include "ProximPortEditPolicy.hh"
#include "ProximBusEditPolicy.hh"

/**
 * The Constructor
 */
ProximEditPolicyFactory::ProximEditPolicyFactory() :
    EditPolicyFactory() {
}

/**
 * The Destructor.
 */
ProximEditPolicyFactory::~ProximEditPolicyFactory() {
}

/**
 * Creates edit policy for register files.
 */
EditPolicy*
ProximEditPolicyFactory::createRFEditPolicy() {
    return new ProximRFEditPolicy();
}

/**
 * Creates edit policy for function units.
 */
EditPolicy*
ProximEditPolicyFactory::createFUEditPolicy() {
    return new ProximFUEditPolicy();
}

/**
 * Creates edit policy for immediate units.
 */
EditPolicy*
ProximEditPolicyFactory::createIUEditPolicy() {
    return new ProximIUEditPolicy();
}

/**
 * Creates edit policy for control units.
 */
EditPolicy*
ProximEditPolicyFactory::createGCUEditPolicy() {
    return new ProximFUEditPolicy();
}


/**
 * Creates edit policy for sockets.
 */
EditPolicy*
ProximEditPolicyFactory::createSocketEditPolicy() {
    return new ProximSocketEditPolicy();
}

/**
 * Creates edit policy for ports.
 */
EditPolicy*
ProximEditPolicyFactory::createPortEditPolicy() {
    return new ProximPortEditPolicy();
}


/**
 * Creates edit policy for buses.
 */
EditPolicy*
ProximEditPolicyFactory::createBusEditPolicy() {
    return new ProximBusEditPolicy();
}



/**
 * Creates edit policy for bus segments.
 */
EditPolicy*
ProximEditPolicyFactory::createSegmentEditPolicy() {
    return new ProximBusEditPolicy();
}

