/**
 * @file WriteRequest.hh
 *
 * Declaration of WriteRequest struct.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef WRITE_REQUEST_HH
#define WRITE_REQUEST_HH

#include <vector>

/**
* Models an uncommitted write request.
*/
struct WriteRequest {
    WriteRequest();
    /// Data to be written.
    Memory::MAUTable data_;
    /// Data table size.
    std::size_t dataSize_;
    /// Address to be written to.
    Word address_;
    /// Size of the data to be read/written.
    int size_;
};

/**
 * A "typedef" for request queue
 */
struct RequestQueue : public std::vector<WriteRequest*> {
};

#endif
