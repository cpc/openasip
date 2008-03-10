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
