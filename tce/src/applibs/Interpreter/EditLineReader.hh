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
 * @file EditLineReader.hh
 *
 * Declaration of EditLineReader class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 2 June 2004 by jm, pj, tr, jn
 * @note rating: yellow
 */

#ifndef TTA_EDIT_LINE_READER_HH
#define TTA_EDIT_LINE_READER_HH

extern "C" {
#include <histedit.h>
}
#include <string>
#include <map>

#include "LineReader.hh"

/**
 * LineReader implementation using libedit library.
 *
 * libedit offers functionality to edit a line as well as command history
 * browsing functionality. In other words, user can edit the line he is 
 * writing as well as browse history of commands.
 */
class EditLineReader : public LineReader {
public:
    explicit EditLineReader(std::string program = "");
    virtual ~EditLineReader();

    virtual void initialize(
        std::string defPrompt = "",
        FILE* in = stdin, 
        FILE* out = stdout, 
        FILE* err = stderr);
    virtual std::string readLine(std::string prompt = "")
        throw (ObjectNotInitialized, EndOfFile);
    virtual char charQuestion(
        std::string question, 
        std::string allowedChars,
        bool caseSensitive = false,
        char defaultAnswer = '\0') throw (ObjectNotInitialized);
  
private:
    /// value_type for map.
    typedef std::map<EditLine*, EditLineReader*>::value_type ValType;
    /// Iterator for map.
    typedef std::map<EditLine*, EditLineReader*>::iterator MapIt;
    
    /// Copying not allowed.
    EditLineReader(const EditLineReader&);
    /// Assignment not allowed.
    EditLineReader& operator=(const EditLineReader&);

    static char* wrapperToCallPrompt(EditLine* edit);
    char* prompt();
    void updateHistory(const char* c);

    /// Line reader prompt.
    char* prompt_;
    /// The name of the invocating program.
    std::string program_;
    /// EditLine instance.
    EditLine* editLine_;
    /// History instance.
    History* history_;
    /// Map containing all (EditLine*, EditLineReader*) pairs to make prompt
    /// printing possible. This map offers to callback function
    /// 'wrapperToCallPrompt' a right instance of EditLineReader. This
    /// instance then returns the right prompt.
    static std::map<EditLine*, EditLineReader*> lineReaders_;

    /// Input stream is saved for end-of-file checking.
    FILE* in_;
};

#endif
