/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
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
