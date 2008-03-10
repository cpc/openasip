/**
 * @file NetlistWriter.cc
 *
 * Implementation of NetlistWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <string>
#include "NetlistWriter.hh"

using std::string;

namespace ProGe {

/**
 * The constructor.
 *
 * @param netlist The netlist to be written in some HDL.
 */
NetlistWriter::NetlistWriter(const Netlist& netlist) : netlist_(netlist) {
}


/**
 * The destructor.
 */
NetlistWriter::~NetlistWriter() {
}


/**
 * Returns the netlist.
 *
 * @return The netlist.
 */
const Netlist&
NetlistWriter::netlist() const {
    return netlist_;
}
}
