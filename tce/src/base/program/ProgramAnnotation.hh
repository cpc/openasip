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
 * @file ProgramAnnotation.hh
 *
 * Declaration of ProgramAnnotation class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGRAM_ANNOTATION_HH
#define TTA_PROGRAM_ANNOTATION_HH

#include <string>
#include <vector>

#include "BaseType.hh"

namespace TTAProgram {

/**
 * ProgramAnnotations provide means for adding arbitrary "annotations" to
 * moves or immediate values in instructions of TTA programs.
 *
 * This is a POM API to support the corresponding TPEF feature.
 */
class ProgramAnnotation {
public:
    /// the ID in TPEF is 24 bits, here enum
    enum Id {
        ANN_FIRST_RESERVED              = 0x00000000, ///<First reserved annot.
        ANN_LAST_RESERVED               = 0x0000FFFF, ///<Last reserved annot.
        ANN_STACKFRAME_BEGIN            = 0x00011000, ///<First stackframe ann.
        ANN_STACKFRAME_LVAR_ALLOC       = 0x00011010, ///<Local variable alloc
        ANN_STACKFRAME_LVAR_DEALLOC     = 0x00011011, ///<Local var dealloc
        ANN_STACKFRAME_LVAR_RA_ALLOC    = 0x00011012, ///<Local var+RA alloc
        ANN_STACKFRAME_LVAR_RA_DEALLOC  = 0x00011013, ///<Local var+RA dealloc
        ANN_STACKFRAME_RA_ALLOC         = 0x00011020, ///<Return Address alloc
        ANN_STACKFRAME_RA_DEALLOC       = 0x00011021, ///<Return Addr dealloc
        ANN_STACKFRAME_RA_SAVE          = 0x00011022, ///<Return Address save
        ANN_STACKFRAME_RA_RESTORE       = 0x00011023, ///<Return Addr restore
        ANN_STACKFRAME_GPR_SAVE_BEGIN   = 0x00011030, ///<Used to count saves
        ANN_STACKFRAME_GPR_SAVE         = 0x00011031, ///<GPR save by pushing
        ANN_STACKFRAME_GPR_SAVE_REG     = 0x00011032,
        ANN_STACKFRAME_GPR_RESTORE_REG  = 0x00011038,
        ANN_STACKFRAME_GPR_RESTORE      = 0x00011039, ///<GPR restore by pop
        ANN_STACKFRAME_FPR_SAVE_BEGIN   = 0x00011040, ///<Used to count saves
        ANN_STACKFRAME_FPR_SAVE         = 0x00011041, ///<FPR save by  pushing
        ANN_STACKFRAME_FPR_SAVE_REG     = 0x00011042, ///<FPR save by  pushing
        ANN_STACKFRAME_FPR_RESTORE_REG  = 0x00011048, ///<FPR restore by pop
        ANN_STACKFRAME_FPR_RESTORE      = 0x00011049, ///<FPR restore by pop
        ANN_STACKFRAME_OUT_PARAM_ALLOC  = 0x00011050, ///<outgoing param alloc
        ANN_STACKFRAME_OUT_PARAM_DEALLOC= 0x00011051, ///<outgoing param deall.
        ANN_STACKFRAME_PROCEDURE_RETURN = 0x00011060, ///<precedure return jmp

        ANN_STACKFRAME_STACK_INIT       = 0x00011070, ///<Stack initialization
        ANN_STACKFRAME_UNKNOWN          = 0x00011FFE, ///<Unknown SF-related
        ANN_STACKFRAME_END              = 0x00011FFF, ///<Last stackfame annot.

        ANN_STACKUSE_BEGIN              = 0x00012000,
        ANN_STACKUSE_OUT_PARAM          = 0x00012010,///<output parameter.
        ANN_STACKUSE_LOCAL_VARIABLE     = 0x00012011,///<local variable
        ANN_STACKUSE_RA                 = 0x00012012,///<ra, old frontend
        ANN_STACKUSE_IN_PARAM           = 0x00012013,///<input parameter
        ANN_STACKUSE_SAVED_GPR          = 0x00012014,///<saved GPR
        ANN_STACKUSE_SAVED_FPR          = 0x00012015,///<saved FPR
        ANN_STACKUSE_SPILL              = 0x00012016,///<spilled variable
        ANN_STACKUSE_RA_SAVE            = 0x00012017,///<ra, new frontend
        ANN_STACKUSE_FP_SAVE            = 0x00012018,///<frame ptr save/load
        ANN_STACKUSE_END                = 0x00012FFF,
        
