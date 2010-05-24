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
 * @file Scope.cc
 *
 * Implementation of Scope class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "Scope.hh"
#include "CodeLabel.hh"
#include "DataLabel.hh"
#include "ContainerTools.hh"
#include "Application.hh"

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// Scope
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
Scope::Scope(): parent_(NULL) {
}

/**
 * Destructor.
 */
Scope::~Scope() {
    for (unsigned int i = 0; i < dataLabels_.size(); i++) {
        delete dataLabels_.at(i);
    }
    dataLabels_.clear();

    for (unsigned int i = 0; i < codeLabels_.size(); i++) {
        delete codeLabels_.at(i);
    }
    codeLabels_.clear();
}

/**
 * Tells whether this is a global scope.
 *
 * @return True if this is a global scope.
 */
bool
Scope::isGlobal() const {
    return false;
}

/**
 * Tells whether this is a unit scope.
 *
 * @return True if this is a unit scope.
 */
bool
Scope::isUnit() const {
    return false;
}

/**
 * Tells whether this is a procedure scope.
 *
 * @return True if this is a procedure scope.
 */
bool
Scope::isProcedure() const {
    return false;
}

/**
 * Tells whether this is a local scope.
 *
 * @return True if this is a local scope.
 */
bool
Scope::isLocal() const {
    return false;
}

/**
 * Adds a child scope to this scope.
 *
 * @param scope The child scope to be added.
 * @exception ObjectAlreadyExists if the given scope has already been added.
 *            or a scope with the same name already exists.
 */
void
Scope::addChild(const Scope& scope) throw (ObjectAlreadyExists) {
    if (!ContainerTools::containsValue(children_, &scope)) {
        children_.push_back(&scope);
    } else {
        throw ObjectAlreadyExists(__FILE__, __LINE__);
    }
}

/**
 * Return the parent scope., that is, the scope that contains this scope.
 *
 * @return The parent scope, that is, the scope that contains this scope.
 * @exception WrongSubclass if this scope has no parent scope (it is the
 *                          global scope).
 * @exception IllegalRegistration if no parent has been set (and the scope
 *                                is not the global scope.
 */
Scope&
Scope::parent() const
    throw (WrongSubclass, IllegalRegistration) {

    if (isGlobal()) {
        throw WrongSubclass(
            __FILE__, __LINE__, "Scope::parent()",
            "Global scope cannot have parent scope.");
    } else if (parent_ == NULL) {
        throw IllegalRegistration(
            __FILE__, __LINE__, "Scope::parent()", "No parent set.");
    } else {
        return *parent_;
    }
}

/**
 * Sets the parent scope.
 *
 * @param scope The new parent scope.
 * @exception WrongSubclass if the scope is the global scope.
 */
void
Scope::setParent(Scope& scope)
    throw (WrongSubclass) {

    if (!isGlobal()) {
        parent_ = &scope;
    } else {
        throw WrongSubclass(
            __FILE__, __LINE__, "Scope::parent()",
            "Global scope cannot have parent scope.");
    }
}

/**
 * Returns the number of contained scopes.
 *
 * Only scopes directly contained in this scope are counted, not inner
 * scopes inside one of the child scopes.
 *
 * @return The number of contained scopes.
 */
int
Scope::childCount() const {
    return children_.size();
}

/**
 * Returns the child scope at given index.
 *
 * @param index The index of the child scope.
 * @return The child scope at given index.
 * @exception OutOfRange if the exceeds the number of scopes contained in
 *                       this scope.
 */
const Scope&
Scope::child(int index) const
    throw (OutOfRange) {

    if (index >= 0 && static_cast<unsigned int>(index) < children_.size()) {
        return *children_.at(index);
    } else {
        throw OutOfRange(__FILE__, __LINE__);
    }
}

/**
 * Tells whether this scope contains a code label with the given name.
 *
 * @param The name of the label.
 * @return True if this scope contains a code label with the given name.
 */
bool
Scope::containsCodeLabel(const std::string& name) const {
    for (unsigned int i = 0; i < codeLabels_.size(); i++) {
        if (name == codeLabels_.at(i)->name()) {
            return true;
        }
    }
    return false;
}

/**
 * Tells whether this scope contains a data label with the given name.
 *
 * @param The name of the label.
 * @return True if this scope contains a data label with the given name.
 */
bool
Scope::containsDataLabel(const std::string& name) const {
    for (unsigned int i = 0; i < dataLabels_.size(); i++) {
        if (name == dataLabels_.at(i)->name()) {
            return true;
        }
    }
    return false;
}

/**
 * Returns the code label with the given name.
 *
 * @param name The name of the label.
 * @exception KeyNotFound if no code label with the given name exists in this
 * scope.
 * @return The code label with the given name.
 */
const CodeLabel&
Scope::codeLabel(const std::string& name) const throw (KeyNotFound) {
    for (unsigned int i = 0; i < codeLabels_.size(); i++) {
        if (name == codeLabels_.at(i)->name()) {
            return *codeLabels_.at(i);
        }
    }
    throw KeyNotFound(__FILE__, __LINE__);
}

/**
 * Returns the data label with the given name.
 *
 * @param name The name of the label.
 * @exception KeyNotFound if no data label with the given name exists in this
 * scope.
 * @return The data label with the given name.
 */
const DataLabel&
Scope::dataLabel(const std::string& name) const throw (KeyNotFound) {
    for (unsigned int i = 0; i < dataLabels_.size(); i++) {
        if (name == dataLabels_.at(i)->name()) {
            return *dataLabels_.at(i);
        }
    }
    throw KeyNotFound(__FILE__, __LINE__);
}

