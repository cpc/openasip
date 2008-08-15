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
 * @file FUFactory.cc
 *
 * Definition of FUFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#include <vector>

#include "WxConversion.hh"
#include "FUFactory.hh"
#include "UnitPortFactory.hh"
#include "FunctionUnit.hh"
#include "EditPart.hh"
#include "UnitFigure.hh"
#include "FUPort.hh"
#include "EditPolicyFactory.hh"
#include "HWOperation.hh"

using std::vector;
using namespace TTAMachine;

/**
 * The Constructor.
 */
FUFactory::FUFactory(EditPolicyFactory& editPolicyFactory):
    EditPartFactory(editPolicyFactory) {

    registerFactory(new UnitPortFactory(editPolicyFactory));
}


/**
 * The Destructor.
 */
FUFactory::~FUFactory() {
}


/**
 * Returns an EditPart corresponding to a function unit.
 *
 * @param component Function unit of which to create the EditPart.
 * @return NULL if the parameter is not an instance of the
 *         FunctionUnit class.
 */
EditPart*
FUFactory::createEditPart(MachinePart* component) {

    FunctionUnit* fu = dynamic_cast<FunctionUnit*>(component);

    if (fu != NULL) {
	EditPart* fuEditPart = new EditPart();
	fuEditPart->setModel(fu);

	UnitFigure* fig = new UnitFigure();
	wxString name = WxConversion::toWxString(fu->name());
	name.Prepend(_T("FU: "));
	fig->setName(name);
	fuEditPart->setFigure(fig);

	for (int i = 0; i < fu->portCount(); i++) {
	    vector<Factory*>::const_iterator iter;
	    for (iter = factories_.begin(); iter != factories_.end(); iter++) {
		EditPart* portEditPart =
		    (*iter)->createEditPart(fu->port(i));
		if (portEditPart != NULL) {
		    fuEditPart->addChild(portEditPart);
		    EditPolicy* editPolicy =
			editPolicyFactory_.createFUPortEditPolicy();
		    if (editPolicy != NULL) {
			portEditPart->installEditPolicy(editPolicy);
		    }
		}
	    }
	}

        wxString operations = _T("{ ");
	for (int i = 0; i < fu->operationCount(); i++) {
            if (i > 0) {
                operations.Append(_T(", "));
            }
            operations.Append(
                WxConversion::toWxString(fu->operation(i)->name()));
        }
        operations.Append(_T(" }"));
        fig->setInfo(operations);

	fuEditPart->setSelectable(true);

	EditPolicy* editPolicy = editPolicyFactory_.createFUEditPolicy();
	if (editPolicy != NULL) {
	    fuEditPart->installEditPolicy(editPolicy);
	}

	return fuEditPart;

    } else {
	return NULL;
    } 
}
