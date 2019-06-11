/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file CostEstimationData.cc
 *
 * Implementation of CostEstimationData class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
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
