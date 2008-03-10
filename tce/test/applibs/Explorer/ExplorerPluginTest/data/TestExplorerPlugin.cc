/**
 * @file TestExplorerPlugin.cc
 *
 * Dummy test explorer plugin.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
 * @note rating: red
 */
#include <vector>
#include <string>
#include "DesignSpaceExplorerPlugin.hh"
#include "DSDBManager.hh"
#include "Machine.hh"


/**
 * Dummy explorer plugin to test the loading system.
 */
class TestExplorerPlugin : public DesignSpaceExplorerPlugin {
public:
    DESCRIPTION("dummy explorer plugin to test the loading system");

    virtual std::vector<RowID>
    explore(const RowID&, const unsigned int&) {
        RowID conf1 = 1;
        RowID conf2 = 2;
        RowID conf3 = 3;
        std::vector<RowID> result;
        result.push_back(conf1);
        result.push_back(conf2);
        result.push_back(conf3);
        return result;
    }
};

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(TestExplorerPlugin);
