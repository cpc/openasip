/**
 * @file WriteRequest.cc
 *
 * Definition of WriteRequest struct.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#include "Memory.hh"
#include "WriteRequest.hh"

WriteRequest::WriteRequest() :
    data_(NULL),
    dataSize_(0),
    address_(0),
    size_(0) {
}
