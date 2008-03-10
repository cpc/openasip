/**
 * @file InterpreterContext.hh
 *
 * Declaration of InterpreterContext interface class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note reviewed 27 May 2004 by pj, jn, vpj, ll
 * @note rating: green
 */

#ifndef TTA_INTERPRETER_CONTEXT_HH
#define TTA_INTERPRETER_CONTEXT_HH

/**
 * Container for data that is handled by CustomCommands.
 */
class InterpreterContext {
public:
    InterpreterContext();
    virtual ~InterpreterContext();
};

#endif
