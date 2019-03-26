/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
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
