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
 * @file MachineImplementation.hh
 *
 * Declaration of MachineImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_IMPLEMENTATION_HH
#define TTA_MACHINE_IMPLEMENTATION_HH

#include <string>
#include <vector>
#include "Serializable.hh"
#include "FUImplementationLocation.hh"
#include "RFImplementationLocation.hh"
#include "BusImplementationLocation.hh"
#include "SocketImplementationLocation.hh"


namespace IDF {

class UnitImplementationLocation;

/**
 * Repsesents the implementation of a machine defined in an IDF.
 */
class MachineImplementation : public Serializable {
public:
    MachineImplementation();

    MachineImplementation(const ObjectState* state)
        throw (ObjectStateLoadingException);

    virtual ~MachineImplementation();

    std::string sourceIDF() const;

    std::string icDecoderPluginName() const;
    bool hasICDecoderPluginName() const;
    std::string icDecoderPluginFile() const
        throw (FileNotFound);

    bool hasICDecoderPluginFile() const;
    std::string icDecoderHDB() const
        throw (FileNotFound);
    bool hasICDecoderHDB() const;
    std::string decompressorFile() const
        throw (FileNotFound);
    bool hasDecompressorFile() const;

    void setICDecoderPluginName(const std::string& name);
    void setICDecoderPluginFile(const std::string& file)
        throw (FileNotFound);
    void setICDecoderHDB(const std::string& file)
        throw (FileNotFound);
    void setDecompressorFile(const std::string& file)
        throw (FileNotFound);
 
    unsigned icDecoderParameterCount() const;
    std::string icDecoderParameterName(unsigned param) const
        throw (OutOfRange);
    std::string icDecoderParameterValue(const std::string& name) const;
    std::string icDecoderParameterValue(unsigned param) const
        throw (OutOfRange);
    void clearICDecoderParameters();

    void setICDecoderParameter(
        const std::string& name, const std::string& value);

    bool hasFUImplementation(const std::string& unitName) const;
    bool hasRFImplementation(const std::string& unitName) const;
    bool hasIUImplementation(const std::string& unitName) const;
    bool hasBusImplementation(const std::string& busName) const;
    bool hasSocketImplementation(const std::string& socketName) const;

    int fuImplementationCount() const;
    int rfImplementationCount() const;
    int iuImplementationCount() const;
    int busImplementationCount() const;
    int socketImplementationCount() const;

    FUImplementationLocation& fuImplementation(const std::string& fu) const
        throw (InstanceNotFound);
    RFImplementationLocation& rfImplementation(const std::string& rf) const
        throw (InstanceNotFound);
    RFImplementationLocation& iuImplementation(const std::string& iu) const
        throw (InstanceNotFound);
    BusImplementationLocation& busImplementation(const std::string& bus) const
        throw (InstanceNotFound);
    SocketImplementationLocation& socketImplementation(
        const std::string& socket) const
        throw (InstanceNotFound);
    
    FUImplementationLocation& fuImplementation(int index) const
        throw (OutOfRange);
    RFImplementationLocation& rfImplementation(int index) const
        throw (OutOfRange);
    RFImplementationLocation& iuImplementation(int index) const
        throw (OutOfRange);
    BusImplementationLocation& busImplementation(int index) const
        throw (OutOfRange);
    SocketImplementationLocation& socketImplementation(int index) const
        throw (OutOfRange);

    void addFUImplementation(FUImplementationLocation* implementation)
        throw (ObjectAlreadyExists, InvalidData);
    void addRFImplementation(RFImplementationLocation* implementation)
        throw (ObjectAlreadyExists, InvalidData);
    void addIUImplementation(RFImplementationLocation* implementation)
        throw (ObjectAlreadyExists, InvalidData);
    void addBusImplementation(BusImplementationLocation* implementation)
        throw (ObjectAlreadyExists, InvalidData);
    void addSocketImplementation(SocketImplementationLocation* implementation)
        throw (ObjectAlreadyExists, InvalidData);

