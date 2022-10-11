/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file MinimalOpSetCheck.cc
 *
 * Implementation of MinimalOpSetCheck class.
 *
 * @author Esa Määttä 2008 (esa.maatta-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2010
 * @note rating: red
 */

#include <set>
#include <vector>
#include <string>

#include "MinimalOpSetCheck.hh"
#include "FunctionUnit.hh"
#include "Machine.hh"
#include "Environment.hh"
#include "FullyConnectedCheck.hh"
#include "CIStringSet.hh"
#include "MachineCheckResults.hh"

MinimalOpSetCheck::MinimalOpSetCheck() : 
    MachineCheck("Common helper functionality for minimal opset checks.") {
    buildMinimalOpSet();
}

MinimalOpSetCheck::~MinimalOpSetCheck() {
}

/**
 * Checks if machine has all operations in minimal opset.
 *
 * @param machine Machine to be checked against minimal opset.
 * @return True if minimal operation set was met, false otherwise.
 */
bool 
MinimalOpSetCheck::check(const TTAMachine::Machine& machine) const {
    TTAMachine::Machine::FunctionUnitNavigator fuNav = 
        machine.functionUnitNavigator();
    TCETools::CIStringSet opSet;
    // construct the opset list
    for (int i = 0; i < fuNav.count(); i++) {
        TTAMachine::FunctionUnit* fu = fuNav.item(i);
        fu->operationNames(opSet);
    }

    // if machines opset is smaller than required opset
    if (opSet.size() < minimalOpSet_.size()) {
        return false;
    }

    TCETools::CIStringSet::const_iterator first1 = minimalOpSet_.begin();
    TCETools::CIStringSet::const_iterator last1 = minimalOpSet_.end();

    TCETools::CIStringSet::iterator first2 = opSet.begin();
    TCETools::CIStringSet::iterator last2 = opSet.end();

    // return false if missing operation was found
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            return false;
        } else if (*first2 < *first1) {
            ++first2;
        } else { 
            ++first1; 
            ++first2; 
        }
    }
    if (first1 != last1) {
        return false;
    }
    return true;
}


/**
 * Checks the machine if it misses operations from the minimal op set.
 *
 * @param results Results of the validation are added to the given instance.
 */
bool 
MinimalOpSetCheck::check(
        const TTAMachine::Machine& machine,
        MachineCheckResults& results) const {
    
    // construct the opset list
    TTAMachine::Machine::FunctionUnitNavigator fuNav = 
        machine.functionUnitNavigator();
    TCETools::CIStringSet opSet;
    for (int i = 0; i < fuNav.count(); i++) {
        TTAMachine::FunctionUnit* fu = fuNav.item(i);
        fu->operationNames(opSet);
    }

    TCETools::CIStringSet::iterator first1 = minimalOpSet_.begin();
    TCETools::CIStringSet::iterator last1 = minimalOpSet_.end();

    TCETools::CIStringSet::iterator first2 = opSet.begin();
    TCETools::CIStringSet::iterator last2 = opSet.end();

    std::string eMsg = "Operation missing from the minimal operation set: ";

    bool errorsAdded = false;
    // missing opset is the difference towards minimalOpSet_
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            results.addError(*this, eMsg.append(*first1++));
            errorsAdded = true;
        } else if (*first2 < *first1) {
            ++first2;
        } else { 
            ++first1; 
            ++first2; 
        }
    }
    while (first1 != last1) {
        results.addError(*this, eMsg.append(*first1++));
        errorsAdded = true;
    }
    return !errorsAdded;
}


/**
 * Checks if machine has all operations in minimal opset.
 *
 * Ignores fus with specified names from the check. This is useful with
 * testing if minimal opset requirement breaks if a certain FUs are removed.
 *
 * @param machine Machine to be checked against minimal opset.
 * @param ignoreFUs Names of the fus to be ignored regarding the check.
 * @return True if minimal operation set was met, false otherwise.
 */
bool 
MinimalOpSetCheck::checkWithIgnore(
    const TTAMachine::Machine& machine,
    const std::set<std::string>& ignoreFUName) const {

    TTAMachine::Machine::FunctionUnitNavigator fuNav = 
        machine.functionUnitNavigator();
    TCETools::CIStringSet opSet;
    // construct the opset list
    for (int i = 0; i < fuNav.count(); i++) {
        TTAMachine::FunctionUnit* fu = fuNav.item(i);
        if (ignoreFUName.find(fu->name()) == ignoreFUName.end()) {
            fu->operationNames(opSet);
        }
    }

    // if machines opset is smaller than required opset
    if (opSet.size() < minimalOpSet_.size()) {
        return false;
    }

    TCETools::CIStringSet::const_iterator first1 = minimalOpSet_.begin();
    TCETools::CIStringSet::const_iterator last1 = minimalOpSet_.end();

    TCETools::CIStringSet::iterator first2 = opSet.begin();
    TCETools::CIStringSet::iterator last2 = opSet.end();

    // return false if missing operation was found
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            return false;
        } else if (*first2 < *first1) {
            ++first2;
        } else { 
            ++first1; 
            ++first2; 
        }
    }
    if (first1 != last1) {
        return false;
    }
    return true;
}


