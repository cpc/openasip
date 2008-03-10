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
