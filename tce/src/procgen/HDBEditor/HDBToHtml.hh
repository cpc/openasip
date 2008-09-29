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
 * @file HDBToHtml.hh
 *
 * Declaration of HDBToHtml class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_TO_HTML_HH
#define TTA_HDB_TO_HTML_HH

#include <iostream>
#include "DBTypes.hh"

namespace HDB {
    class HDBManager;
}

/**
 * Collection of functions for generating html pages displaying information of
 * HDB elements.
 */
class HDBToHtml {
public:
    HDBToHtml(const HDB::HDBManager& hdb);
    virtual ~HDBToHtml();

    void fuEntryToHtml(RowID id, std::ostream& stream);
    void rfEntryToHtml(RowID id, std::ostream& stream);
    void busEntryToHtml(RowID id, std::ostream& stream);
    void socketEntryToHtml(RowID id, std::ostream& stream);
    void fuArchToHtml(RowID id, std::ostream& stream);
    void rfArchToHtml(RowID id, std::ostream& stream);
    void fuImplToHtml(RowID id, std::ostream& stream);
    void rfImplToHtml(RowID id, std::ostream& stream);
    void costFunctionPluginToHtml(RowID id, std::ostream& stream);

    static const std::string HDB_ROOT;
    static const std::string FU_ENTRIES;
    static const std::string RF_ENTRIES;
    static const std::string RF_IU_ENTRIES;
    static const std::string BUS_ENTRIES;
    static const std::string SOCKET_ENTRIES;
    static const std::string FU_ARCHITECTURES;
    static const std::string RF_ARCHITECTURES;
    static const std::string FU_IMPLEMENTATIONS;
    static const std::string RF_IMPLEMENTATIONS;
    static const std::string COST_PLUGINS;

private:
    /// HDB to create html from.
    const HDB::HDBManager& hdb_;
};

#endif
