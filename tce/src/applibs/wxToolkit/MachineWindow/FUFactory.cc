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
 * @file FUFactory.cc
 *
 * Definition of FUFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
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
