/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file IUFactory.cc
 *
 * Definition of IUFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#include <string>
#include <vector>

#include "IUFactory.hh"
#include "UnitPortFactory.hh"
#include "ImmediateUnit.hh"
#include "EditPart.hh"
#include "UnitFigure.hh"
#include "FUPort.hh"
#include "WxConversion.hh"
#include "EditPolicyFactory.hh"

using std::vector;
using namespace TTAMachine;

/**
 * The Constructor.
 */
IUFactory::IUFactory(EditPolicyFactory& editPolicyFactory):
    EditPartFactory(editPolicyFactory) {

    registerFactory(new UnitPortFactory(editPolicyFactory));
}

/**
 * The Destructor.
 */
IUFactory::~IUFactory() {
}

/**
 * Returns an EditPart corresponding to a immediate unit.
 *
 * @param component Immediate unit of which to create the EditPart.
 * @return NULL if the parameter is not an instance of the
 *         ImmediateUnit class.
 */
EditPart*
IUFactory::createEditPart(MachinePart* component) {

    ImmediateUnit* imm = dynamic_cast<ImmediateUnit*>(component);

    if (imm != NULL) {
	EditPart* immEditPart = new EditPart();
	immEditPart->setModel(imm);

	UnitFigure* fig = new UnitFigure();
	wxString name = WxConversion::toWxString(imm->name());
	name.Prepend(_T("IMM: "));
	fig->setName(name);
	immEditPart->setFigure(fig);

	for (int i = 0; i < imm->portCount(); i++) {
	    vector<Factory*>::const_iterator iter;
	    for (iter = factories_.begin(); iter != factories_.end(); iter++) {
		EditPart* portEditPart =
		    (*iter)->createEditPart(imm->port(i));
		if (portEditPart != NULL) {
		    EditPolicy* editPolicy =
			editPolicyFactory_.createIUPortEditPolicy();
		    if (editPolicy != NULL) {
			portEditPart->installEditPolicy(editPolicy);
		    }
		    immEditPart->addChild(portEditPart);
		}
	    }
	}


        wxString info = WxConversion::toWxString(imm->numberOfRegisters());
        info.Append(_T("x"));
        info.Append(WxConversion::toWxString(imm->width()));
        fig->setInfo(info);
	immEditPart->setSelectable(true);

	EditPolicy* editPolicy = editPolicyFactory_.createIUEditPolicy();
	if (editPolicy != NULL) {
	    immEditPart->installEditPolicy(editPolicy);
	}

	return immEditPart;

    } else {
	return NULL;
    } 
}
