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
 * @file GlobalScope.cc
 *
 * Implementation of GlobalScope class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "GlobalScope.hh"
#include "CodeLabel.hh"
#include "DataLabel.hh"
#include "SequenceTools.hh"
#include "Program.hh"
#include "InstructionReference.hh"
#include "InstructionReferenceManager.hh"

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// GlobalScope
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
GlobalScope::GlobalScope() {
}

/**
 * Destructor.
 */
GlobalScope::~GlobalScope() {
    SequenceTools::deleteAllItems(allCodeLabels_);
    SequenceTools::deleteAllItems(allDataLabels_);
}

/**
 * Tells whether this is a global scope.
 *
 * @return True always.
 */
bool
GlobalScope::isGlobal() const {
    return true;
}

/**
 * Adds a code label to the global bookkeeping.
 *
 * @param codeLabel The label to be added.
 * @param owner The owner scope of the label.
 */
void
GlobalScope::addGlobalCodeLabel(
    const CodeLabel& codeLabel, const Scope& owner) {

    allCodeLabels_.push_back(new LabelOwner(codeLabel, owner));
}

/**
 * Returns the number of code labels at the given address.
 *
 * Applies to all child scopes of this global scope.
 *
 * @param address The address of the label(s).
 * @return The number of code labels at the given address.
 */
int
GlobalScope::globalCodeLabelCount(Address address) const {
    int count = 0;
    for (unsigned int i = 0; i < allCodeLabels_.size(); i++) {
        if (allCodeLabels_.at(i)->label().address().location() ==
            address.location() &&
            &allCodeLabels_.at(i)->label().address().space() ==
            &address.space()) {
            count++;
        }
    }
    return count;
}

/**
 * Returns the code label in the given address.
 *
 * Applies to all child scopes of this global scope.
 *
 * @param address The address of the label.
 * @param index The index of the label if there are many labels at the
 *              address.
 * @exception KeyNotFound If no code labels in the given address were found.
 * @return The code label in the given address.
 */
const CodeLabel&
GlobalScope::globalCodeLabel(Address address, int index) const
    throw (KeyNotFound) {

    int found = 0;

    for (unsigned int i = 0; i < allCodeLabels_.size(); i++) {

        if (allCodeLabels_.at(i)->label().address().location() ==
            address.location() &&
            &allCodeLabels_.at(i)->label().address().space() ==
            &address.space()) {

            if (index == found) {
                return dynamic_cast<const CodeLabel&>(
                    allCodeLabels_.at(i)->label());
            } else {
                found++;
            }
        }
    }

    throw KeyNotFound(__FILE__, __LINE__);
}

/**
 * Returns the number of code labels in program.
 *
 * Applies to all child scopes of this global scope.
 *
 * @return The number of code labels in program.
 */
int
GlobalScope::globalCodeLabelCount() const {
    return allCodeLabels_.size();
}

/**
 * Returns the code label of requested index.
 *
 * Applies to all child scopes of this global scope.
 *
 * @param index The index of the label.
 * @exception KeyNotFound If no code labels in the given index.
 * @return The code label in the given index.
 */
const CodeLabel&
GlobalScope::globalCodeLabel(int index) const
    throw (KeyNotFound) {

    if (index < globalCodeLabelCount()) {
        return dynamic_cast<const CodeLabel&>(
            allCodeLabels_.at(index)->label());
    }

    throw KeyNotFound(__FILE__, __LINE__);
}

/**
 * Adds a data label to the global bookkeeping.
 *
 * Applies to all child scopes of this global scope.
 *
 * @param dataLabel The label to be added.
 * @param owner The owner scope of the label.
 */
void
GlobalScope::addGlobalDataLabel(
    const DataLabel& dataLabel, const Scope& owner) {

    allDataLabels_.push_back(new LabelOwner(dataLabel, owner));
}

/**
 * Returns the number of data labels at the given address.
 *
 * Applies to all child scopes of this global scope.
 *
 * @param address The address of the label(s).
 * @return The number of data labels at the given address.
 */
int
GlobalScope::globalDataLabelCount(Address address) const {
    int count = 0;
    for (unsigned int i = 0; i < allDataLabels_.size(); i++) {
        if (allDataLabels_.at(i)->label().address().location() ==
            address.location() &&
            &allDataLabels_.at(i)->label().address().space() ==
            &address.space()) {
            count++;
        }
    }
    return count;
}

/**
 * Returns the data label in the given address.
 *
 * Applies to all child scopes of this global scope.
 *
 * @param address The address of the label.
 * @param index The index of the label if there are many labels at the
 *              address.
 * @exception KeyNotFound If no data labels in the given address were found.
 * @return The data label in the given address.
 */
