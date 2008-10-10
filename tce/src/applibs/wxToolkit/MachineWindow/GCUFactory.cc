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
 * @file GCUFactory.cc
 *
 * Definition of GCUFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#include <string>
#include <vector>

#include "WxConversion.hh"
#include "GCUFactory.hh"
#include "UnitPortFactory.hh"
#include "ControlUnit.hh"
#include "EditPart.hh"
#include "UnitFigure.hh"
#include "FUPort.hh"
#include "SpecialRegisterPort.hh"
#include "EditPolicyFactory.hh"
#include "HWOperation.hh"

using std::vector;
using namespace TTAMachine;

/**
 * The Constructor.
 */
GCUFactory::GCUFactory(EditPolicyFactory& editPolicyFactory):
    EditPartFactory(editPolicyFactory) {
    registerFactory(new UnitPortFactory(editPolicyFactory));
}

/**
 * The Destructor.
 */
GCUFactory::~GCUFactory() {
}

/**
 * Returns an EditPart corresponding to a global control unit.
 *
 * @param component Global control unit of which to create the EditPart.
 * @return NULL if the parameter is not an instance of the
 *         ControlUnit class.
 */
EditPart*
GCUFactory::createEditPart(MachinePart* component) {

    ControlUnit* gcu = dynamic_cast<ControlUnit*>(component);

    if (gcu != NULL) {
	EditPart* gcuEditPart = new EditPart();
	gcuEditPart->setModel(gcu);

	UnitFigure* fig = new UnitFigure();
	wxString name = WxConversion::toWxString(gcu->name());
	name.Prepend(_T("GCU: "));
	fig->setName(name);
	gcuEditPart->setFigure(fig);

        // Create editparts of operation ports.
	for (int i = 0; i < gcu->operationPortCount(); i++) {
	    vector<Factory*>::const_iterator iter;
	    for (iter = factories_.begin(); iter != factories_.end(); iter++) {
		EditPart* portEditPart =
		    (*iter)->createEditPart(gcu->operationPort(i));
		if (portEditPart != NULL) {
                    EditPolicy* portEditPolicy =
                        editPolicyFactory_.createFUPortEditPolicy();
                    if (portEditPolicy != NULL) {
                        portEditPart->installEditPolicy(portEditPolicy);
                    }
                    gcuEditPart->addChild(portEditPart);
		}
	    }
	}

        // Create editparts of special register ports.
	for (int i = 0; i < gcu->specialRegisterPortCount(); i++) {
	    vector<Factory*>::const_iterator iter;
	    for (iter = factories_.begin(); iter != factories_.end(); iter++) {
		EditPart* portEditPart =
		    (*iter)->createEditPart(gcu->specialRegisterPort(i));
		if (portEditPart != NULL) {
                    EditPolicy* portEditPolicy =
                        editPolicyFactory_.createSRPortEditPolicy();
                    if (portEditPolicy != NULL) {
                        portEditPart->installEditPolicy(portEditPolicy);
                    }
		    gcuEditPart->addChild(portEditPart);
		}
	    }
	}

        wxString operations = _T("{ ");
	for (int i = 0; i < gcu->operationCount(); i++) {
            if (i > 0) {
                operations.Append(_T(", "));
            }
            operations.Append(
                WxConversion::toWxString(gcu->operation(i)->name()));
        }
        operations.Append(_T(" }"));

        fig->setInfo(operations);

	gcuEditPart->setSelectable(true);

	EditPolicy* editPolicy = editPolicyFactory_.createGCUEditPolicy();
	if (editPolicy != NULL) {
	    gcuEditPart->installEditPolicy(editPolicy);
	}

	return gcuEditPart;

    } else {
	return NULL;
    } 
}
