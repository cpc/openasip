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
 * @file RFFactory.cc
 *
 * Definition of RFFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
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
