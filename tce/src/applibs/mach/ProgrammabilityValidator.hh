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
/*
 * @file ProgrammabilityValidator.cc
 *
 * Header file for ProgrammabilityValidator.
 *
 * @author Jari M�ntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGRAMMABILITY_VALIDATOR_HH
#define TTA_PROGRAMMABILITY_VALIDATOR_HH

#include <string>
#include <set>
#include <vector>
#include <map>

#include "Exception.hh"

using std::map;
using std::set;
using std::vector;
using std::string;
using std::pair;

namespace TTAMachine {
    class Machine;
    class RegisterGuard;
    class RegisterFile;
    class Segment;
    class Port;
    class FunctionUnit;
    class HWOperation;
    class Socket;
}

namespace TTAProgram {
    class Program;
    class Procedure;
}

namespace TPEF {
    class Binary;
}

using namespace TTAMachine;
using namespace TTAProgram;

class ProgrammabilityValidatorResults;
class MinimalOpSetCheck;

class ProgrammabilityValidator {
public:
    
    /// Error codes for different errors.
    enum ErrorCode {
        /// Machine don't have all the operations needed to support ANSI C.
        OPERATION_MISSING_FROM_THE_PRIMITIVE_OPERATION_SET,
        /// Machine should have one and only one boolean register.
        BOOLEAN_REGISTER_ERROR,
        /// Bus is missing a boolean register file guard.
        BOOLEAN_REGISTER_GUARD_MISSING,
        /// Global connection register could not be determined.
        GLOBAL_CONNECTION_REGISTER_NOT_FOUND,
        /// Connection is missing to or from the global connection register
        MISSING_CONNECTION
    };

    ProgrammabilityValidator(const Machine& machine);
    ~ProgrammabilityValidator();

    ProgrammabilityValidatorResults* validate();
    ProgrammabilityValidatorResults* validate(
        const std::set<ErrorCode>& errorsToCheck);  

    void checkPrimitiveSet(ProgrammabilityValidatorResults& results) const;
    void findConnections();
    void printConnections();
    void printRegisterConnections();
    const RegisterFile* findGlobalConnectionRegister();
    void findGlobalRegisterConnections();
    void printNotConnectedSockets(const RegisterFile* rf);
    void searchMissingRFConnections(const RegisterFile* rf,
                                    vector<pair<string,
                                    string> >& toConnections,
                                    vector<pair<string,
                                    string> >& fromConnections);
    bool checkBooleanRegister(
        ProgrammabilityValidatorResults& results);
    TPEF::Binary* profile(ProgrammabilityValidatorResults& results);

private:
    void
    addConnectionToProgram(const Port* sourcePort,
                           const Port* destPort,
                           Segment* segment,
                           int& counter);
    
    void 
    addConnections(set<Segment*>& newConnections,
                   set<Segment*>& connections);
    void
    addOperationConnections(const FunctionUnit* fu,
                            set<HWOperation*>& operations);

    void
    listConnections(set<Segment*>& connections,
                    const Socket* socket);
    void
    printConnection(const Port* sourcePort,
                    const Port* destPort,
                    Segment* segment,
                    int& counter);
    
    void
    findRegisterGuard(Segment* segment, RegisterGuard*& guard);

    void
    addDirectConnection(const Port* sourcePort,
                        const Port* destPort,
                        Segment* segment);

    void
    addGlobalRegisterConnection(const Port* sourcePort,
                                const Port* destPort,
                                Segment* segment);

    void addDirectConnectionsToProgram();
    void addGlobalRegisterConnectionsToProgram();

    /// the target machine
    const Machine& machine_;
    /// Program where moves are generated
    Program* program_;
    /// Procedure of the program
    Procedure* procedure_;
    /// Boolean register file of the machine
    RegisterFile* booleanRegister_;
    /// The global connection register of the machine
    RegisterFile* globalConnectionRegister_;
    /// Minimal operation checker
    MinimalOpSetCheck* minimalOpSetCheck_;
    
    /// All inputs found in the machine are stored here.
    vector<pair<const Port*, set<Segment*> > > inputs;

    /// Struct of connections. 
    struct Connections {
        set<Segment*> inputs;
        set<Segment*> outputs;
        set<Segment*> triggeredInputs;
        set<HWOperation*> operations;
        int registers;
    };

    /// Moves are generated from these.
    struct ConnectionSet {
        const Port* sourcePort;
        const Port* destPort;
        Segment* segment;
    };

    /// The counter of direct connections added to the program.
    int directCounter;
    /// The counter of global register file connections added to the program.
    int gcrCounter;

    /// Set of connections to ensure that same Move is not done in multiple
    /// buses
    set<pair<const Port*, const Port*> > allConnections;
    set<pair<const Port*, const Port*> > gcrConnections;

    /// Set of unit connections to ensure that only one connection per
    /// register file is added
    set<pair<const Port*, const RegisterFile*> > portRegisterConnections;
    set<pair<const RegisterFile*, const Port*> > registerPortConnections;
    set<pair<const RegisterFile*, const RegisterFile*> > registerRegisterConnections;

    set<pair<const Port*, const RegisterFile*> > portGcrConnections;
    set<pair<const RegisterFile*, const Port*> > gcrPortConnections;
    set<pair<const RegisterFile*, const RegisterFile*> > gcrRegisterConnections;
 
    /// Storage of all the Moves that are found in the machine
    vector<ConnectionSet> directConnections;
    /// Storage of all Moves to and from the Global Connection Register
    vector<ConnectionSet> globalRegisterConnections;
    
    /// Function unit connections
    map<string, map<const Socket*, Connections> > FUConnections;
    /// Global control unit connections
    map<string, map<const Socket*, Connections> > GCUConnections;
    /// Register file connections
    map<string, map<const Socket*, Connections> > RFConnections;
    /// Immediate unit connections
    map<string, map<const Socket*, Connections> > IMMConnections;

    /// type definition for going through connection maps
    typedef map<string, map<const Socket*, Connections> >::iterator mapIter;
    /// type definition for going through connection sets
    typedef set<pair<const Port*, const Port*> >::iterator portSetIter;
    typedef set<pair<const RegisterFile*, const RegisterFile*> >::iterator registerRegisterSetIter;
    typedef set<pair<const Port*, const RegisterFile*> >::iterator portRegisterSetIter;
    typedef set<pair<const RegisterFile*, const Port*> >::iterator registerPortSetIter;
};

#endif
