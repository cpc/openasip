/**
 * @file Model.hh
 *
 * Declaration of Model class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 */

#ifndef TTA_MODEL_HH
#define TTA_MODEL_HH

#include <vector>
#include <string>
#include <deque>
#include "Exception.hh"

class wxDocument;
class ModelObserver;

namespace TTAMachine {
    class Machine;
}

/**
 * An interface for modifying the Machine Object Model.
 */
class Model {
public:
    Model();
    Model(const std::string& fileName)
        throw (SerializerException, ObjectStateLoadingException);
    ~Model();

    TTAMachine::Machine* getMachine();
    void undo();
    void redo();
    bool canUndo();
    bool canRedo();
    void addObserver(ModelObserver* observer);
    void notifyObservers();
    void pushToStack();
    void popFromStack();

private:

    /// Maximum undo stack size.
    static const unsigned int UNDO_STACK_MAX_SIZE;

    /// Machine assigned for this Model. Singleton within a Model.
    TTAMachine::Machine* machine_;

    /// Table of model observers.
    typedef std::vector<ModelObserver*> ObserverTable;

    /// Model observers.
    ObserverTable observers_;

    /// Copying not allowed.
    Model(const Model&);
    /// Assignment not allowed.
    Model& operator=(const Model&);

    /// Maximum size of the undo stack.
    unsigned undoStackSize_;
    /// Machine stack for undo functionality.
    typedef std::deque<TTAMachine::Machine*> UndoStack;
    UndoStack undoStack_;
    /// Undone modification cache for the redo funciton.
    TTAMachine::Machine* undone_;
};

#endif
