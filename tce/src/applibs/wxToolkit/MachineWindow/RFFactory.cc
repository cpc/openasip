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
 * @file RFFactory.cc
 *
 * Definition of RFFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#include <string>
#include <vector>

#include "RFFactory.hh"
#include "UnitPortFactory.hh"
#include "RegisterFile.hh"
#include "EditPart.hh"
#include "UnitFigure.hh"
#include "WxConversion.hh"
#include "FUPort.hh"
#include "EditPolicyFactory.hh"
#include "UnboundedRegisterFile.hh"

using std::vector;
using namespace TTAMachine;

/**
 * The Constructor.
 */
RFFactory::RFFactory(EditPolicyFactory& editPolicyFactory):
    EditPartFactory(editPolicyFactory) {

    registerFactory(new UnitPortFactory(editPolicyFactory));
}

/**
 * The Destructor.
 */
RFFactory::~RFFactory() {
}

/**
 * Returns an EditPart corresponding to a register file.
 *
 * @param component Register file of which to create the EditPart.
 * @return NULL if the parameter is not an instance of the
 *         RegisterFile class.
 */
EditPart*
RFFactory::createEditPart(MachinePart* component) {

    RegisterFile* rf = dynamic_cast<RegisterFile*>(component);

    if (rf != NULL) {
	EditPart* rfEditPart = new EditPart();
	rfEditPart->setModel(rf);

	UnitFigure* fig = new UnitFigure();
	wxString name = WxConversion::toWxString(rf->name().c_str());
	name.Prepend(_T("RF: "));
	fig->setName(name);
	rfEditPart->setFigure(fig);

	for (int i = 0; i < rf->portCount(); i++) {
	    vector<Factory*>::const_iterator iter;
	    for (iter = factories_.begin(); iter != factories_.end(); iter++) {
		EditPart* portEditPart =
		    (*iter)->createEditPart(rf->port(i));
		if (portEditPart != NULL) {

		    EditPolicy* editPolicy =
			editPolicyFactory_.createRFPortEditPolicy();

		    if (editPolicy != NULL) {
			portEditPart->installEditPolicy(editPolicy);
		    }

		    rfEditPart->addChild(portEditPart);
		}
	    }
	}

        wxString info;
        if (dynamic_cast<UnboundedRegisterFile*>(rf) != NULL) {
            info.Append(_T("?"));
        } else {
            info.Append(WxConversion::toWxString(rf->numberOfRegisters()));
        }
        info.Append(_T("x"));
        info.Append(WxConversion::toWxString(rf->width()));

        fig->setInfo(info);

	rfEditPart->setSelectable(true);

	EditPolicy* editPolicy = editPolicyFactory_.createRFEditPolicy();
	if (editPolicy != NULL) {
	    rfEditPart->installEditPolicy(editPolicy);
	}

	return rfEditPart;

    } else {
	return NULL;
    } 
}
