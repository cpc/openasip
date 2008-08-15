/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ProDeEditPolicyFactory.cc
 *
 * Implementation of ProDeEditPolicyFactory class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProDeEditPolicyFactory.hh"

#include "ProDeFUEditPolicy.hh"
#include "ProDeRFEditPolicy.hh"
#include "ProDeIUEditPolicy.hh"
#include "ProDeGCUEditPolicy.hh"
#include "ProDePortEditPolicy.hh"
#include "ProDeBusEditPolicy.hh"
#include "ProDeSocketEditPolicy.hh"
#include "ProDeSegmentEditPolicy.hh"
#include "ProDeBridgeEditPolicy.hh"
#include "ProDeFUPortEditPolicy.hh"
#include "ProDeRFPortEditPolicy.hh"
#include "ProDeIUPortEditPolicy.hh"
#include "ProDeSRPortEditPolicy.hh"

/**
 * The Constructor.
 */
ProDeEditPolicyFactory::ProDeEditPolicyFactory() {
}

/**
 * The Destructor.
 */
ProDeEditPolicyFactory::~ProDeEditPolicyFactory() {
}

/**
 * Returns edit policy for function units.
 *
 * @return Function unit edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createFUEditPolicy() {
    return new ProDeFUEditPolicy();
}

/**
 * Returns edit policy for register files.
 *
 * @return Register file edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createRFEditPolicy() {
    return new ProDeRFEditPolicy();
}

/**
 * Returns edit policy for immediate units.
 *
 * @return Immediate unit edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createIUEditPolicy() {
    return new ProDeIUEditPolicy();
}

/**
 * Returns edit policy for global control unit.
 *
 * @return Global control unit edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createGCUEditPolicy() {
    return new ProDeGCUEditPolicy();
}

/**
 * Returns edit policy for function unit ports.
 *
 * @return Function unit port edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createFUPortEditPolicy() {
    return new ProDeFUPortEditPolicy();
}

/**
 * Returns edit policy for register file ports.
 *
 * @return Register file port edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createRFPortEditPolicy() {
    return new ProDeRFPortEditPolicy();
}

/**
 * Returns edit policy for immediate unit ports.
 *
 * @return Immediate unit port edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createIUPortEditPolicy() {
    return new ProDeIUPortEditPolicy();
}

/**
 * Returns edit policy for special register ports.
 *
 * @return Special register port edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createSRPortEditPolicy() {
    return new ProDeSRPortEditPolicy();
}

/**
 * Returns edit policy for ports.
 *
 * @return Port edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createPortEditPolicy() {
    return new ProDePortEditPolicy();
}

/**
 * Returns edit policy for buses.
 *
 * @return Bus edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createBusEditPolicy() {
    return new ProDeBusEditPolicy();
}

/**
 * Returns edit policy for sockets.
 *
 * @return Socket edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createSocketEditPolicy() {
    return new ProDeSocketEditPolicy();
}

/**
 * Returns edit policy for bus segments.
 *
 * @return Segment edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createSegmentEditPolicy() {
    return new ProDeSegmentEditPolicy();
}

/**
 * Returns edit policy for bridgs.
 *
 * @return Bridge edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createBridgeEditPolicy() {
    return new ProDeBridgeEditPolicy();
}