    void removeFUImplementation(const std::string& unitName)
        throw (InstanceNotFound);
    void removeRFImplementation(const std::string& unitName)
        throw (InstanceNotFound);
    void removeIUImplementation(const std::string& unitName)
        throw (InstanceNotFound);
    void removeBusImplementation(const std::string& unitName)
        throw (InstanceNotFound);
    void removeSocketImplementation(const std::string& unitName)
        throw (InstanceNotFound);

    // methods from Serializable interface
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
    virtual ObjectState* saveState() const;

    static MachineImplementation*
    loadFromIDF(const std::string& idfFileName)
        throw (Exception);

    /// ObjectState name for machine implementation.
    static const std::string OSNAME_MACHINE_IMPLEMENTATION;
    /// ObjectState attribute name for the source IDF.
    static const std::string OSKEY_SOURCE_IDF;
    /// ObjectState name for the name of the IC/decoder plugin file.
    static const std::string OSNAME_IC_DECODER_PLUGIN;
    /// ObjectState attribute name for ic&decoder name.
    static const std::string OSKEY_IC_DECODER_NAME;
    /// ObjectState attribute name for ic&decoder file.
    static const std::string OSKEY_IC_DECODER_FILE;
    /// ObjectState attribute name for ic&decoder parameter.
    static const std::string OSNAME_IC_DECODER_PARAMETER;
    /// ObjectState attribute name for ic&decoder parameter name.
    static const std::string OSKEY_IC_DECODER_PARAMETER_NAME;
    /// ObjectState attribute name for ic&decoder parameter value.
    static const std::string OSKEY_IC_DECODER_PARAMETER_VALUE;
    /// ObjectState attribute name for ic&decoder HDB.
    static const std::string OSKEY_IC_DECODER_HDB;
    /// ObjectState attribute key for the name of the decompressor file.
    static const std::string OSKEY_DECOMPRESSOR_FILE;
    /// ObjectState name for FU implementation container.
    static const std::string OSNAME_FU_IMPLEMENTATIONS;
    /// ObjectState name for RF implementation container.
    static const std::string OSNAME_RF_IMPLEMENTATIONS;
    /// ObjectState name for IU implementation container.
    static const std::string OSNAME_IU_IMPLEMENTATIONS;
    /// ObjectState name for bus implementation container.
    static const std::string OSNAME_BUS_IMPLEMENTATIONS;
    /// ObjectState name for socket implementation container.
    static const std::string OSNAME_SOCKET_IMPLEMENTATIONS;
    
private:
    /// Vector type for UnitImplementationLocation.
    typedef std::vector<UnitImplementationLocation*> ImplementationTable;

    /// IC/Decoder parameter struct.
    struct Parameter {
        std::string name;
        std::string value;
    };

    UnitImplementationLocation* findImplementation(
        const ImplementationTable& table,
        const std::string& unitName) const;
    void ensureIndexValidity(
        int index, 
        const ImplementationTable& table) const
        throw (OutOfRange);
    void clearState();

    /// FU implementations.
    ImplementationTable fuImplementations_;
    /// RF implementations.
    ImplementationTable rfImplementations_;
    /// IU implementations.
    ImplementationTable iuImplementations_;
    /// BUS implementations.
    ImplementationTable busImplementations_;
    /// SOCKET implementations.
    ImplementationTable socketImplementations_;

    /// Name of the IC/decoder plugin.
    std::string icDecoderPluginName_;
    /// Name of the IC/decoder plugin file.
    std::string icDecoderPluginFile_;
    /// Name of the HDB of the IC/decoder plugin.
    std::string icDecoderHDB_;
    /// Name of the decompressor block file.
    std::string decompressorFile_;
    /// Absolute path to the source IDF file.
    std::string sourceIDF_;
    /// IC/decoder plugin parameters.
    std::vector<Parameter> icDecoderParameters_;
};
}

#endif

    
