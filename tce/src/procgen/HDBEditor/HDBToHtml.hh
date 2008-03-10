/**
 * @file HDBToHtml.hh
 *
 * Declaration of HDBToHtml class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
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