/**
 * Return operations that are missing from a machine.
 *
 * Returns operations that are missing from a machine compared to the minimal
 * operation set.
 *
 * @param machine Machine to be checked against minimal opset.
 * @param missingOps Vector where missing operation names are to be stored.
 */
void 
MinimalOpSetCheck::missingOperations(
    const TTAMachine::Machine& machine,
    std::vector<std::string>& missingOps) const {

    // construct the opset list
    TTAMachine::Machine::FunctionUnitNavigator fuNav = 
        machine.functionUnitNavigator();
    TCETools::CIStringSet opSet;
    for (int i = 0; i < fuNav.count(); i++) {
        TTAMachine::FunctionUnit* fu = fuNav.item(i);
        fu->operationNames(opSet);
    }

    TCETools::CIStringSet::const_iterator first1 = minimalOpSet_.begin();
    TCETools::CIStringSet::const_iterator last1 = minimalOpSet_.end();

    TCETools::CIStringSet::iterator first2 = opSet.begin();
    TCETools::CIStringSet::iterator last2 = opSet.end();

    // missing opset is the difference towards minimalOpSet_
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            missingOps.push_back(*first1++);
        } else if (*first2 < *first1) {
            ++first2;
        } else { 
            ++first1; 
            ++first2; 
        }
    }
    while (first1 != last1) {
        missingOps.push_back(*first1++);
    }
}


/**
 * Constructs a minimal opset from a given machine.
 *
 * @param machine Machine that is used as reference for minimal opset.
 */
void 
MinimalOpSetCheck::buildMinimalOpSet(const TTAMachine::Machine* machine) {
    bool deleteMach = false;
    if (machine == NULL) {
        machine = TTAMachine::Machine::loadFromADF(Environment::minimalADF());
        deleteMach = true;
    }

    TTAMachine::Machine::FunctionUnitNavigator fuNav = 
        machine->functionUnitNavigator();
    // construct the opset list
    for (int i = 0; i < fuNav.count(); i++) {
        TTAMachine::FunctionUnit* fu = fuNav.item(i);
        fu->operationNames(minimalOpSet_);
    }

    if (deleteMach) {
        delete machine;
        machine = NULL;
    }
}


/**
 * Returns constructed minimal opset.
 *
 * @return Minimal opset as strings in a set.
 */
TCETools::CIStringSet
MinimalOpSetCheck::minimalOpSet() const {
    return minimalOpSet_;
}


/**
 * Adds FUs to the machine so that it doesn't miss operations anymore.
 *
 * Check is done against minimal opset.
 *
 * @param machine Machine to be checked against minimal opset and where FUs
 * are inserted so that minimal opset is fulfilled.
 * @return A short description what was done.
 */
std::string
MinimalOpSetCheck::fix(TTAMachine::Machine& mach) const {
    std::vector<std::string> missingOps;
    missingOperations(mach, missingOps);

    if (missingOps.size() < 1) {
        const std::string errorMessage = "No missing operations found.";
        throw InvalidData(
            __FILE__, __LINE__, __func__, errorMessage);
    }

    // go through minimal adf and add FUs that include missing ops
    TTAMachine::Machine* minMach = TTAMachine::Machine::loadFromADF(
            Environment::minimalADF());
    
    TTAMachine::Machine::FunctionUnitNavigator fuNav = 
        minMach->functionUnitNavigator();
    std::set<std::string> fuAdded;
    FullyConnectedCheck conCheck = FullyConnectedCheck();

    for (unsigned int moi = 0; moi < missingOps.size(); ++moi) {
        for (int fui = 0; fui < fuNav.count(); ++fui) {
            TTAMachine::FunctionUnit* fu = fuNav.item(fui);
            if (fu->hasOperation(missingOps.at(moi))) {
                if (fuAdded.end() != fuAdded.find(fu->name())) {
                    break;
                }
                fuAdded.insert(fu->name());
                fu->unsetMachine();
                mach.addFunctionUnit(*fu);
                // connect the fu
                for (int op = 0; op < fu->operationPortCount(); ++op) {
                    conCheck.connectFUPort(*fu->operationPort(op));
                }
                break;
            }
        }
    }
    delete minMach;
    return "Operations were added to fulfill minimal opset requirements.";
}

/**
 * Returns true if the checker can automatically fix the machine to pass
 * the check.
 *
 * @return True, minimal opset can be always added to the machine.
 */
bool
MinimalOpSetCheck::canFix(const TTAMachine::Machine&) const {
    return true;
}
