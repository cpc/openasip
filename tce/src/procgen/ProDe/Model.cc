/**
 * @file Model.cc
 *
 * Definition of Model class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 */

#include "Application.hh"
#include "Model.hh"
#include "ModelConstants.hh"
#include "Machine.hh"
#include "ADFSerializer.hh"
#include "ModelObserver.hh"
#include "ImmediateUnit.hh"
#include "Socket.hh"
#include "Bus.hh"
#include "FUPort.hh"
#include "Port.hh"
#include "Segment.hh"
#include "Guard.hh"
#include "ProDe.hh"
#include "ProDeOptions.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 */
Model::Model() : machine_(new Machine()), undone_(NULL) {
}


/**
 * The Destructor.
 */
Model::~Model() {
    // delete machine object states in the undo stack
    while (undoStack_.size() != 0) {
        Machine* machine = undoStack_.front();
        undoStack_.pop_front();
        delete machine;
    }
}


/**
 * The Constructor.
 *
 * Creates model from a specified file.
 *
 * @param fileName The file name of the machine description to open.
 */
Model::Model(const std::string& fileName)
    throw (SerializerException, ObjectStateLoadingException) :
    undone_(NULL) {

    // read Machine from fileName and set it to machine_
    ADFSerializer reader;
    reader.setSourceFile(fileName);
    machine_ = reader.readMachine();
}


/**
 * Returns the Machine.
 */
Machine*
Model::getMachine() {
    return machine_;
}


/**
 * Restores Machine to the state before the last modification.
 */
void
Model::undo() {

    // delete redo cache
    if (undone_ != NULL) {
        delete undone_;
        undone_ = NULL;
    }

    // add machine to the redo cache before undo
    undone_ = new Machine(*machine_);

    // pop last machine from the undo stack and set is as the machine
    popFromStack();
}


/**
 * Redoes last undoed modification.
 */
void
Model::redo() {

    assert (undone_ != NULL);

    Machine* undone = undone_;
    undone_ = NULL;

    // push machine to the undo stack before redoing
    pushToStack();

    // replace machine from the redo cache
    if (machine_ != NULL) {
	delete machine_;
    }

    machine_ = undone;
    notifyObservers();
}

/**
 * Adds an observer for this Model.
 *
 * @param observer The observer to add.
 */
void
Model::addObserver(ModelObserver* observer) {
    observers_.push_back(observer);
}


/**
 * Notifies all observers of this Model to update themselves.
 */
void
Model::notifyObservers() {
    for (ObserverTable::iterator i = observers_.begin();
         i != observers_.end(); i++) {
        (*i)->update();
    }
}


/**
 * Pushes the current machine to the undo stack.
 */
void
Model::pushToStack() {

    // Redo is not available after modifications, clear redo cache
    if (undone_ != NULL) {
        delete undone_;
        undone_ = NULL;
    }

    // if undo stack size is at maximum, delete oldest machine from the stack
    while (undoStack_.size() > 0 &&
           undoStack_.size() >=
           unsigned(wxGetApp().options()->undoStackSize())) {

        Machine* last = undoStack_.back();
        undoStack_.pop_back();
        delete last;
    }

    // push copy of the current machine to the undo stack
    undoStack_.push_front(new Machine(*machine_));
}


/**
 * Replaces the current machine with the one that is in the top of the
 * undo stack.
 */
void
Model::popFromStack() {

    if (machine_ != NULL) {
	delete machine_;
    }

    machine_ = undoStack_.front();
    undoStack_.pop_front();
    notifyObservers();
}


/**
 * Returns true if the undo stack is not empty.
 *
 * @return True, if the undo stack is not empty.
 */
bool
Model::canUndo() {
    if (undoStack_.size() > 0) {
        return true;
    }
    return false;
}



/**
 * Returns true if the last undone command can be redone.
 *
 * @return True, if the last undone command can be redone.
 */
bool
Model::canRedo() {
    if (undone_ != NULL) {
        return true;
    }
    return false;
}
