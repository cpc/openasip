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
 * @file CostEstimationData.cc
 *
 * Implementation of CostEstimationData class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "CostEstimationData.hh"

/**
 * The Constructor.
 */
CostEstimationData::CostEstimationData():
    hasName_(false), hasValue_(false),
    hasFUReference_(false), hasRFReference_(false),
    hasBusReference_(false), hasSocketReference_(false),
    hasPluginID_(false) {

}

/**
 * Destructor.
 */
CostEstimationData::~CostEstimationData() {
}

/**
 * Returns name of the cost estimation data value.
 *
 * @return Cost data name.
 */
std::string
CostEstimationData::name() const
    throw (NotAvailable) {

    if (!hasName_) {
        throw NotAvailable(
            __FILE__, __LINE__, __func__, "Attribute value not set.");
    }

    return name_;
}

/**
 * Returns value of the cost estimation data.
 *
 * @return Cost data value.
 */
DataObject
CostEstimationData::value() const
    throw (NotAvailable) {

    if (!hasValue_) {
        throw NotAvailable(
            __FILE__, __LINE__, __func__, "Attribute value not set.");
    }

    return value_;
}

/**
 * Returns the cost function plugin ID.
 *
 * @return Cost function plugin ID.
 */
RowID
CostEstimationData::pluginID() const
    throw (NotAvailable) {

    if (!hasPluginID_) {
        throw NotAvailable(
            __FILE__, __LINE__, __func__, "Attribute value not set.");
    }

    return pluginID_;
}

/**
 * Returns the FU entry reference.
 *
 * @return Function unit entry reference.
 */
RowID
CostEstimationData::fuReference() const
    throw (NotAvailable) {

    if (!hasFUReference_) {
        throw NotAvailable(
            __FILE__, __LINE__, __func__, "Attribute value not set.");
    }

    return fuReference_;
}

/**
 * Returns the RF entry reference.
 *
 * @return Register file entry reference.
 */
RowID
CostEstimationData::rfReference() const
    throw (NotAvailable) {

    if (!hasRFReference_) {
        throw NotAvailable(
            __FILE__, __LINE__, __func__, "Attribute value not set.");
    }

    return rfReference_;
}

/**
 * Returns the bus entry reference.
 *
 * @return Register file entry reference.
 */
RowID
CostEstimationData::busReference() const
    throw (NotAvailable) {

    if (!hasBusReference_) {
        throw NotAvailable(
            __FILE__, __LINE__, __func__, "Attribute value not set.");
    }

    return busReference_;
}

/**
 * Returns the socket entry reference.
 *
 * @return Register file entry reference.
 */
RowID
CostEstimationData::socketReference() const
    throw (NotAvailable) {

    if (!hasSocketReference_) {
        throw NotAvailable(
            __FILE__, __LINE__, __func__, "Attribute value not set.");
    }

    return socketReference_;
}
