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
 * @file CostDBEntry.cc
 *
 * Implementation of CostDBEntry class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include "CostDBEntry.hh"
#include "Application.hh"
#include "CostDBEntryStatsRF.hh"
#include "CostDBEntryStatsFU.hh"

/**
 * Constructor.
 *
 * @param key An entry key.
 */
CostDBEntry::CostDBEntry(CostDBEntryKey* key) :
    entryKey_(key) {
}

/**
 * Constructor.
 *
 * Combines two entries into one using weighter as a weighting factor
 * on interpolating the statistics. Also replacing weighter field in
 * entry key.
 *
 * @param entry1 First entry.
 * @param entry2 Second entry.
 * @param weighter A field.
 */
CostDBEntry::CostDBEntry(
    const CostDBEntry& entry1,
    const CostDBEntry& entry2,
    const EntryKeyField& weighter) {

    EntryKeyField field1 = entry1.keyFieldOfType(*weighter.type());
    EntryKeyField field2 = entry2.keyFieldOfType(*weighter.type());
    double coefficient = weighter.coefficient(field1, field2);

    entryKey_ = entry1.entryKey_->copy();
    entryKey_->replaceField(new EntryKeyField(weighter));
    for (StatsTable::const_iterator i = entry1.entryStats_.begin();
         i != entry1.entryStats_.end(); i++) {
        for (StatsTable::const_iterator j = entry2.entryStats_.begin();
             j != entry2.entryStats_.end(); j++) {
            // newStats cannot be base class type if stats are for FU or RF
            if (dynamic_cast<CostDBEntryStatsRF*>(*i) &&
                dynamic_cast<CostDBEntryStatsRF*>(*j)) {
                CostDBEntryStatsRF* newStats = new CostDBEntryStatsRF(
                    *dynamic_cast<CostDBEntryStatsRF*>(*i),
                    *dynamic_cast<CostDBEntryStatsRF*>(*j), coefficient);
                addStatistics(newStats);
            } else if (dynamic_cast<CostDBEntryStatsFU*>(*i) &&
                       dynamic_cast<CostDBEntryStatsFU*>(*j)) {
                CostDBEntryStatsFU* newStats = new CostDBEntryStatsFU(
                    *dynamic_cast<CostDBEntryStatsFU*>(*i),
                    *dynamic_cast<CostDBEntryStatsFU*>(*j), coefficient);
                addStatistics(newStats);
            } else { // if the entry is Bus or Socket
                CostDBEntryStats* newStats = 
                    new CostDBEntryStats(*(*i), *(*j), coefficient);
                addStatistics(newStats);
            }
        }
    }
}

/**
 * Destructor.
 */
CostDBEntry::~CostDBEntry() {
    assert(entryKey_ != NULL);
    delete entryKey_;
    entryKey_ = NULL;

    for (StatsTable::iterator i = entryStats_.begin();
         i != entryStats_.end(); i++) {
        assert(*i != NULL);
        delete *i;
    }
}

/**
 * Copies the entry.
 *
 * Client is responsible of deallocating the memory reserved for the
 * returned object.
 *
 * @return A copy of the entry.
 */
CostDBEntry*
CostDBEntry::copy() const {
    CostDBEntry* newEntry = new CostDBEntry(entryKey_->copy());

    for (StatsTable::const_iterator i = entryStats_.begin();
         i != entryStats_.end(); i++) {
        newEntry->addStatistics((*i)->copy());
    }

    return newEntry;
}

/**
 * Returns type of the entry.
 *
 * @return Type.
 */
const EntryKeyProperty*
CostDBEntry::type() const {
    return entryKey_->type();
}

/**
 * Returns demanded field of the entry.
 *
 * @param type Type of the field.
 * @return Demanded entry field.
 */
EntryKeyField
CostDBEntry::keyFieldOfType(const EntryKeyFieldProperty& type) const {
    return entryKey_->keyFieldOfType(type);
}

/**
 * Returns demanded field of the entry.
 *
 * @param type Type of the field.
 * @return Demanded entry field.
 */
EntryKeyField
CostDBEntry::keyFieldOfType(std::string type) const {
    return entryKey_->keyFieldOfType(type);
}

/**
 * Returns the number of fields in an entry.
 *
 * @return The number of fields in an entry.
 */
int
CostDBEntry::fieldCount() const {
    return entryKey_->fieldCount();
}

/**
 * Returns the field found on the given index.
 *
 * The index must be between 0 and the number of entry key - 1.
 *
 * @param index Index.
 * @return The field found on the given index.
 */
const EntryKeyField&
CostDBEntry::field(int index) const {
    return entryKey_->field(index);
}

/**
 * Replaces certain field of the entry.
 *
 * @param newField A field.
 */
void
CostDBEntry::replaceField(EntryKeyField* newField) {
    entryKey_->replaceField(newField);
}

/**
 * Checks whether two entries have equal key.
 *
 * @param entry Database entry to compare with.
 * @return True if two entries have equal key, false otherwise.
 */
bool
CostDBEntry::isEqualKey(const CostDBEntry& entry) const {
    return entryKey_->isEqual(*entry.entryKey_);
}

/**
 * Add new statistics into this entry.
 *
 * @param newStats Statistics to be added.
 */
void
CostDBEntry::addStatistics(CostDBEntryStats* newStats) {
    entryStats_.push_back(newStats);
}


/**
 * Returns the number of statistics in the entry.
 * 
 * @return Number of statistics in the entry.
 */
int
CostDBEntry::statisticsCount() const {
    return entryStats_.size();
}

/**
 * Returns the statistics according to given index.
 *
 * @param index The index of the statistics.
 * @return The statistics according to given index.
 * @throw OutOfRange Index is out of bounds.
 */
const CostDBEntryStats&
CostDBEntry::statistics(int index) const throw (OutOfRange) {
    if (index < 0 || index >= statisticsCount()) {
	throw OutOfRange(__FILE__, __LINE__, "CostDBEntry::statistics");
    }
    return *entryStats_[index];
}
