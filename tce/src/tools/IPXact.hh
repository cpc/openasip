/*
    Copyright (c) 2002-2017 Tampere University.

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
* @file IPXact.hh
*
* @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
*/
#pragma once

#include <string>
#include <vector>

namespace ipxact {

    struct Parameter {
        std::string id;
        std::string type;
        std::string name;
        std::string value;
    };

    struct Port {
        std::string name;
        std::string direction;
        bool vector;
        std::string left;
        std::string right;
        std::string width;
        std::string defaultValue;
    };

    struct ModuleInfo {
        std::string name;
        std::vector<Parameter> parameters;
        std::vector<Port> ports;
    };

    struct BusInfo {
        std::string name;
        std::vector<Port> ports;
    };

    /**
     * Parse IP-XACT abstraction bus definition.
     */
    BusInfo parseBus(std::string file);

    /**
     * Parse IP-XACT component description.
     */
    ModuleInfo parseComponent(std::string file);
}
