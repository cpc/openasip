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
 * @file IPXact.cc
 *
 * @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
 */
#include "IPXact.hh"

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/regex.hpp>

namespace ipxact {

BusInfo
parseBus(std::string file) {
    BusInfo bus;
    using boost::property_tree::ptree;

    ptree pt;
    read_xml(file, pt);

    bus.name = pt.get_child(
                     "ipxact:abstractionDefinition.ipxact:busType"
                     ".<xmlattr>.name")
                   .data();
    ptree ports = pt.get_child(
        "ipxact:abstractionDefinition."
        "ipxact:ports");

    for (auto&& p : ports) {
        if (p.first == "ipxact:port") {
            std::string name =
                p.second.get_child("ipxact:logicalName").data();
            std::string width = p.second
                                    .get_child(
                                        "ipxact:wire."
                                        "ipxact:onMaster.ipxact:width")
                                    .data();
            std::string direction =
                p.second
                    .get_child(
                        "ipxact:wire."
                        "ipxact:onMaster.ipxact:direction")
                    .data();
            std::string left = width + "-1";
            std::string right = "0";
            std::string defaultValue =
                direction == "out"
                    ? p.second.get_child("ipxact:wire.ipxact:defaultValue")
                          .data()
                    : "";

            bus.ports.emplace_back(Port{
                name, direction, true, left, right, width, defaultValue});
        }
    }

    return bus;
}

ModuleInfo
parseComponent(std::string file) {
    ModuleInfo module;
    using boost::property_tree::ptree;

    ptree pt;
    read_xml(file, pt);
    boost::optional<ptree&> parameters =
        pt.get_child_optional("ipxact:component.ipxact:parameters");
    ptree ports = pt.get_child("ipxact:component.ipxact:model.ipxact:ports");
    module.name = pt.get_child("ipxact:component.ipxact:name").data();

    // get parameters if any.
    if (parameters) {
        for (auto&& p : *parameters) {
            if (p.first == "ipxact:parameter") {
                std::string id =
                    p.second.get_child("<xmlattr>.parameterId").data();
                std::string type =
                    p.second.get_child("<xmlattr>.type").data();
                std::string name = p.second.get_child("ipxact:name").data();
                std::string value = p.second.get_child("ipxact:value").data();
                Parameter parameter = {id, type, name, value};
                module.parameters.emplace_back(parameter);
            }
        }
    }

    // get ports
    for (auto&& p : ports) {
        if (p.first == "ipxact:port") {
            std::string name = p.second.get_child("ipxact:name").data();
            std::string direction =
                p.second.get_child("ipxact:wire.ipxact:direction").data();
            std::transform(
                direction.begin(), direction.end(), direction.begin(),
                ::tolower);
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            bool vector = false;
            std::string left = "0";
            std::string right = "0";
            std::string width = "0";
            ptree wire = p.second.get_child("ipxact:wire");
            for (auto&& w : wire) {
                if (w.first == "ipxact:vectors") {
                    vector = true;
                    left = w.second.get_child("ipxact:vector.ipxact:left")
                               .data();
                    width = left;
                    right = w.second.get_child("ipxact:vector.ipxact:right")
                                .data();
                }
            }
            // replace uuids in left and right.
            for (auto&& m : module.parameters) {
                boost::regex expr("\\b" + m.id + "\\b");
                width = boost::regex_replace(width, expr, m.value);
                left = boost::regex_replace(left, expr, m.name);
                right = "0";  // boost::regex_replace(right, expr, m.name);
            }
            width += "+1";

            module.ports.emplace_back(
                Port{name, direction, vector, left, right, width, ""});
        }
    }

    return module;
}
}  // namespace ipxact
