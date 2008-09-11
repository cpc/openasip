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
