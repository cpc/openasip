/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file CostDBTypes.hh
 *
 * Declaration of CostDBTypes class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */


#ifndef TTA_COST_DB_TYPES_HH
#define TTA_COST_DB_TYPES_HH


#include <string>
#include <vector>

class CostDBEntry;
class MatchType;

/**
 * Type definiotions and constant for the cost database.
 */
class CostDBTypes {
public:
    /**
     * Type of match to be accepted in the database queries for a
     * certain field.
     */
    enum TypeOfMatch {
        MATCH_ALL,          //< All entries accepted.
        MATCH_EXACT,        //< Equally matching entries accepted.
        MATCH_SUBSET,       //< Smaller or equally matching entries accepted.
        MATCH_SUPERSET,     //< Greater or equally matching entries accepted.
        MATCH_INTERPOLATION //< Interpolation of smaller and greater entries.
    };

    /// Entry type for function units.
    static const std::string EK_UNIT;
    /// Entry type for register files.
    static const std::string EK_RFILE;
    /// Entry type for move bus.
    static const std::string EK_MBUS;
    /// Entry type for sockets.
    static const std::string EK_SOCKET;
    /// Entry type for input sockets.
    static const std::string EK_INPUT_SOCKET;
    /// Entry type for output sockets.
    static const std::string EK_OUTPUT_SOCKET;
    /// Entry type for immediate sockets.
    static const std::string EK_INLINE_IMM_SOCKET;

    /// Field type for bit width of an entry.
    static const std::string EKF_BIT_WIDTH;
    /// Field type for operations supported by an entry.
    static const std::string EKF_OPERATIONS;
    /// Field type for latency of an entry.
    static const std::string EKF_LATENCY;
    /// Field type for number of registers in an entry.
    static const std::string EKF_NUM_REGISTERS;
    /// Field type for number of read ports in an entry.
    static const std::string EKF_READ_PORTS;
    /// Field type for number of write ports in an entry.
    static const std::string EKF_WRITE_PORTS;
    /// Field type for number of bidirectional ports in an entry.
    static const std::string EKF_BIDIR_PORTS;
    /// Field type for number of max simultaneous reads in an entry.
    static const std::string EKF_MAX_READS;
    /// Field type for number of max simultaneous writes in an entry.
    static const std::string EKF_MAX_WRITES;
    /// Field type for fanin of bus in an entry.
    static const std::string EKF_BUS_FANIN;
    /// Field type for fanout of bus in an entry.
    static const std::string EKF_BUS_FANOUT;
    /// Field type for fanin of input socket in an entry.
    static const std::string EKF_INPUT_SOCKET_FANIN;
    /// Field type for fanout of output socket in an entry.
    static const std::string EKF_OUTPUT_SOCKET_FANOUT;
    /// Field type for guard support in an entry.
    static const std::string EKF_GUARD_SUPPORT;
    /// Field type for guard latency in an entry.
    static const std::string EKF_GUARD_LATENCY;
    /// Field type for function unit entry;
    static const std::string EKF_FUNCTION_UNIT;

    /// Table of database entries.
    typedef std::vector<CostDBEntry*> EntryTable;

    /// Table of types of match.
    typedef std::vector<MatchType*> MatchTypeTable;
};

#endif
