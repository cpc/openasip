/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file BaseNetlistBlock.hh
 *
 * Declaration of BaseNetlistBlock class.
 *
 * Created on: 20.4.2015
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef BASENETLISTBLOCK_HH
#define BASENETLISTBLOCK_HH

#include <vector>
#include <string>

#include "IGenerationPhases.hh"

#include "Parameter.hh"
#include "ProGeContext.hh"
#include "SignalTypes.hh"
#include "SignalGroupTypes.hh"

namespace ProGe {

class NetlistPort;
class NetlistPortGroup;
class Netlist;
class Parameter;
class ProGeContext;

/*
 * Base netlist block class for all netlist blocks and non-modifiable view of
 * block hierarchy.
 */
class BaseNetlistBlock : public IGenerationPhases {
public:
    friend class NetlistPort;
    friend class NetlistPortGroup;
    friend class Parameter;

    typedef std::vector<BaseNetlistBlock*> BlockContainerType;
    typedef std::vector<Parameter> ParameterContainerType;
    typedef std::vector<NetlistPort*> PortContainerType;
    typedef std::vector<NetlistPortGroup*> PortGroupContainerType;

    BaseNetlistBlock();
    explicit BaseNetlistBlock(BaseNetlistBlock* parent);
    BaseNetlistBlock(
        const std::string& moduleName,
        const std::string& instanceName,
        BaseNetlistBlock* parent = nullptr);
    virtual ~BaseNetlistBlock();

    const std::string& instanceName() const;
    void setInstanceName(const std::string& name);
    const std::string& moduleName() const;
    const std::string name() const;

    virtual size_t subBlockCount() const;
    virtual const BaseNetlistBlock& subBlock(size_t index) const;
    virtual bool hasSubBlock(const std::string& instanceName) const;
    virtual bool isSubBlock(const BaseNetlistBlock& block) const;

    virtual bool hasParameter(const std::string& name) const;
    virtual const Parameter& parameter(const std::string& name) const;
    virtual size_t parameterCount() const;
    virtual const Parameter& parameter(size_t index) const;

    virtual size_t portCount() const;
    virtual const NetlistPort& port(size_t index) const;
    virtual std::vector<const NetlistPort*> portsBy(SignalType type) const;
    virtual const NetlistPort& portBy(SignalType type, size_t index = 0) const;
    virtual bool hasPortsBy(SignalType type) const;
    virtual const NetlistPort* port(
        const std::string& portName,
        bool partialMatch = true) const;

    virtual size_t portGroupCount() const;
    virtual const NetlistPortGroup& portGroup(size_t index) const;
    virtual std::vector<const NetlistPortGroup*> portGroupsBy(
        SignalGroupType type) const;

    virtual const Netlist& netlist() const;

    virtual bool hasParentBlock() const;
    virtual const BaseNetlistBlock& parentBlock() const;

    virtual bool isVirtual() const { return false; };

    virtual void build() override;
    virtual void connect() override;
    virtual void finalize() override;
    virtual void write(
        const Path& targetBaseDir, HDL targetLang = VHDL) const override;
    virtual void writeSelf(
        const Path& targetBaseDir, HDL targetLang = VHDL) const;

    virtual size_t packageCount() const;
    virtual const std::string& package(size_t idx) const;

    PortContainerType& ports() {return ports_;}

    /**
     * Returns true if the netlist block in the block hierarchy is a leaf
     * block.
     *
     * Leaf block are not modifiable: No ports, sub blocks or ports may be
     * added or removed.
     */
    virtual bool isLeaf() const { return true; }

    BaseNetlistBlock* shallowCopy(const std::string& instanceName = "") const;

protected:
    Netlist& netlist();

    virtual NetlistPort& port(size_t index);
    virtual BaseNetlistBlock& subBlock(size_t index);
    virtual BaseNetlistBlock& parentBlock();

    void setModuleName(const std::string& name);
    void addSubBlock(
        BaseNetlistBlock* subBlock,
        const std::string& instanceName = "");
    void deleteSubBlock(BaseNetlistBlock* subBlock);
    void removeSubBlock(BaseNetlistBlock* subBlock);
    NetlistPort* addPort(NetlistPort* port);
    void removePort(NetlistPort* port);
    void addPortGroup(NetlistPortGroup* portGroup);
    void removePortGroup(NetlistPortGroup* portGroup);
    void setParameter(const Parameter& param);
    void addParameter(const Parameter& param);
    Parameter& parameter(const std::string& name);
    NetlistPort* findPort(
        const std::string& portName,
        bool recursiveSearch = false,
        bool partialMatch = true) const;
    void addPackage(const std::string& packageName);

    void connectClocks();
    void connectResets();

private:
    BaseNetlistBlock(const BaseNetlistBlock&);
    BaseNetlistBlock& operator=(const BaseNetlistBlock&);

    virtual void setParent(BaseNetlistBlock* parent);

    /// The reference to parent block of this block.
    BaseNetlistBlock* parent_;
    /// The sub blocks of this netlist block.
    BlockContainerType subBlocks_;
    /// The parameters of the block.
    ParameterContainerType parameters_;
    /// The ports of the block.
    PortContainerType ports_;
    /// The ports of the block.
    PortGroupContainerType portGroups_;
    /// The netlist of the block.
    Netlist* netlist_;
    /// The instance name of the block.
    std::string instanceName_;
    /// The module name of the block.
    std::string moduleName_;
    /// The referenced packages by the module.
    std::vector<std::string> packages_;
};

/**
 * Compares 2 NetlistBlock's names lexicographically(dictionary order).
 *
 * Can be used to organize containers of type NetlistBlock to dictionary
 * order according to their instanceNames + moduleNames.
 * @param a the first NetlistBlock to compare.
 * @param b the second NetlistBlock to compare.
 * @return true, if a comes before b in dictionary order.
 */
class NetlistBlockNameComparator {
public:
    bool operator () (
        const BaseNetlistBlock* a,
        const BaseNetlistBlock* b) const {
        return (a->instanceName() + a->moduleName()) <
                (b->instanceName() + b->moduleName());
    }
};

} /* namespace ProGe */

#endif /* BASENETLISTBLOCK_HH */
