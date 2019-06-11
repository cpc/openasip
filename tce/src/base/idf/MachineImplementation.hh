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
 * @file MachineImplementation.hh
 *
 * Declaration of MachineImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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

    void makeImplFilesRelative(const std::vector<std::string>& sPaths);    
    bool checkImplFiles(
        size_t& missingFiles,
        size_t& alternativeFiles);
    bool isLibraryImplFile(
            const std::string& path,
            std::string& resolvedPath);
    
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

    bool checkImplFile(
        const std::vector<std::string>& primarySearchPaths,
        const std::vector<std::string>& secondarySearchPaths,
        std::string& file);
    bool isLibraryImplFile();
    void makeHDBPathRelative(
        const std::vector<std::string>& searchPaths,
        UnitImplementationLocation& implem) const;
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

    /// Implementation files defined in IDF which cannot be located.
    std::vector<std::string> missingFiles_;
    /// Possible alternative file paths for missing implementation files.
    std::vector<std::string> alternativeFiles_;

};
}

#endif

    