        ANN_REGISTER_BEGIN              = 0x00020000,
        ANN_REGISTER_RV_SAVE            = 0x00020010,///<Save to RV register
        ANN_REGISTER_RV_READ            = 0x00020011,///<Read from RV reg
        ANN_REGISTER_SP_SAVE            = 0x00020020,///<save to Stack pointer 
        ANN_REGISTER_SP_READ            = 0x00020021,///<Stack Pointer read
        ANN_REGISTER_IPARAM_SAVE        = 0x00020030,///<Save to int param reg
        ANN_REGISTER_IPARAM_READ        = 0x00020031,///<Read from int param
        ANN_REGISTER_FP_SAVE            = 0x00020040,///<Frame Pointer save
        ANN_REGISTER_FP_READ            = 0x00020041,///<Frame Pointer read
        ANN_REGISTER_END                = 0x0002FFFF,

        /// Candidate units can be passed for resource manager for
        /// choosing the source/destination unit of the move. If this
        /// annotation is present, the move's source or destination unit 
        /// should be chosen from the set given in annotation
        /// payload data string. Naturally, there can be multiple annotations
        /// of the same type to define multiple candidates for choosing
        /// the unit for source/destination.
        ANN_ALLOWED_UNIT_SRC          = 0x00030000,///<Src. unit candidate
        ANN_ALLOWED_UNIT_DST          = 0x00030001,///<Dst. unit candidate

        ANN_CONN_CANDIDATE_UNIT_SRC          = 0x00030005,///<Src. unit candidate
        ANN_CONN_CANDIDATE_UNIT_DST          = 0x00030006,///<Dst. unit candidate

        ANN_REJECTED_UNIT_SRC          = 0x00030007,///<Src. unit rejected
        ANN_REJECTED_UNIT_DST          = 0x00030008,///<Dst. unit rejected

        /// A reg to reg move that was added because of missing connectivity 
        /// between the original target and destination (a temp move).
        ANN_CONNECTIVITY_MOVE           = 0x00030002,

        ANN_REQUIRES_LIMM               = 0x00030003,

        /// debugging info annotations

        /// Full path to the program source code file the annotated move
        /// originates from
        ANN_DEBUG_SOURCE_CODE_PATH      = 0x00040000,

        /// The line number in the source code file the annotated move
        /// originates from
        ANN_DEBUG_SOURCE_CODE_LINE      = 0x00040001,


        /* Moves with this annotation are from an OpenCL work item at
           the given local ID "offset". The ID does not necessarily map 
           to real work item local ids, but depends on the work group 
           loop parallelization level ("warp size") etc. 

           The local ID components are stored to the same integer payload 
           to make ID comparisons fast and easy:

           (z & 0x0FF) | ((y & 0x0FF) << 8) | ((x & 0x0FF) << 16)

           Thus, the maximum allowed WI "offset" is (255, 255, 255). */

        ANN_OPENCL_WORK_ITEM_ID        =  0x00050000,
        // In case of vector memory access, the work item id from the
        // memory access is for the first of the work items in the vector
        // access. Also the last of the work item ID in the vector access is
        // necessary.
        ANN_OPENCL_WORK_ITEM_ID_LAST        =  0x00050001,
        
        ANN_JUMP_TO_NEXT                = 0x00500002,

        /// An instruction annotated with this annotation is the first
        /// instruction of a basic block in a loop with the 
        /// trip count stored in the annotation payload.
        ANN_LOOP_TRIP_COUNT             = 0x00600000,
        /// An instruction annotated with this annotation is the first
        /// instruction of a basic block in an inner loop.
        ANN_LOOP_INNER,

        /// information retrieved (from LLVM) about a pointer access
        ANN_POINTER_NAME                = 0x00700000,
        ANN_POINTER_OFFSET              = 0x00700001,
        ANN_POINTER_NOALIAS             = 0x00700002,
        ANN_POINTER_ADDR_SPACE          = 0x00700003,

        /// an illegal annotation ID (the id is only 24 bits, this has more
        /// meaningful bits)        
        ANN_UNDEF_ID                    = 0xFF000000

    };

    ProgramAnnotation(Id id, int value);
    ProgramAnnotation(Id id, const std::string& data = "");
    ProgramAnnotation(Id id, const std::vector<Byte>& payload);

    ~ProgramAnnotation();
    
    ProgramAnnotation::Id id() const;

    std::string stringValue() const;

    inline void setStringValue(const std::string& data) {
        payload_ = std::vector<Byte>(data.begin(), data.end());
    };

    int intValue() const;
    void setIntValue(int value);

    const std::vector<Byte>& payload() const;
private:
    /// the id
    Id id_;
    /// the payload data
    std::vector<Byte> payload_;
};

}

#endif
