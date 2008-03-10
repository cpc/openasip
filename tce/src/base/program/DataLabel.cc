/**
 * @file DataLabel.cc
 *
 * Implementation of DataLabel class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "DataLabel.hh"
#include "Scope.hh"

using std::string;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// DataLabel
/////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * Registers this label to the owning scope.
 *
 * @param name Name of the label. Must be unique within the owning scope.
 * @param address The address of the location corresponding to this label.
 * @param scope The innermost scope that contains this label.
 */
DataLabel::DataLabel(
    const std::string& name, Address address, const Scope& scope) :
    Label(name, address, scope) {
}

/**
 * The destructor.
 */
DataLabel::~DataLabel() {
}

}
