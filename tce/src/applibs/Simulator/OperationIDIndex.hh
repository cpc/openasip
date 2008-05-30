/**
 * @file OperationIDIndex.hh
 *
 * Declaration of OperationIDIndex wrapper class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */
#include <map>
#include <string>

/// Type for identifying operations in the conflict detector interface.
typedef int OperationID;

/**
 * A wrapper class around std::map
 */
class OperationIDIndex : public std::map<std::string, OperationID> {
public:
    OperationIDIndex();
    virtual ~OperationIDIndex();
private:
};
