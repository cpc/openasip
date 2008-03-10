/**
 * @file InterPassData.hh
 *
 * Declaration of InterPassData.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_INTER_PASS_DATA_HH
#define TTA_INTER_PASS_DATA_HH

#include <map>
#include <string>
#include "Exception.hh"

class InterPassDatum;

/** 
 * Class for storing inter-pass data of any type.
 *
 * Provides a generic "key-value" interface and shortcut methods for
 * accessing the most frequently used data.
 */
class InterPassData {
public:
    InterPassData();
    virtual ~InterPassData();

    InterPassDatum& datum(const std::string& key)
        throw (KeyNotFound);
    bool hasDatum(const std::string& key) const;
    void setDatum(const std::string& key, InterPassDatum* datum);
private:
    /// container for the data
    std::map<std::string, InterPassDatum*> data_;
};

#endif

