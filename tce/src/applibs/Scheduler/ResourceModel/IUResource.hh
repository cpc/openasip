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
 * @file IUResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the IUResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_IURESOURCE_HH
#define TTA_IURESOURCE_HH

#include <string>
#include <vector>

#include "SchedulingResource.hh"
#include "Terminal.hh"

/**
 * An interface for scheduling resources of Resource Model
 * The derived class IUResource
 */

class IUResource : public SchedulingResource {
public:
    virtual ~IUResource();
    IUResource(
        const std::string& name,
        const int registers,
        const int width,
        const int latency,
        const bool signExtension);

    virtual bool isInUse(const int cycle) const;
    virtual bool isAvailable(const int cycle) const;
    virtual bool canAssign(const int, const MoveNode&)const;
    virtual bool canAssign(
        const int defCycle,
        const int useCycle,
        const MoveNode& node) const;
    virtual void assign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void assign(
        const int defCycle,
        const int useCycle,
        MoveNode& node,
        int& index)
        throw (Exception);
    virtual void unassign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual bool isIUResource() const;

    int registerCount() const;
    TTAProgram::Terminal* immediateValue(const MoveNode& node) const
        throw (KeyNotFound);
    int immediateWriteCycle(const MoveNode& node) const
        throw (KeyNotFound);    
    int width() const;

protected:
    virtual bool validateDependentGroups();
    virtual bool validateRelatedGroups();
    void setRegisterCount(const int registers);

private:
    // Stores first and last cycle register is marked for use
    // also the actual value of constant to be stored in register
    struct ResourceRecordType{
        ResourceRecordType();
        ~ResourceRecordType();
        int         definition_;
        int         use_;
        TTAProgram::Terminal*    immediateValue_;
    };
    // For each register, there are several def-use combinations
    // non overlapping
    typedef std::vector<ResourceRecordType*> ResourceRecordVectorType;
    // Copying forbidden
    IUResource(const IUResource&);
    // Assignment forbidden
    IUResource& operator=(const IUResource&);
    int findAvailable(const int defCycle, const int useCycle) const;

    std::vector<ResourceRecordVectorType> resourceRecord_;
    // Number of registers in given IU
    int registerCount_;
    // The bit width of registers in IU
    int width_;
    // The latency of IU
    int latency_;
    //Extention of IU, true == sign extends, false == zero extends
    bool signExtension_;
};
#endif
