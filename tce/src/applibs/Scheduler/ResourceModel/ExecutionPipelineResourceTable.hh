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
 * @file ExecutionPipelineResourceTable.hh
 *
 * Declaration of the ExecutionPipelineResourceTable class.
 *
 * @author Heikki Kultala 2009 (heikki.kultala-no-spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_EXECUTION_PIPELINE_RESOURCE_TABLE_HH
#define TTA_EXECUTION_PIPELINE_RESOURCE_TABLE_HH

#include <string>
#include <map>
#include <vector>

namespace TTAMachine {
    class FunctionUnit;
}

class ExecutionPipelineResourceTable {
public:
    inline unsigned int numberOfResources() const;
    inline unsigned int pipelineSize() const;
    inline unsigned int maximalLatency() const;

    inline bool operationPipeline(int op, int cycle, int res) const;

    inline int operationIndex(const std::string& opName) const;

    inline bool hasOperation(const std::string& opName) const;

    inline const std::map<int,int>& operationLatency(
        unsigned int operationIndex) const;


    inline const std::string& name() const;

    static const ExecutionPipelineResourceTable& resourceTable(
        const TTAMachine::FunctionUnit& fu);

    static void finalize();
private:
    ExecutionPipelineResourceTable(const TTAMachine::FunctionUnit& fu);

    void setLatency(
        const std::string& opName,
        const int output,
        const int latency);

    void setResourceUse(
        const std::string& opName, const int cycle, const int resIndex);

    std::string name_;

    /// Type for resource vector, represents one cycle of use
    typedef std::vector<bool> ResourceVector;

    /// Type for resource reservation table, resource vector x latency
    typedef std::vector<ResourceVector> ResourceTable;

    typedef std::map<const TTAMachine::FunctionUnit*, 
                     ExecutionPipelineResourceTable*> ResourceTableMap;

    /// Resource and ports vector width, depends on particular FU
    int numberOfResources_;
    /// Maximal latency of operation in FU
    unsigned int maximalLatency_;
    /// Operations supported, name - index to operation pipeline vector
    std::map<std::string, int> operationSupported_;
    // latencies of all operations supported by this FU.
    std::vector<std::map<int,int> > operationLatencies_;
    /// Pipelines for operations
    std::vector<ResourceTable> operationPipelines_;

    /// Contains these tables for all FU's
    static ResourceTableMap allResourceTables_;
};

#include "ExecutionPipelineResourceTable.icc"

#endif
