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
 * @file ProDeEditPolicyFactory.hh
 *
 * Declaration of ProDeEditPolicyFactory class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