const DataLabel&
GlobalScope::globalDataLabel(Address address, int index) const
    throw (KeyNotFound) {

    int found = 0;

    for (unsigned int i = 0; i < allDataLabels_.size(); i++) {

        if (allDataLabels_.at(i)->label().address().location() ==
            address.location() &&
            &allDataLabels_.at(i)->label().address().space() ==
            &address.space()) {

            if (index == found) {
                return dynamic_cast<const DataLabel&>(
                    allDataLabels_.at(i)->label());
            } else {
                found++;
            }
        }
    }

    throw KeyNotFound(__FILE__, __LINE__);
}

/**
 * Returns the number of data labels in the program.
 *
 * Applies to all child scopes of this global scope.
 *
 * @return The number of data labels in the program.
 */
int
GlobalScope::globalDataLabelCount() const {
    return allDataLabels_.size();
}

/**
 * Returns the data label of given index.
 *
 * Applies to all child scopes of this global scope.
 *
 * @param index The index of the label.
 * @exception KeyNotFound If no data labels with given index.
 * @return The data label of given index.
 */
const DataLabel&
GlobalScope::globalDataLabel(int index) const
    throw (KeyNotFound) {

    if (index < globalDataLabelCount()) {
        return dynamic_cast<const DataLabel&>(
            allDataLabels_.at(index)->label());
    }

    throw KeyNotFound(__FILE__, __LINE__);
}

/**
 * Creates a copy of the scope and its labels.
 *
 * The targets of the labels are not converted to point to the instructions of
 * the given program and data of the data sections.
 *
 * @return A copy of the scope.
 */
Scope*
GlobalScope::copy() const {

    GlobalScope* newScope = new GlobalScope();
    for (unsigned int i = 0; i < allDataLabels_.size(); ++i) {
        const DataLabel& source = dynamic_cast<const DataLabel&>(
            allDataLabels_.at(i)->label());
        newScope->addDataLabel(new DataLabel(source));
    }

    for (unsigned int i = 0; i < allCodeLabels_.size(); ++i) {
        const CodeLabel& source = dynamic_cast<const CodeLabel&>(
            allCodeLabels_.at(i)->label());
        newScope->addCodeLabel(new CodeLabel(source));
    }
    return newScope;
}

/**
 * Sets the address space of all data labels.
 *
 * @param space The address space.
 */
void
GlobalScope::setDataLabelAddressSpace(
    const TTAMachine::AddressSpace& space) {

    LabelOwnerList newDataLabels_;
    for (int i = 0; i < globalDataLabelCount(); ++i) {
        const DataLabel& source = globalDataLabel(i);

        LabelOwner* labelOwner = new LabelOwner(
            *(new DataLabel(
                source.name(), Address(source.address().location(), space), 
                *this)), *this);
        newDataLabels_.push_back(labelOwner);
    }    
    SequenceTools::deleteAllItems(allDataLabels_);
    allDataLabels_ = newDataLabels_;
}

/**
 * Creates a copy of the scope and its labels.
 *
 * The targets of the labels are converted to point to the instructions of
 * the given program. The data labels still point to the old address spaces,
 * assuming the target machine is still the same.
 *
 * @param program The Program containing the instructions code labels should
 *        be converted to point to.
 * @return A copy of the scope.
 */
Scope*
GlobalScope::copyAndRelocate(const TTAProgram::Program& program)
    const {

    GlobalScope* newScope = new GlobalScope();
    for (unsigned int i = 0; i < allDataLabels_.size(); ++i) {
        const Label& source = allDataLabels_.at(i)->label();
        newScope->addDataLabel(new DataLabel(
                                   source.name(), source.address(), 
                                   *newScope));
    }

    for (unsigned int i = 0; i < allCodeLabels_.size(); ++i) {
        const CodeLabel& source = dynamic_cast<const CodeLabel&>(
            allCodeLabels_.at(i)->label());
        newScope->addCodeLabel(
            new CodeLabel(
                program.instructionReferenceManager().createReference(
                    program.instructionAt(source.address().location())),
                source.name()));
    }
    return newScope;
}

/**
 * Removes all code labels attached to the given instruction address.
 *
 * @param address The instruction address.
 */
void
GlobalScope::removeCodeLabels(InstructionAddress address) {

    for (LabelOwnerList::iterator i = allCodeLabels_.begin();
         i != allCodeLabels_.end(); ) {
        if ((*i)->label().address().location() == address) {
            delete (*i);
            i = allCodeLabels_.erase(i); //returns the following element or end
        } else {
            i++; // advance to next
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// LabelOwner
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param label The label.
 * @param owner The owner scope of the label.
 */
GlobalScope::LabelOwner::LabelOwner(const Label& label, const Scope& owner):
    label_(&label), owner_(&owner) {
}

/**
 * Destructor.
 *
 * @note Is this really the owner of the label? Then it should delete it!
 */
GlobalScope::LabelOwner::~LabelOwner() {
}

/**
 * Returns the label.
 *
 * @return The label.
 */
const Label&
GlobalScope::LabelOwner::label() const {
    return *label_;
}

/**
 * Returns the owning scope of the label.
 *
 * @return The owning scope.
 */
const Scope&
GlobalScope::LabelOwner::scope() const {
    return *owner_;
}

}
