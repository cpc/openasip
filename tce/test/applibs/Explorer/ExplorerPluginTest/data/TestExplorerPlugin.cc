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
 * @file TestExplorerPlugin.cc
 *
 * Dummy test explorer plugin.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
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

    virtual bool requiresStartingPointArchitecture() const { return false; }
    virtual bool producesArchitecture() const { return false; }
    virtual bool requiresHDB() const { return true; }
    virtual bool requiresSimulationData() const { return false; }

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

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(TestExplorerPlugin)
