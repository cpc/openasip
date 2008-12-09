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
 * @file CostDatabaseRegistry.cc
 *
 * Implementation of CostDatabaseRegistry class.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */
#include "CostDatabaseRegistry.hh"
#include "AssocTools.hh"
#include "HDBManager.hh"
#include "Application.hh"
#include "CostDatabase.hh"

CostDatabaseRegistry* CostDatabaseRegistry::instance_ = NULL;

/**
 * Constructor.
 */ 
CostDatabaseRegistry::CostDatabaseRegistry() {
}

/**
 * Destructor.
 */
CostDatabaseRegistry::~CostDatabaseRegistry() {
    AssocTools::deleteAllValues(registry_);
    if (instance_ != NULL) {
        delete instance_;
        instance_ = NULL;
    }
}

/**
 * Creates and returns an instance of cost database registry.
 *
 * @return An instance of cost database registry.
 */
CostDatabaseRegistry&
CostDatabaseRegistry::instance() {

    if (instance_ == NULL) {
        instance_ = new CostDatabaseRegistry();
    }
    return *instance_;
}

/**
 * Returns CostDatabase that is build against the HDB at given path.
 *
 * In case the CostDatabase is not found in registry, tries to generate it.
 *
 * @param hdbFileName File name of the HDB to build the CostDatabase against.
 * @return The CostDatabase build against the HDB file.
 * @exception Exception In case there was a problem while creating the
 * CostDatabase.
 */
CostDatabase& 
CostDatabaseRegistry::costDatabase(const HDB::HDBManager& hdb)
    throw (Exception) {
    
    if (AssocTools::containsKey(registry_, &hdb)) {
        return *registry_[&hdb];
    }
    CostDatabase* costDatabase = &CostDatabase::instance(hdb);
    registry_[&hdb] = costDatabase;
    return *costDatabase;
}

/**
 * Adds CostDatabase and HDB used in the CostDatabase in to the registry.
 *
 * @param costDatabase The cost database to be added in to the registry.
 * @param hdb The HDB that is used to create the CostDatabase.
 */
void
CostDatabaseRegistry::addCostDatabase(
    CostDatabase* costDatabase, const HDB::HDBManager& hdb) {
    
    if (AssocTools::containsKey(registry_, &hdb)) {
        delete costDatabase;
        costDatabase = NULL;
        return;
    }
    registry_[&hdb] = costDatabase;
}

/**
 * Returns true if the registry contains a CostDatabase build against the
 * given HDB.
 *
 * @param hdb The HDB that is used in creating the CostDatabase.
 * @return True if the registry contains a CostDatabase build against the
 * given HDB.
 */
bool
CostDatabaseRegistry::hasCostDatabase(const HDB::HDBManager& hdb) {

    if (AssocTools::containsKey(registry_, &hdb)) {
        return true;
    } else {
        return false;
    }
}

/**
 * Returns the total number of stored CostDatabases.
 *
 * @return Count of stored CostDatabases.
 */
int
CostDatabaseRegistry::costDatabaseCount() {

    return registry_.size();
}

/**
 * Returns the CostDatabase from given index.
 *
 * @return The CostDatabase from the given index.
 * @exception OutOfRange Is thrown if index is bigger than the CostDatabase
 * count.
 */
CostDatabase&
CostDatabaseRegistry::costDatabase(unsigned int index) 
    throw (OutOfRange) {

    if (index > (registry_.size() - 1)) {
        throw OutOfRange(__FILE__, __LINE__,
                         "CostDatabaseRegistry::costDatavase(unsigned int)");
    }
    std::map<const HDB::HDBManager*, CostDatabase*>::const_iterator iter =
        registry_.begin();
    for (unsigned int counter = 0; iter != registry_.end(); iter++) {
        if (counter == index) {
            break;
        } else {
            counter++;
        }
    }
    return *(*iter).second;
}

/**
 * Returns the full path of the HDB with given index.
 *
 * @return The HDB file path.
 * @exception OutOfRange Is thrown if index is out of range.
 */
std::string
CostDatabaseRegistry::hdbPath(unsigned index) 
    throw (OutOfRange) {

    if (index > (registry_.size() - 1)) {
        throw OutOfRange(__FILE__, __LINE__,
                         "CostDatabaseRegistry::hdbPath(unsigned int)");
    }
    std::map<const HDB::HDBManager*, CostDatabase*>::const_iterator iter = 
        registry_.begin();

    for (unsigned c = 0; iter != registry_.end(); iter++) {
        if (c == index) return (*iter).first->fileName();
        c++;
    }
    assert(false);
}
