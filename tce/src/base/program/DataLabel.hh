/**
 * @file DataLabel.hh
 *
 * Declaration of DataLabel class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_DATA_LABEL_HH
#define TTA_DATA_LABEL_HH

#include "Label.hh"

namespace TTAProgram {

/**
 * A symbol that represents a location in the data area used by the program.
 *
 * Usually, a data label is the starting address of a program variable.
 */
class DataLabel : public Label {
public:
    DataLabel(const std::string& name, Address address, const Scope& scope);
    virtual ~DataLabel();
};

}

#endif
