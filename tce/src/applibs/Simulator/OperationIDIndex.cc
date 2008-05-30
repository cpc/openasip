/**
 * @file OperationIDIndex.cc
 *
 * Definition of OperationIDIndex wrapper class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */
#include <map>
#include <string>
#include "OperationIDIndex.hh"

/**
 * Default constructor
 */
OperationIDIndex::OperationIDIndex() :
    std::map<std::string, OperationID>() {
}

/**
 * Default destructor
 */
OperationIDIndex::~OperationIDIndex() {
}
