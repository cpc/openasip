/**
 * @file NetlistWriter.hh
 *
 * Declaration of NetlistWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NETLIST_WRITER_HH
#define TTA_NETLIST_WRITER_HH

#include <string>
#include "Exception.hh"

namespace ProGe {

class Netlist;

/**
 * Interface for different netlist writers.
 */
class NetlistWriter {
public:
    NetlistWriter(const Netlist& netlist);
    virtual ~NetlistWriter();

    virtual void write(const std::string& dstDirectory)
        throw (IOException, InvalidData) = 0;

protected:
    const Netlist& netlist() const;

private:
    const Netlist& netlist_;
};
}

#endif