/**
 * Returns the number of code labels in the given address visible in this
 * scope.
 *
 * @param address The address of the label.
 * @return The number of code labels in the given address visible in this
 *         scope.
 */
int
Scope::codeLabelCount(Address address) const {
    int count = 0;
    for (unsigned int i = 0; i < codeLabels_.size(); i++) {
        if (&codeLabels_.at(i)->address().space() == &address.space() &&
            codeLabels_.at(i)->address().location() == address.location()) {
            count++;
        }
    }
    return count;
}

/**
 * Returns a code label visible in this scope in the given address and index.
 *
 * @param address The address of the code label.
 * @param index The index of the label if there area many labels at this
 *              address.
 * @return A code label visible in this scope in the given address and index.
 */
const CodeLabel&
Scope::codeLabel(Address address, int index) const
    throw (OutOfRange, KeyNotFound) {

    if (index < codeLabelCount(address)) {

        int found = -1;
        for (unsigned int i = 0; i < codeLabels_.size(); i++) {

            if (&codeLabels_.at(i)->address().space() ==
                &address.space() &&
                codeLabels_.at(i)->address().location() ==
                address.location()) {

                found++;
                if (found == index) {
                    return *codeLabels_.at(i);
                }
            }
        }

        throw KeyNotFound(__FILE__, __LINE__);

    } else {
        throw OutOfRange(__FILE__, __LINE__);
    }
}

/**
 * Returns the number of data labels in the given address visible in this
 * scope.
 *
 * @param address The address of the label.
 * @return The number of data labels in the given address visible in this
 *         scope.
 */
int
Scope::dataLabelCount(Address address) const {
    int count = 0;
    for (unsigned int i = 0; i < dataLabels_.size(); i++) {
        if (&dataLabels_.at(i)->address().space() == &address.space() &&
            dataLabels_.at(i)->address().location() == address.location()) {
            count++;
        }
    }
    return count;
}

/**
 * Returns a data label visible in this scope in the given address and index.
 *
 * @param address The address of the data label.
 * @param index The index of the label if there area many labels at this
 *              address.
 * @return A data label visible in this scope in the given address and index.
 */
const DataLabel&
Scope::dataLabel(Address address, int index) const
    throw (OutOfRange, KeyNotFound) {

    if (index < dataLabelCount(address)) {

        int found = -1;
        for (unsigned int i = 0; i < dataLabels_.size(); i++) {

            if (&dataLabels_.at(i)->address().space() ==
                &address.space() &&
                dataLabels_.at(i)->address().location() ==
                address.location()) {

                found++;
                if (found == index) {
                    return *dataLabels_.at(i);
                }
            }
        }

        throw KeyNotFound(__FILE__, __LINE__);

    } else {
        throw OutOfRange(__FILE__, __LINE__);
    }
}

/**
 * Adds a code label to this scope.
 *
 * @param codeLabel The code label to be added.
 * @exception KeyAlreadyExists of a code label of the same name already
 *            exists.
 */
void
Scope::addCodeLabel(const CodeLabel* codeLabel) throw (KeyAlreadyExists) {

    if (containsCodeLabel(codeLabel->name())) {
        throw KeyAlreadyExists(
            __FILE__, __LINE__, "Scope::addCodeLabel()",
            "A code label already exists by this name.");

    } else {
        codeLabels_.push_back(codeLabel);

        // add label to the global bookkeeping
        Scope* scope = NULL;

        if (!isGlobal()) {
            scope = &parent();
            while (!scope->isGlobal()) {
                try {
                    scope = &scope->parent();
                } catch (WrongSubclass) {
                    assert(false);
                } catch (IllegalRegistration) {
                    assert(false);
                }
            }
        } else {
            scope = this;
        }

        scope->addGlobalCodeLabel(*codeLabel, *this);
    }
}

/**
 * Adds a data label to this scope.
 *
 * @param dataLabel The data label to be added.
 * @exception KeyAlreadyExists of a data label of the same name already
 *            exists.
 */
void
Scope::addDataLabel(const DataLabel* dataLabel) throw (KeyAlreadyExists) {
    if (containsDataLabel(dataLabel->name())) {
        throw KeyAlreadyExists(
            __FILE__, __LINE__, "Scope::addDataLabel()",
            "A data label already exists by this name.");
    } else {
        dataLabels_.push_back(dataLabel);

        // add label to the global bookkeeping
        Scope* scope = NULL;

        if (!isGlobal()) {
            scope = &parent();
            while (!scope->isGlobal()) {
                try {
                    scope = &scope->parent();
                } catch (WrongSubclass) {
                    assert(false);
                } catch (IllegalRegistration) {
                    assert(false);
                }
            }
        } else {
            scope = this;
        }

        scope->addGlobalDataLabel(*dataLabel, *this);
    }
}

/**
 * Removes all code labels attached to the given instruction address.
 *
 * @param address The instruction address.
 */
void
Scope::removeCodeLabels(InstructionAddress address) {

    for (CodeLabelList::iterator i = codeLabels_.begin();
         i != codeLabels_.end(); ) {
        CodeLabelList::iterator next = i;
        next++;
        if ((*i)->address().location() == address) {
            delete (*i);
            codeLabels_.erase(i);
        }
        i = next;
    }
}

/**
 * Removes data label at the given index.
 *
 * @param address The data label index.
 */
void
Scope::removeDataLabels(const int index) {
    
    if (index >= 0 && index < dataLabels_.size()) {
        DataLabelList::iterator i = dataLabels_.begin();
        i += index;
        delete(*i);
        dataLabels_.erase(i);
    }
}
    
}
