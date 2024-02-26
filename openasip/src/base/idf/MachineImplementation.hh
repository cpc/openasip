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
#include "FUGenerated.hh"
#include "RFGenerated.hh"


namespace IDF {

class UnitImplementationLocation;

/**
 * Represents the implementation of a machine defined in an IDF.
 */
class MachineImplementation : public Serializable {
public:
    MachineImplementation();

    MachineImplementation(const ObjectState* state);

    virtual ~MachineImplementation();

    std::string sourceIDF() const;

    std::string icDecoderPluginName() const;
    bool hasICDecoderPluginName() const;
    std::string icDecoderPluginFile() const;

    bool hasICDecoderPluginFile() const;
    std::string icDecoderHDB() const;
    bool hasICDecoderHDB() const;
    std::string decompressorFile() const;
    bool hasDecompressorFile() const;

    void setICDecoderPluginName(const std::string& name);
    void setICDecoderPluginFile(const std::string& file);
    void setICDecoderHDB(const std::string& file);
    void setDecompressorFile(const std::string& file);

    unsigned icDecoderParameterCount() const;
    std::string icDecoderParameterName(unsigned param) const;
    std::string icDecoderParameterValue(const std::string& name) const;
    std::string icDecoderParameterValue(unsigned param) const;
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

    FUImplementationLocation& fuImplementation(const std::string& fu) const;
    RFImplementationLocation& rfImplementation(const std::string& rf) const;
    RFImplementationLocation& iuImplementation(const std::string& iu) const;
    BusImplementationLocation& busImplementation(const std::string& bus) const;
    SocketImplementationLocation& socketImplementation(
        const std::string& socket) const;

    FUImplementationLocation& fuImplementation(int index) const;
    RFImplementationLocation& rfImplementation(int index) const;
    RFImplementationLocation& iuImplementation(int index) const;
    BusImplementationLocation& busImplementation(int index) const;
    SocketImplementationLocation& socketImplementation(int index) const;

    void addFUImplementation(FUImplementationLocation* implementation);
    void addRFImplementation(RFImplementationLocation* implementation);
    void addIUImplementation(RFImplementationLocation* implementation);
    void addBusImplementation(BusImplementationLocation* implementation);
    void addSocketImplementation(SocketImplementationLocation* implementation);

    void removeFUImplementation(const std::string& unitName);
    void removeRFImplementation(const std::string& unitName);
    void removeIUImplementation(const std::string& unitName);
    void removeBusImplementation(const std::string& unitName);
    void removeSocketImplementation(const std::string& unitName);

    // methods from Serializable interface
    virtual void loadState(const ObjectState* state);
    virtual ObjectState* saveState() const;

    static MachineImplementation* loadFromIDF(const std::string& idfFileName);

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
    /// ObjectState name for FU generations container.
    static const std::string OSNAME_FU_GENERATED;
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

    const std::vector<FUGenerated>& FUGenerations() const;
    std::vector<FUGenerated>& FUGenerations();
    bool hasFUGeneration(const std::string& name) const;
    void removeFuGeneration(const std::string& name);
    void addFuGeneration(const FUGenerated& fug);
    const std::vector<RFGenerated>& RFGenerations() const;
    std::vector<RFGenerated>& RFGenerations();
    bool hasRFGeneration(const std::string& name) const;
    void removeRFGeneration(const std::string& name);
    void addRFGeneration(const RFGenerated& rfg);


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
    void ensureIndexValidity(int index, const ImplementationTable& table) const;
    void clearState();

    /// Generated FUs.
    std::vector<FUGenerated> fuGenerated_;
    /// Generated RFs.
    std::vector<RFGenerated> RFGenerated_;

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

    
