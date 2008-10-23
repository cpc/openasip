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
 * @file MinimalOpSetCheck.cc
 *
 * Implementation of MinimalOpSetCheck class.
 *
 * @author Esa Määttä 2008 (esa.maatta-no.spam-tut.fi)
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

//using namespace TTAMachine;

/**
 * Constructor for using this generic class directly.
 */
MinimalOpSetCheck::MinimalOpSetCheck() : 
    MachineCheck("Common helper functionality for minimal opset checks.") {

    buildMinimalOpSet();
}


/**
 * Destructor.
 */
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
    std::set<std::string> opSet;
    // construct the opset list
    for (int i = 0; i < fuNav.count(); i++) {
        TTAMachine::FunctionUnit* fu = fuNav.item(i);
        fu->operationNames(opSet);
    }

    // if machines opset is smaller than required opset
    if (opSet.size() < minimalOpSet_.size()) {
        return false;
    }

    std::set<std::string>::const_iterator first1 = minimalOpSet_.begin();
    std::set<std::string>::const_iterator last1 = minimalOpSet_.end();

    std::set<std::string>::iterator first2 = opSet.begin();
    std::set<std::string>::iterator last2 = opSet.end();

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
    std::set<std::string> opSet;
    for (int i = 0; i < fuNav.count(); i++) {
        TTAMachine::FunctionUnit* fu = fuNav.item(i);
        fu->operationNames(opSet);
    }

    std::set<std::string>::const_iterator first1 = minimalOpSet_.begin();
    std::set<std::string>::const_iterator last1 = minimalOpSet_.end();

    std::set<std::string>::iterator first2 = opSet.begin();
    std::set<std::string>::iterator last2 = opSet.end();

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
    std::set<std::string> opSet;
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

    std::set<std::string>::const_iterator first1 = minimalOpSet_.begin();
    std::set<std::string>::const_iterator last1 = minimalOpSet_.end();

    std::set<std::string>::iterator first2 = opSet.begin();
    std::set<std::string>::iterator last2 = opSet.end();

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
    std::set<std::string> opSet;
    for (int i = 0; i < fuNav.count(); i++) {
        TTAMachine::FunctionUnit* fu = fuNav.item(i);
        fu->operationNames(opSet);
    }

    std::set<std::string>::const_iterator first1 = minimalOpSet_.begin();
    std::set<std::string>::const_iterator last1 = minimalOpSet_.end();

    std::set<std::string>::iterator first2 = opSet.begin();
    std::set<std::string>::iterator last2 = opSet.end();

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
    if (machine == NULL) {
        machine = TTAMachine::Machine::loadFromADF(Environment::minimalADF());
    }

    TTAMachine::Machine::FunctionUnitNavigator fuNav = 
        machine->functionUnitNavigator();
    // construct the opset list
    for (int i = 0; i < fuNav.count(); i++) {
        TTAMachine::FunctionUnit* fu = fuNav.item(i);
        fu->operationNames(minimalOpSet_);
    }
}


/**
 * Returns constructed minimal opset.
 *
 * @return Minimal opset as strings in a set.
 */
std::set<std::string> 
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
MinimalOpSetCheck::fix(TTAMachine::Machine& mach) const
    throw (InvalidData) {
    
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
