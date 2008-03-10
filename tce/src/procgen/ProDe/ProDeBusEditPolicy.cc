/**
 * @file ProDeBusEditPolicy.cc
 *
 * Definition of ProDeBusEditPolicy class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#include <string>
#include <boost/format.hpp>

#include "ProDeBusEditPolicy.hh"
#include "Request.hh"
#include "ComponentCommand.hh"
#include "SetStatusTextCmd.hh"
#include "ModifyBusCmd.hh"
#include "DeleteBusCmd.hh"
#include "CopyComponent.hh"
#include "EditPart.hh"
#include "MachinePart.hh"
#include "Bus.hh"
#include "ProDeTextGenerator.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 */
ProDeBusEditPolicy::ProDeBusEditPolicy(): EditPolicy() {
}

/**
 * The Destructor.
 */
ProDeBusEditPolicy::~ProDeBusEditPolicy() {
}

/**
 * Returns the Command corresponding to the type of the Request.
 *
 * @param request Request to be handled.
 * @return NULL if the Request cannot be handled.
 */
ComponentCommand*
ProDeBusEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();

    if (type == Request::MODIFY_REQUEST) {
        ModifyBusCmd* modifyCmd = new ModifyBusCmd(host_);
        return modifyCmd;

    } else if (type == Request::DELETE_REQUEST) {
        DeleteBusCmd* deleteCmd = new DeleteBusCmd(host_);
        return deleteCmd;

    } else if (type == Request::COPY_REQUEST) {
        CopyComponent* copyCmd = new CopyComponent(host_);
        return copyCmd;

    } else if (type == Request::STATUS_REQUEST) {
        Bus* bus = dynamic_cast<Bus*>(host_->model());
        ProDeTextGenerator* generator = ProDeTextGenerator::instance();
        format fmt = generator->text(ProDeTextGenerator::STATUS_BUS);
        string extension = "";
        if (bus->signExtends()) {
            extension = generator->text(
                ProDeTextGenerator::TXT_RADIO_EXTENSION_SIGN).str();
        } else {
            extension = generator->text(
                ProDeTextGenerator::TXT_RADIO_EXTENSION_ZERO).str();
        }
        fmt % bus->name() % Conversion::toString(bus->width()) %
            Conversion::toString(bus->immediateWidth()) %
            extension;
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
ProDeBusEditPolicy::canHandle(Request* request) const {
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
