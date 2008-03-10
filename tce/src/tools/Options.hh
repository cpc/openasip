/**
 * @file Options.hh
 *
 * Declaration of Options class.
 *
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPTIONS_HH
#define TTA_OPTIONS_HH

#include <string>
#include <map>
#include <vector>

#include "OptionValue.hh"

using std::map;
using std::string;
using std::vector;

/**
 * Generic container of option values.
 */
class Options {
public:
    Options();
    virtual ~Options();
    void addOptionValue(const string& name, OptionValue* option)
	throw (TypeMismatch);

    // index for lists of options
    OptionValue& optionValue(const string& name, int index = 0)
	throw (OutOfRange, KeyNotFound);
    int valueCount(const string& name)
	throw (KeyNotFound);

private:
    map<string, vector<OptionValue*> > options_;
};

#endif
