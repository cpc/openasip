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
 * @file ProximEditPolicyFactory.cc
 *
 * Implementation of ProximEditPolicyFactory class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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

