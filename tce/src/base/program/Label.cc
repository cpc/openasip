/**
 * @file Label.cc
 *
 * Implementation of Label class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "Label.hh"
#include "Scope.hh"
#include "NullAddressSpace.hh"

using std::string;
using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// Label
/////////////////////////////////////////////////////////////////////////////

Label::Label(): address_(Address(0, NullAddressSpace::instance())) {
}

/**
 * The constructor.
 *
 * Registers this label to the owning scope.
 *
 * @param name Name of the label. Must be unique within the owning scope.
 * @param address The address of the location corresponding to this label.
 * @param scope The innermost scope that contains this label.
 */
Label::Label(const std::string& name, Address address, const Scope& scope):
    name_(name), address_(address), scope_(&scope) {
}

/**
 * The destructor.
 */
Label::~Label() {
}

/**
 * Returns the name of this label.
 *
 * @return The name of this label.
 */
string
Label::name() const {
    return name_;
}

/**
 * Returns the address of the location corresponding to this label.
 *
 * @return The address of the location corresponding to this label.
 */
Address
Label::address() const {
    return address_;
}

/**
 * Returns the innermost scope that contains this label.
 *
 * @return The innermost scope that contains this label.
 */
const Scope&
Label::scope() const {
    return *scope_;
}

/**
 * Sets the name of this label.
 */
void
Label::setName(const string& name) {
    name_ = name;
}

/**
 * Sets the address of the location corresponding to this label.
 */
void
Label::setAddress(Address address) {
    address_ = address;
}

/**
 * Sets the innermost scope that contains this label.
 */
void
Label::setScope(const Scope& scope) {
    scope_ = &scope;
}

}
