/**
 * @file ProDeIUEditPolicy.cc
 *
 * Definition of ProDeIUEditPolicy class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#include <boost/format.hpp>
#include <string>

#include "ProDeIUEditPolicy.hh"
#include "ImmediateUnit.hh"
#include "Request.hh"
#include "ComponentCommand.hh"
#include "ModifyIUCmd.hh"
#include "DeleteIUCmd.hh"
#include "CopyComponent.hh"
#include "ProDeTextGenerator.hh"
#include "EditPart.hh"
#include "SetStatusTextCmd.hh"
#include "Conversion.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 */
ProDeIUEditPolicy::ProDeIUEditPolicy(): EditPolicy() {
}

/**
 * The Destructor.
 */
ProDeIUEditPolicy::~ProDeIUEditPolicy() {
}

/**
 * Returns the Command corresponding to the type of the Request.
 *
 * @param request Request to be handled.
 * @return NULL if the Request cannot be handled.
 */
ComponentCommand*
ProDeIUEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();

    if (type == Request::MODIFY_REQUEST) {
        ModifyIUCmd* modifyCmd = new ModifyIUCmd(host_);
        return modifyCmd;

    } else if (type == Request::DELETE_REQUEST) {
        DeleteIUCmd* deleteCmd = new DeleteIUCmd(host_);
        return deleteCmd;

    } else if (type == Request::COPY_REQUEST) {
        CopyComponent* copyCmd = new CopyComponent(host_);
        return copyCmd;

    } else if (type == Request::STATUS_REQUEST) {

        ImmediateUnit* iu = dynamic_cast<ImmediateUnit*>(host_->model());
        ProDeTextGenerator* generator = ProDeTextGenerator::instance();
        format fmt = generator->text(
            ProDeTextGenerator::STATUS_IMMEDIATE_UNIT);
        string extension = "";
        if (iu->extensionMode() == Machine::SIGN) {
            extension = generator->text(
                ProDeTextGenerator::TXT_RADIO_EXTENSION_SIGN).str();
        } else if (iu->extensionMode() == Machine::ZERO) {
            extension = generator->text(
                ProDeTextGenerator::TXT_RADIO_EXTENSION_ZERO).str();
        } else {
            // unknown extension
            assert(false);
        }
        fmt % iu->name() % Conversion::toString(iu->numberOfRegisters()) %
            Conversion::toString(iu->width()) % extension;
        SetStatusTextCmd* statusCmd = new SetStatusTextCmd(fmt.str());
        return statusCmd;

    } else {
	return NULL;
    }
}

/**
 * Tells whether this EditPolicy is able to handle a certain type
 * of Request.
 *
 * @param request Request to be asked if it can be handled.
 * @return True if the Request can be handled, false otherwise.
 */
bool
ProDeIUEditPolicy::canHandle(Request* request) const {
    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST ||
	type == Request::DELETE_REQUEST ||
        type == Request::COPY_REQUEST ||
        type == Request::STATUS_REQUEST) {
	return true;
    } else {
	return false;
    }
}
