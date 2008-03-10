/**
 * @file ProgramOperation.hh
 *
 * Declaration of ProgramOperation class.
 *
 * @author Heikki Kultala 2006 (hkultala@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TCE_PROGRAM_OPERATION_HH
#define TCE_PROGRAM_OPERATION_HH

#include <string>
#include <map>
#include "Exception.hh"
#include "Operation.hh"
#include "MoveNode.hh"
#include "MoveNodeSet.hh"


/**
 * Represents a single execution of an operation in a program.
 */
class ProgramOperation {
public:
    ProgramOperation(Operation &operation);
    ProgramOperation();
    ~ProgramOperation();

    void addNode(MoveNode& node) throw (IllegalParameters);
    void addInputNode(MoveNode& node) throw (IllegalParameters);
    void addOutputNode(MoveNode& node) throw (IllegalParameters);

    void removeOutputNode(MoveNode& node) throw (IllegalRegistration);
    void removeInputNode(MoveNode& node) throw (IllegalRegistration);

    bool isComplete();
    bool isReady();
    bool isMultinode();
    bool isAssigned();
    bool areInputsAssigned();

    MoveNode& opcodeSettingNode() throw (InvalidData);
    MoveNodeSet& inputNode(int in) throw (OutOfRange,KeyNotFound);
    MoveNodeSet& outputNode(int out) throw (OutOfRange,KeyNotFound);

    bool hasOutputNode(int out) const;
    bool hasInputNode(int in) const;

    Operation& operation() const;

    int inputMoveCount() const;
    int outputMoveCount() const;

    MoveNode& inputMove(int index) const;
    MoveNode& outputMove(int index) const;

    MoveNode& triggeringMove() const throw (InvalidData);

    std::string toString() const;

    unsigned int poId() const;
private:
    // copying forbidden
    ProgramOperation(const ProgramOperation&);
    // assignment forbidden
    ProgramOperation& operator=(const ProgramOperation&);
    // OSAL Operation this program operation is executing.
    Operation& operation_;
    // map from operand index to MoveNodeSet
    std::map<int,MoveNodeSet*> inputMoves_;
    // map from operand index to MoveNodeSet
    std::map<int,MoveNodeSet*> outputMoves_;
    // all input moves
    std::vector<MoveNode*> allInputMoves_;
    // all output moves
    std::vector<MoveNode*> allOutputMoves_;
    unsigned int poId_;
    static unsigned int idCounter;
};

#endif
