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
 * @file ICDecoderGeneratorPlugin.hh
 *
 * Declaration of ICDecoderGeneratorPlugin class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_ICDECODER_GENERATOR_PLUGIN_HH
#define TTA_ICDECODER_GENERATOR_PLUGIN_HH

#include <map>
#include <string>

#include "Socket.hh"
#include "HDBTypes.hh"

namespace TTAMachine {
class Machine;
class ImmediateUnit;
}

class BinaryEncoding;

namespace ProGe {

class Netlist;
class NetlistGenerator;

/**
 * Base class for plugins that are able to generate and IC and decoder.
 *
 * @todo Add rest of the methods.
 */
class ICDecoderGeneratorPlugin {
public:
    ICDecoderGeneratorPlugin(
        const TTAMachine::Machine& machine,
        const BinaryEncoding& bem,
        const std::string& description);
    virtual ~ICDecoderGeneratorPlugin();

    /**
     * Completes the given netlist by adding IC block and completing the
     * decoder block by adding the ports connected to IC. Connects also IC
     * to all the units in the machine.
     *
     * @param netlist The netlist to complete.
     * @param generator The netlist generator which generated the netlist.
     */
    virtual void completeNetlist(
        Netlist& netlist,
        const NetlistGenerator& generator) = 0;

    /**
     * Generates the interconnection network and instruction decoder to the
     * given destination directory.
     *
     * @param destinationDirectory The destination directory.
     * @param generator The netlist generator that generated the netlist.
     */
    virtual void generate(
        const std::string& destinationDirectory,
        const NetlistGenerator& generator) = 0;

    /**
     * Returns the required latency of the hardware implementation of the
     * given immediate unit.
     *
     * @param iu The immediate unit.
     */
    virtual int requiredRFLatency(
        const TTAMachine::ImmediateUnit& iu) const = 0;

    /**
     * Verifies that the plugin is compatible with the machine.
     *
     * @exception InvalidData If the plugin is not compatible with the 
     *                        machine.
     */
    virtual void verifyCompatibility() const
        throw (InvalidData) = 0;

    std::string pluginDescription() const;
    int recognizedParameterCount() const;
    std::string recognizedParameter(int index) const
        throw (OutOfRange);
    std::string parameterDescription(const std::string& paramName) const
        throw (IllegalParameters);
    void setParameter(const std::string& name, const std::string& value)
        throw (IllegalParameters);

    const TTAMachine::Machine& machine() const;
    const BinaryEncoding& bem() const;

protected:
    static TTAMachine::Socket::Direction convertDirection(
        HDB::Direction direction);
    void addParameter(
        const std::string& name,
        const std::string& description);
    bool hasParameterSet(const std::string& name) const;
    std::string parameterValue(const std::string& name) const
        throw (KeyNotFound);

private:
    /// Map type for strings.
    typedef std::map<std::string, std::string> StringMap;

    /// The machine to generate.
    const TTAMachine::Machine& machine_;
    /// The binary encoding map.
    const BinaryEncoding& bem_;
    /// Parameters set.
    StringMap parameterValues_;
    /// Parameter descriptions.
    StringMap parameterDescriptions_;
    /// Description of the plugin.
    std::string description_;
};

/**
 * Exports the given class as an IC&decoder generator.
 *
 * @note The class name of the generator MUST be [PLUGIN_NAME__]Generator,
 *       thus a generator exported with EXPORT_ICDEC_GENERATOR(Example)
 *       exports a class called ExampleGenerator.
 */
#define EXPORT_ICDEC_GENERATOR(PLUGIN_NAME__) \
extern "C" { \
    ICDecoderGeneratorPlugin*\
    create_generator_plugin_##PLUGIN_NAME__(\
        const TTAMachine::Machine& machine,\
        const BinaryEncoding& bem) {\
        PLUGIN_NAME__##Generator* instance = \
            new PLUGIN_NAME__##Generator(machine, bem);     \
        return instance;\
    }\
    void delete_generator_plugin_##PLUGIN_NAME__(\
        ICDecoderGeneratorPlugin* target) {\
        delete target;\
    }\
}
}

#endif
