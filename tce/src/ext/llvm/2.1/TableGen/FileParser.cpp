/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse Fileparse
#define yylex   Filelex
#define yyerror Fileerror
#define yylval  Filelval
#define yychar  Filechar
#define yydebug Filedebug
#define yynerrs Filenerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INT = 258,
     BIT = 259,
     STRING = 260,
     BITS = 261,
     LIST = 262,
     CODE = 263,
     DAG = 264,
     CLASS = 265,
     DEF = 266,
     MULTICLASS = 267,
     DEFM = 268,
     FIELD = 269,
     LET = 270,
     IN = 271,
     CONCATTOK = 272,
     SHLTOK = 273,
     SRATOK = 274,
     SRLTOK = 275,
     STRCONCATTOK = 276,
     INTVAL = 277,
     ID = 278,
     VARNAME = 279,
     STRVAL = 280,
     CODEFRAGMENT = 281
   };
#endif
/* Tokens.  */
#define INT 258
#define BIT 259
#define STRING 260
#define BITS 261
#define LIST 262
#define CODE 263
#define DAG 264
#define CLASS 265
#define DEF 266
#define MULTICLASS 267
#define DEFM 268
#define FIELD 269
#define LET 270
#define IN 271
#define CONCATTOK 272
#define SHLTOK 273
#define SRATOK 274
#define SRLTOK 275
#define STRCONCATTOK 276
#define INTVAL 277
#define ID 278
#define VARNAME 279
#define STRVAL 280
#define CODEFRAGMENT 281




/* Copy the first part of user declarations.  */
#line 14 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"

#include "Record.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Streams.h"
#include <algorithm>
#include <cstdio>
#define YYERROR_VERBOSE 1

int yyerror(const char *ErrorMsg);
int yylex();

namespace llvm {
  struct MultiClass {
    Record Rec;  // Placeholder for template args and Name.
    std::vector<Record*> DefPrototypes;
    
    MultiClass(const std::string &Name) : Rec(Name) {}
  };

  
static std::map<std::string, MultiClass*> MultiClasses;
  
extern int Filelineno;
static MultiClass *CurMultiClass = 0;    // Set while parsing a multiclass.
static std::string *CurDefmPrefix = 0;   // Set while parsing defm.
static Record *CurRec = 0;
static bool ParsingTemplateArgs = false;

typedef std::pair<Record*, std::vector<Init*>*> SubClassRefTy;

struct LetRecord {
  std::string Name;
  std::vector<unsigned> Bits;
  Init *Value;
  bool HasBits;
  LetRecord(const std::string &N, std::vector<unsigned> *B, Init *V)
    : Name(N), Value(V), HasBits(B != 0) {
    if (HasBits) Bits = *B;
  }
};

static std::vector<std::vector<LetRecord> > LetStack;


extern std::ostream &err();

/// getActiveRec - If inside a def/class definition, return the def/class.
/// Otherwise, if within a multidef, return it.
static Record *getActiveRec() {
  return CurRec ? CurRec : &CurMultiClass->Rec;
}

static void addValue(const RecordVal &RV) {
  Record *TheRec = getActiveRec();
  
  if (RecordVal *ERV = TheRec->getValue(RV.getName())) {
    // The value already exists in the class, treat this as a set...
    if (ERV->setValue(RV.getValue())) {
      err() << "New definition of '" << RV.getName() << "' of type '"
            << *RV.getType() << "' is incompatible with previous "
            << "definition of type '" << *ERV->getType() << "'!\n";
      exit(1);
    }
  } else {
    TheRec->addValue(RV);
  }
}

static void addSuperClass(Record *SC) {
  if (CurRec->isSubClassOf(SC)) {
    err() << "Already subclass of '" << SC->getName() << "'!\n";
    exit(1);
  }
  CurRec->addSuperClass(SC);
}

static void setValue(const std::string &ValName, 
                     std::vector<unsigned> *BitList, Init *V) {
  if (!V) return;

  Record *TheRec = getActiveRec();
  RecordVal *RV = TheRec->getValue(ValName);
  if (RV == 0) {
    err() << "Value '" << ValName << "' unknown!\n";
    exit(1);
  }

  // Do not allow assignments like 'X = X'.  This will just cause infinite loops
  // in the resolution machinery.
  if (!BitList)
    if (VarInit *VI = dynamic_cast<VarInit*>(V))
      if (VI->getName() == ValName)
        return;
  
  // If we are assigning to a subset of the bits in the value... then we must be
  // assigning to a field of BitsRecTy, which must have a BitsInit
  // initializer...
  //
  if (BitList) {
    BitsInit *CurVal = dynamic_cast<BitsInit*>(RV->getValue());
    if (CurVal == 0) {
      err() << "Value '" << ValName << "' is not a bits type!\n";
      exit(1);
    }

    // Convert the incoming value to a bits type of the appropriate size...
    Init *BI = V->convertInitializerTo(new BitsRecTy(BitList->size()));
    if (BI == 0) {
      V->convertInitializerTo(new BitsRecTy(BitList->size()));
      err() << "Initializer '" << *V << "' not compatible with bit range!\n";
      exit(1);
    }

    // We should have a BitsInit type now...
    assert(dynamic_cast<BitsInit*>(BI) != 0 || (cerr << *BI).stream() == 0);
    BitsInit *BInit = (BitsInit*)BI;

    BitsInit *NewVal = new BitsInit(CurVal->getNumBits());

    // Loop over bits, assigning values as appropriate...
    for (unsigned i = 0, e = BitList->size(); i != e; ++i) {
      unsigned Bit = (*BitList)[i];
      if (NewVal->getBit(Bit)) {
        err() << "Cannot set bit #" << Bit << " of value '" << ValName
              << "' more than once!\n";
        exit(1);
      }
      NewVal->setBit(Bit, BInit->getBit(i));
    }

    for (unsigned i = 0, e = CurVal->getNumBits(); i != e; ++i)
      if (NewVal->getBit(i) == 0)
        NewVal->setBit(i, CurVal->getBit(i));

    V = NewVal;
  }

  if (RV->setValue(V)) {
    err() << "Value '" << ValName << "' of type '" << *RV->getType()
	  << "' is incompatible with initializer '" << *V << "'!\n";
    exit(1);
  }
}

// addSubClass - Add SC as a subclass to CurRec, resolving TemplateArgs as SC's
// template arguments.
static void addSubClass(Record *SC, const std::vector<Init*> &TemplateArgs) {
  // Add all of the values in the subclass into the current class...
  const std::vector<RecordVal> &Vals = SC->getValues();
  for (unsigned i = 0, e = Vals.size(); i != e; ++i)
    addValue(Vals[i]);

  const std::vector<std::string> &TArgs = SC->getTemplateArgs();

  // Ensure that an appropriate number of template arguments are specified...
  if (TArgs.size() < TemplateArgs.size()) {
    err() << "ERROR: More template args specified than expected!\n";
    exit(1);
  }
  
  // Loop over all of the template arguments, setting them to the specified
  // value or leaving them as the default if necessary.
  for (unsigned i = 0, e = TArgs.size(); i != e; ++i) {
    if (i < TemplateArgs.size()) {  // A value is specified for this temp-arg?
      // Set it now.
      setValue(TArgs[i], 0, TemplateArgs[i]);

      // Resolve it next.
      CurRec->resolveReferencesTo(CurRec->getValue(TArgs[i]));
                                  
      
      // Now remove it.
      CurRec->removeValue(TArgs[i]);

    } else if (!CurRec->getValue(TArgs[i])->getValue()->isComplete()) {
      err() << "ERROR: Value not specified for template argument #"
            << i << " (" << TArgs[i] << ") of subclass '" << SC->getName()
            << "'!\n";
      exit(1);
    }
  }

  // Since everything went well, we can now set the "superclass" list for the
  // current record.
  const std::vector<Record*> &SCs = SC->getSuperClasses();
  for (unsigned i = 0, e = SCs.size(); i != e; ++i)
    addSuperClass(SCs[i]);
  addSuperClass(SC);
}

} // End llvm namespace

using namespace llvm;



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 210 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
{
  std::string*                StrVal;
  int                         IntVal;
  llvm::RecTy*                Ty;
  llvm::Init*                 Initializer;
  std::vector<llvm::Init*>*   FieldList;
  std::vector<unsigned>*      BitList;
  llvm::Record*               Rec;
  std::vector<llvm::Record*>* RecList;
  SubClassRefTy*              SubClassRef;
  std::vector<SubClassRefTy>* SubClassList;
  std::vector<std::pair<llvm::Init*, std::string> >* DagValueList;
}
/* Line 187 of yacc.c.  */
#line 366 "FileParser.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 379 "FileParser.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  27
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   207

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  42
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  50
/* YYNRULES -- Number of rules.  */
#define YYNRULES  103
/* YYNRULES -- Number of states.  */
#define YYNSTATES  194

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   281

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      36,    37,     2,     2,    38,    40,    35,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    39,    41,
      27,    29,    28,    30,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    33,     2,    34,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    31,     2,    32,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    14,    16,    21,    23,
      25,    27,    28,    30,    31,    34,    36,    38,    40,    42,
      44,    46,    50,    55,    60,    64,    68,    73,    78,    85,
      92,    99,   106,   113,   114,   117,   120,   125,   126,   128,
     130,   134,   137,   141,   147,   152,   154,   155,   159,   160,
     162,   164,   168,   173,   176,   183,   184,   187,   189,   193,
     195,   200,   202,   206,   207,   210,   212,   216,   220,   221,
     223,   225,   226,   228,   230,   232,   233,   237,   238,   239,
     246,   250,   252,   254,   257,   259,   260,   261,   270,   271,
     278,   280,   282,   284,   286,   291,   293,   297,   298,   303,
     308,   311,   313,   316
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      91,     0,    -1,    23,    -1,     5,    -1,     4,    -1,     6,
      27,    22,    28,    -1,     3,    -1,     7,    27,    44,    28,
      -1,     8,    -1,     9,    -1,    43,    -1,    -1,    14,    -1,
      -1,    29,    48,    -1,    23,    -1,    47,    -1,    22,    -1,
      25,    -1,    26,    -1,    30,    -1,    31,    55,    32,    -1,
      23,    27,    56,    28,    -1,    48,    31,    53,    32,    -1,
      33,    55,    34,    -1,    48,    35,    23,    -1,    36,    47,
      51,    37,    -1,    48,    33,    53,    34,    -1,    17,    36,
      48,    38,    48,    37,    -1,    18,    36,    48,    38,    48,
      37,    -1,    19,    36,    48,    38,    48,    37,    -1,    20,
      36,    48,    38,    48,    37,    -1,    21,    36,    48,    38,
      48,    37,    -1,    -1,    39,    24,    -1,    48,    49,    -1,
      50,    38,    48,    49,    -1,    -1,    50,    -1,    22,    -1,
      22,    40,    22,    -1,    22,    22,    -1,    52,    38,    22,
      -1,    52,    38,    22,    40,    22,    -1,    52,    38,    22,
      22,    -1,    52,    -1,    -1,    31,    53,    32,    -1,    -1,
      56,    -1,    48,    -1,    56,    38,    48,    -1,    45,    44,
      23,    46,    -1,    57,    41,    -1,    15,    23,    54,    29,
      48,    41,    -1,    -1,    59,    58,    -1,    41,    -1,    31,
      59,    32,    -1,    43,    -1,    43,    27,    56,    28,    -1,
      61,    -1,    62,    38,    61,    -1,    -1,    39,    62,    -1,
      57,    -1,    64,    38,    57,    -1,    27,    64,    28,    -1,
      -1,    65,    -1,    23,    -1,    -1,    67,    -1,    68,    -1,
      68,    -1,    -1,    63,    72,    60,    -1,    -1,    -1,    10,
      69,    74,    66,    75,    71,    -1,    11,    70,    71,    -1,
      76,    -1,    77,    -1,    78,    77,    -1,    23,    -1,    -1,
      -1,    12,    79,    81,    66,    82,    31,    78,    32,    -1,
      -1,    13,    23,    84,    39,    61,    41,    -1,    73,    -1,
      76,    -1,    80,    -1,    83,    -1,    23,    54,    29,    48,
      -1,    86,    -1,    87,    38,    86,    -1,    -1,    15,    89,
      87,    16,    -1,    88,    31,    90,    32,    -1,    88,    85,
      -1,    85,    -1,    90,    85,    -1,    90,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   246,   246,   268,   270,   272,   274,   276,   278,   280,
     282,   286,   286,   288,   288,   290,   313,   315,   317,   320,
     323,   325,   338,   366,   373,   376,   383,   386,   394,   396,
     398,   400,   402,   406,   409,   413,   418,   424,   427,   430,
     433,   446,   460,   462,   475,   491,   493,   493,   497,   499,
     503,   506,   510,   527,   529,   535,   535,   536,   536,   538,
     540,   544,   549,   554,   557,   561,   564,   569,   570,   570,
     572,   572,   574,   581,   599,   624,   624,   643,   645,   643,
     651,   661,   675,   678,   682,   693,   695,   693,   702,   702,
     776,   776,   777,   777,   779,   784,   784,   787,   787,   790,
     793,   797,   797,   799
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INT", "BIT", "STRING", "BITS", "LIST",
  "CODE", "DAG", "CLASS", "DEF", "MULTICLASS", "DEFM", "FIELD", "LET",
  "IN", "CONCATTOK", "SHLTOK", "SRATOK", "SRLTOK", "STRCONCATTOK",
  "INTVAL", "ID", "VARNAME", "STRVAL", "CODEFRAGMENT", "'<'", "'>'", "'='",
  "'?'", "'{'", "'}'", "'['", "']'", "'.'", "'('", "')'", "','", "':'",
  "'-'", "';'", "$accept", "ClassID", "Type", "OptPrefix", "OptValue",
  "IDValue", "Value", "OptVarName", "DagArgListNE", "DagArgList",
  "RBitList", "BitList", "OptBitList", "ValueList", "ValueListNE",
  "Declaration", "BodyItem", "BodyList", "Body", "SubClassRef",
  "ClassListNE", "ClassList", "DeclListNE", "TemplateArgList",
  "OptTemplateArgList", "OptID", "ObjectName", "ClassName", "DefName",
  "ObjectBody", "@1", "ClassInst", "@2", "@3", "DefInst", "MultiClassDef",
  "MultiClassBody", "MultiClassName", "MultiClassInst", "@4", "@5",
  "DefMInst", "@6", "Object", "LETItem", "LETList", "LETCommand", "@7",
  "ObjectList", "File", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,    60,    62,    61,
      63,   123,   125,    91,    93,    46,    40,    41,    44,    58,
      45,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    42,    43,    44,    44,    44,    44,    44,    44,    44,
      44,    45,    45,    46,    46,    47,    48,    48,    48,    48,
      48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
      48,    48,    48,    49,    49,    50,    50,    51,    51,    52,
      52,    52,    52,    52,    52,    53,    54,    54,    55,    55,
      56,    56,    57,    58,    58,    59,    59,    60,    60,    61,
      61,    62,    62,    63,    63,    64,    64,    65,    66,    66,
      67,    67,    68,    69,    70,    72,    71,    74,    75,    73,
      76,    77,    78,    78,    79,    81,    82,    80,    84,    83,
      85,    85,    85,    85,    86,    87,    87,    89,    88,    85,
      85,    90,    90,    91
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     4,     1,     4,     1,     1,
       1,     0,     1,     0,     2,     1,     1,     1,     1,     1,
       1,     3,     4,     4,     3,     3,     4,     4,     6,     6,
       6,     6,     6,     0,     2,     2,     4,     0,     1,     1,
       3,     2,     3,     5,     4,     1,     0,     3,     0,     1,
       1,     3,     4,     2,     6,     0,     2,     1,     3,     1,
       4,     1,     3,     0,     2,     1,     3,     3,     0,     1,
       1,     0,     1,     1,     1,     0,     3,     0,     0,     6,
       3,     1,     1,     2,     1,     0,     0,     8,     0,     6,
       1,     1,     1,     1,     4,     1,     3,     0,     4,     4,
       2,     1,     2,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    71,    71,     0,     0,    97,    90,    91,    92,    93,
     101,     0,   103,     0,    70,    72,    73,    77,    74,    63,
      84,    85,    88,     0,     0,   100,   102,     1,    68,     0,
      75,    80,    68,     0,    46,    95,     0,     0,    11,    69,
      78,     2,    59,    61,    64,     0,    86,     0,     0,     0,
      98,     0,    99,    12,     0,    65,     0,    63,     0,     0,
      55,    57,    76,     0,     0,    39,    45,     0,     0,    96,
       6,     4,     3,     0,     0,     8,     9,    10,     0,    67,
      11,    79,     0,     0,     0,     0,     0,    17,    15,    18,
      19,    20,    48,    48,     0,    16,    50,     0,    62,    11,
       0,    89,    41,     0,     0,    47,    94,     0,     0,    13,
      66,     0,     0,     0,     0,     0,     0,     0,    49,     0,
      15,    37,     0,     0,     0,    60,     0,     0,    58,     0,
      56,    81,    82,     0,    40,    42,     0,     0,     0,    52,
       0,     0,     0,     0,     0,     0,    21,    24,    33,    38,
       0,     0,     0,    25,    51,    46,    53,    87,    83,    44,
       0,     5,     7,    14,     0,     0,     0,     0,     0,    22,
       0,    35,     0,    26,    23,    27,     0,    43,     0,     0,
       0,     0,     0,    34,    33,     0,    28,    29,    30,    31,
      32,    36,     0,    54
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    42,    78,    54,   139,    95,    96,   171,   149,   150,
      66,    67,    49,   117,   118,    55,   130,    99,    62,    43,
      44,    30,    56,    39,    40,    15,    16,    17,    19,    31,
      45,     6,    28,    57,     7,   132,   133,    21,     8,    32,
      63,     9,    33,    10,    35,    36,    11,    23,    12,    13
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -98
static const yytype_int16 yypact[] =
{
     147,    32,    32,    48,    49,   -98,   -98,   -98,   -98,   -98,
     -98,     2,   147,    25,   -98,   -98,   -98,   -98,   -98,    36,
     -98,   -98,   -98,    60,   147,   -98,   -98,   -98,    57,    63,
     -98,   -98,    57,    56,    71,   -98,    -6,    -4,    92,   -98,
     -98,   -98,    83,   -98,    74,    10,   -98,    63,    93,    84,
     -98,    60,   -98,   -98,    15,   -98,    12,    36,    43,    63,
     -98,   -98,   -98,    88,    80,     9,    87,    95,    43,   -98,
     -98,   -98,   -98,   104,   126,   -98,   -98,   -98,   110,   -98,
      92,   -98,   125,   127,   128,   129,   130,   -98,   140,   -98,
     -98,   -98,    43,    43,   132,   -98,    59,    39,   -98,    20,
     157,   -98,   -98,   148,   149,   -98,    59,   150,    15,   144,
     -98,    43,    43,    43,    43,    43,    43,   137,   136,   141,
     -98,    43,    93,    93,   153,   -98,    43,   154,   -98,   138,
     -98,   -98,   -98,     5,   -98,    17,   152,   155,    43,   -98,
      70,    76,    85,    91,    97,    53,   -98,   -98,    54,   143,
     145,   146,   151,   -98,    59,    71,   -98,   -98,   -98,   -98,
     162,   -98,   -98,    59,    43,    43,    43,    43,    43,   -98,
     163,   -98,    43,   -98,   -98,   -98,   159,   -98,   103,   106,
     111,   114,   119,   -98,    54,    43,   -98,   -98,   -98,   -98,
     -98,   -98,    47,   -98
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -98,   -52,    78,   -98,   -98,    96,   -68,     7,   -98,   -98,
     -98,   -96,    34,    99,   -57,   -51,   -98,   -98,   -98,    -5,
     -98,   -98,   -98,   -98,   161,   -98,   192,   -98,   -98,   139,
     -98,   -98,   -98,   -98,   -97,    62,   -98,   -98,   -98,   -98,
     -98,   -98,   -98,    -7,   156,   -98,   -98,   -98,   173,   -98
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
     106,    97,    77,   131,    25,    26,     1,     2,     3,     4,
      50,     5,     1,     2,     3,     4,     2,     5,    70,    71,
      72,    73,    74,    75,    76,    27,   151,   152,    52,   110,
      26,   102,    51,    24,    53,   127,   131,   157,    41,   159,
      79,    60,    64,   140,   141,   142,   143,   144,   129,   103,
      80,    61,   128,   148,    98,    14,    77,   160,   154,   145,
      82,    83,    84,    85,    86,    87,    88,   125,    89,    90,
     163,    20,    22,    91,    92,    29,    93,   126,   122,    94,
     123,   169,   124,    34,    38,   122,    41,   123,   193,   124,
     122,   126,   123,   170,   124,    47,   178,   179,   180,   181,
     182,   122,    48,   123,   184,   124,    53,   122,   164,   123,
      58,   124,    59,    68,   165,    65,   122,   192,   123,   100,
     124,   101,   122,   166,   123,   104,   124,   105,   122,   167,
     123,   107,   124,   109,   122,   168,   123,   122,   124,   123,
     186,   124,   122,   187,   123,   122,   124,   123,   188,   124,
     122,   189,   123,   108,   124,   120,   190,     1,     2,     3,
       4,   111,     5,   112,   113,   114,   115,   116,     2,   146,
     134,   135,   136,   138,   126,   147,   153,   155,   174,   156,
     161,   172,   173,   162,   177,   175,   137,   183,   185,   176,
     121,   191,   119,    46,    18,   158,    81,    37,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    69
};

static const yytype_int16 yycheck[] =
{
      68,    58,    54,   100,    11,    12,    10,    11,    12,    13,
      16,    15,    10,    11,    12,    13,    11,    15,     3,     4,
       5,     6,     7,     8,     9,     0,   122,   123,    32,    80,
      37,    22,    38,    31,    14,    15,   133,    32,    23,    22,
      28,    31,    47,   111,   112,   113,   114,   115,    99,    40,
      38,    41,    32,   121,    59,    23,   108,    40,   126,   116,
      17,    18,    19,    20,    21,    22,    23,    28,    25,    26,
     138,    23,    23,    30,    31,    39,    33,    38,    31,    36,
      33,    28,    35,    23,    27,    31,    23,    33,    41,    35,
      31,    38,    33,    39,    35,    39,   164,   165,   166,   167,
     168,    31,    31,    33,   172,    35,    14,    31,    38,    33,
      27,    35,    38,    29,    38,    22,    31,   185,    33,    31,
      35,    41,    31,    38,    33,    38,    35,    32,    31,    38,
      33,    27,    35,    23,    31,    38,    33,    31,    35,    33,
      37,    35,    31,    37,    33,    31,    35,    33,    37,    35,
      31,    37,    33,    27,    35,    23,    37,    10,    11,    12,
      13,    36,    15,    36,    36,    36,    36,    27,    11,    32,
      22,    22,    22,    29,    38,    34,    23,    23,    32,    41,
      28,    38,    37,    28,    22,    34,   108,    24,    29,   155,
      94,   184,    93,    32,     2,   133,    57,    24,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    51
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    10,    11,    12,    13,    15,    73,    76,    80,    83,
      85,    88,    90,    91,    23,    67,    68,    69,    68,    70,
      23,    79,    23,    89,    31,    85,    85,     0,    74,    39,
      63,    71,    81,    84,    23,    86,    87,    90,    27,    65,
      66,    23,    43,    61,    62,    72,    66,    39,    31,    54,
      16,    38,    32,    14,    45,    57,    64,    75,    27,    38,
      31,    41,    60,    82,    61,    22,    52,    53,    29,    86,
       3,     4,     5,     6,     7,     8,     9,    43,    44,    28,
      38,    71,    17,    18,    19,    20,    21,    22,    23,    25,
      26,    30,    31,    33,    36,    47,    48,    56,    61,    59,
      31,    41,    22,    40,    38,    32,    48,    27,    27,    23,
      57,    36,    36,    36,    36,    36,    27,    55,    56,    55,
      23,    47,    31,    33,    35,    28,    38,    15,    32,    57,
      58,    76,    77,    78,    22,    22,    22,    44,    29,    46,
      48,    48,    48,    48,    48,    56,    32,    34,    48,    50,
      51,    53,    53,    23,    48,    23,    41,    32,    77,    22,
      40,    28,    28,    48,    38,    38,    38,    38,    38,    28,
      39,    49,    38,    37,    32,    34,    54,    22,    48,    48,
      48,    48,    48,    24,    48,    29,    37,    37,    37,    37,
      37,    49,    48,    41
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 246 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    if (CurDefmPrefix) {
      // If CurDefmPrefix is set, we're parsing a defm, which means that this is
      // actually the name of a multiclass.
      MultiClass *MC = MultiClasses[*(yyvsp[(1) - (1)].StrVal)];
      if (MC == 0) {
        err() << "Couldn't find class '" << *(yyvsp[(1) - (1)].StrVal) << "'!\n";
        exit(1);
      }
      (yyval.Rec) = &MC->Rec;
    } else {
      (yyval.Rec) = Records.getClass(*(yyvsp[(1) - (1)].StrVal));
    }
    if ((yyval.Rec) == 0) {
      err() << "Couldn't find class '" << *(yyvsp[(1) - (1)].StrVal) << "'!\n";
      exit(1);
    }
    delete (yyvsp[(1) - (1)].StrVal);
  ;}
    break;

  case 3:
#line 268 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {                       // string type
    (yyval.Ty) = new StringRecTy();
  ;}
    break;

  case 4:
#line 270 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {                           // bit type
    (yyval.Ty) = new BitRecTy();
  ;}
    break;

  case 5:
#line 272 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {           // bits<x> type
    (yyval.Ty) = new BitsRecTy((yyvsp[(3) - (4)].IntVal));
  ;}
    break;

  case 6:
#line 274 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {                           // int type
    (yyval.Ty) = new IntRecTy();
  ;}
    break;

  case 7:
#line 276 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {          // list<x> type
    (yyval.Ty) = new ListRecTy((yyvsp[(3) - (4)].Ty));
  ;}
    break;

  case 8:
#line 278 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {                          // code type
    (yyval.Ty) = new CodeRecTy();
  ;}
    break;

  case 9:
#line 280 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {                           // dag type
    (yyval.Ty) = new DagRecTy();
  ;}
    break;

  case 10:
#line 282 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {                       // Record Type
    (yyval.Ty) = new RecordRecTy((yyvsp[(1) - (1)].Rec));
  ;}
    break;

  case 11:
#line 286 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    { (yyval.IntVal) = 0; ;}
    break;

  case 12:
#line 286 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    { (yyval.IntVal) = 1; ;}
    break;

  case 13:
#line 288 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    { (yyval.Initializer) = 0; ;}
    break;

  case 14:
#line 288 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    { (yyval.Initializer) = (yyvsp[(2) - (2)].Initializer); ;}
    break;

  case 15:
#line 290 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  if (const RecordVal *RV = (CurRec ? CurRec->getValue(*(yyvsp[(1) - (1)].StrVal)) : 0)) {
    (yyval.Initializer) = new VarInit(*(yyvsp[(1) - (1)].StrVal), RV->getType());
  } else if (CurRec && CurRec->isTemplateArg(CurRec->getName()+":"+*(yyvsp[(1) - (1)].StrVal))) {
    const RecordVal *RV = CurRec->getValue(CurRec->getName()+":"+*(yyvsp[(1) - (1)].StrVal));
    assert(RV && "Template arg doesn't exist??");
    (yyval.Initializer) = new VarInit(CurRec->getName()+":"+*(yyvsp[(1) - (1)].StrVal), RV->getType());
  } else if (CurMultiClass &&
      CurMultiClass->Rec.isTemplateArg(CurMultiClass->Rec.getName()+"::"+*(yyvsp[(1) - (1)].StrVal))) {
    std::string Name = CurMultiClass->Rec.getName()+"::"+*(yyvsp[(1) - (1)].StrVal);
    const RecordVal *RV = CurMultiClass->Rec.getValue(Name);
    assert(RV && "Template arg doesn't exist??");
    (yyval.Initializer) = new VarInit(Name, RV->getType());
  } else if (Record *D = Records.getDef(*(yyvsp[(1) - (1)].StrVal))) {
    (yyval.Initializer) = new DefInit(D);
  } else {
    err() << "Variable not defined: '" << *(yyvsp[(1) - (1)].StrVal) << "'!\n";
    exit(1);
  }
  
  delete (yyvsp[(1) - (1)].StrVal);
;}
    break;

  case 16:
#line 313 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.Initializer) = (yyvsp[(1) - (1)].Initializer);
  ;}
    break;

  case 17:
#line 315 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.Initializer) = new IntInit((yyvsp[(1) - (1)].IntVal));
  ;}
    break;

  case 18:
#line 317 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.Initializer) = new StringInit(*(yyvsp[(1) - (1)].StrVal));
    delete (yyvsp[(1) - (1)].StrVal);
  ;}
    break;

  case 19:
#line 320 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.Initializer) = new CodeInit(*(yyvsp[(1) - (1)].StrVal));
    delete (yyvsp[(1) - (1)].StrVal);
  ;}
    break;

  case 20:
#line 323 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.Initializer) = new UnsetInit();
  ;}
    break;

  case 21:
#line 325 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    BitsInit *Init = new BitsInit((yyvsp[(2) - (3)].FieldList)->size());
    for (unsigned i = 0, e = (yyvsp[(2) - (3)].FieldList)->size(); i != e; ++i) {
      struct Init *Bit = (*(yyvsp[(2) - (3)].FieldList))[i]->convertInitializerTo(new BitRecTy());
      if (Bit == 0) {
        err() << "Element #" << i << " (" << *(*(yyvsp[(2) - (3)].FieldList))[i]
       	      << ") is not convertable to a bit!\n";
        exit(1);
      }
      Init->setBit((yyvsp[(2) - (3)].FieldList)->size()-i-1, Bit);
    }
    (yyval.Initializer) = Init;
    delete (yyvsp[(2) - (3)].FieldList);
  ;}
    break;

  case 22:
#line 338 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    // This is a CLASS<initvalslist> expression.  This is supposed to synthesize
    // a new anonymous definition, deriving from CLASS<initvalslist> with no
    // body.
    Record *Class = Records.getClass(*(yyvsp[(1) - (4)].StrVal));
    if (!Class) {
      err() << "Expected a class, got '" << *(yyvsp[(1) - (4)].StrVal) << "'!\n";
      exit(1);
    }
    delete (yyvsp[(1) - (4)].StrVal);
    
    static unsigned AnonCounter = 0;
    Record *OldRec = CurRec;  // Save CurRec.
    
    // Create the new record, set it as CurRec temporarily.
    CurRec = new Record("anonymous.val."+utostr(AnonCounter++));
    addSubClass(Class, *(yyvsp[(3) - (4)].FieldList));    // Add info about the subclass to CurRec.
    delete (yyvsp[(3) - (4)].FieldList);  // Free up the template args.
    
    CurRec->resolveReferences();
    
    Records.addDef(CurRec);
    
    // The result of the expression is a reference to the new record.
    (yyval.Initializer) = new DefInit(CurRec);
    
    // Restore the old CurRec
    CurRec = OldRec;
  ;}
    break;

  case 23:
#line 366 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.Initializer) = (yyvsp[(1) - (4)].Initializer)->convertInitializerBitRange(*(yyvsp[(3) - (4)].BitList));
    if ((yyval.Initializer) == 0) {
      err() << "Invalid bit range for value '" << *(yyvsp[(1) - (4)].Initializer) << "'!\n";
      exit(1);
    }
    delete (yyvsp[(3) - (4)].BitList);
  ;}
    break;

  case 24:
#line 373 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.Initializer) = new ListInit(*(yyvsp[(2) - (3)].FieldList));
    delete (yyvsp[(2) - (3)].FieldList);
  ;}
    break;

  case 25:
#line 376 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    if (!(yyvsp[(1) - (3)].Initializer)->getFieldType(*(yyvsp[(3) - (3)].StrVal))) {
      err() << "Cannot access field '" << *(yyvsp[(3) - (3)].StrVal) << "' of value '" << *(yyvsp[(1) - (3)].Initializer) << "!\n";
      exit(1);
    }
    (yyval.Initializer) = new FieldInit((yyvsp[(1) - (3)].Initializer), *(yyvsp[(3) - (3)].StrVal));
    delete (yyvsp[(3) - (3)].StrVal);
  ;}
    break;

  case 26:
#line 383 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.Initializer) = new DagInit((yyvsp[(2) - (4)].Initializer), *(yyvsp[(3) - (4)].DagValueList));
    delete (yyvsp[(3) - (4)].DagValueList);
  ;}
    break;

  case 27:
#line 386 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    std::reverse((yyvsp[(3) - (4)].BitList)->begin(), (yyvsp[(3) - (4)].BitList)->end());
    (yyval.Initializer) = (yyvsp[(1) - (4)].Initializer)->convertInitListSlice(*(yyvsp[(3) - (4)].BitList));
    if ((yyval.Initializer) == 0) {
      err() << "Invalid list slice for value '" << *(yyvsp[(1) - (4)].Initializer) << "'!\n";
      exit(1);
    }
    delete (yyvsp[(3) - (4)].BitList);
  ;}
    break;

  case 28:
#line 394 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.Initializer) = (new BinOpInit(BinOpInit::CONCAT, (yyvsp[(3) - (6)].Initializer), (yyvsp[(5) - (6)].Initializer)))->Fold();
  ;}
    break;

  case 29:
#line 396 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.Initializer) = (new BinOpInit(BinOpInit::SHL, (yyvsp[(3) - (6)].Initializer), (yyvsp[(5) - (6)].Initializer)))->Fold();
  ;}
    break;

  case 30:
#line 398 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.Initializer) = (new BinOpInit(BinOpInit::SRA, (yyvsp[(3) - (6)].Initializer), (yyvsp[(5) - (6)].Initializer)))->Fold();
  ;}
    break;

  case 31:
#line 400 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.Initializer) = (new BinOpInit(BinOpInit::SRL, (yyvsp[(3) - (6)].Initializer), (yyvsp[(5) - (6)].Initializer)))->Fold();
  ;}
    break;

  case 32:
#line 402 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.Initializer) = (new BinOpInit(BinOpInit::STRCONCAT, (yyvsp[(3) - (6)].Initializer), (yyvsp[(5) - (6)].Initializer)))->Fold();
  ;}
    break;

  case 33:
#line 406 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.StrVal) = new std::string();
  ;}
    break;

  case 34:
#line 409 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.StrVal) = (yyvsp[(2) - (2)].StrVal);
  ;}
    break;

  case 35:
#line 413 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.DagValueList) = new std::vector<std::pair<Init*, std::string> >();
    (yyval.DagValueList)->push_back(std::make_pair((yyvsp[(1) - (2)].Initializer), *(yyvsp[(2) - (2)].StrVal)));
    delete (yyvsp[(2) - (2)].StrVal);
  ;}
    break;

  case 36:
#line 418 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyvsp[(1) - (4)].DagValueList)->push_back(std::make_pair((yyvsp[(3) - (4)].Initializer), *(yyvsp[(4) - (4)].StrVal)));
    delete (yyvsp[(4) - (4)].StrVal);
    (yyval.DagValueList) = (yyvsp[(1) - (4)].DagValueList);
  ;}
    break;

  case 37:
#line 424 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.DagValueList) = new std::vector<std::pair<Init*, std::string> >();
  ;}
    break;

  case 38:
#line 427 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    { (yyval.DagValueList) = (yyvsp[(1) - (1)].DagValueList); ;}
    break;

  case 39:
#line 430 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.BitList) = new std::vector<unsigned>();
    (yyval.BitList)->push_back((yyvsp[(1) - (1)].IntVal));
  ;}
    break;

  case 40:
#line 433 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    if ((yyvsp[(1) - (3)].IntVal) < 0 || (yyvsp[(3) - (3)].IntVal) < 0) {
      err() << "Invalid range: " << (yyvsp[(1) - (3)].IntVal) << "-" << (yyvsp[(3) - (3)].IntVal) << "!\n";
      exit(1);
    }
    (yyval.BitList) = new std::vector<unsigned>();
    if ((yyvsp[(1) - (3)].IntVal) < (yyvsp[(3) - (3)].IntVal)) {
      for (int i = (yyvsp[(1) - (3)].IntVal); i <= (yyvsp[(3) - (3)].IntVal); ++i)
        (yyval.BitList)->push_back(i);
    } else {
      for (int i = (yyvsp[(1) - (3)].IntVal); i >= (yyvsp[(3) - (3)].IntVal); --i)
        (yyval.BitList)->push_back(i);
    }
  ;}
    break;

  case 41:
#line 446 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyvsp[(2) - (2)].IntVal) = -(yyvsp[(2) - (2)].IntVal);
    if ((yyvsp[(1) - (2)].IntVal) < 0 || (yyvsp[(2) - (2)].IntVal) < 0) {
      err() << "Invalid range: " << (yyvsp[(1) - (2)].IntVal) << "-" << (yyvsp[(2) - (2)].IntVal) << "!\n";
      exit(1);
    }
    (yyval.BitList) = new std::vector<unsigned>();
    if ((yyvsp[(1) - (2)].IntVal) < (yyvsp[(2) - (2)].IntVal)) {
      for (int i = (yyvsp[(1) - (2)].IntVal); i <= (yyvsp[(2) - (2)].IntVal); ++i)
        (yyval.BitList)->push_back(i);
    } else {
      for (int i = (yyvsp[(1) - (2)].IntVal); i >= (yyvsp[(2) - (2)].IntVal); --i)
        (yyval.BitList)->push_back(i);
    }
  ;}
    break;

  case 42:
#line 460 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    ((yyval.BitList)=(yyvsp[(1) - (3)].BitList))->push_back((yyvsp[(3) - (3)].IntVal));
  ;}
    break;

  case 43:
#line 462 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    if ((yyvsp[(3) - (5)].IntVal) < 0 || (yyvsp[(5) - (5)].IntVal) < 0) {
      err() << "Invalid range: " << (yyvsp[(3) - (5)].IntVal) << "-" << (yyvsp[(5) - (5)].IntVal) << "!\n";
      exit(1);
    }
    (yyval.BitList) = (yyvsp[(1) - (5)].BitList);
    if ((yyvsp[(3) - (5)].IntVal) < (yyvsp[(5) - (5)].IntVal)) {
      for (int i = (yyvsp[(3) - (5)].IntVal); i <= (yyvsp[(5) - (5)].IntVal); ++i)
        (yyval.BitList)->push_back(i);
    } else {
      for (int i = (yyvsp[(3) - (5)].IntVal); i >= (yyvsp[(5) - (5)].IntVal); --i)
        (yyval.BitList)->push_back(i);
    }
  ;}
    break;

  case 44:
#line 475 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyvsp[(4) - (4)].IntVal) = -(yyvsp[(4) - (4)].IntVal);
    if ((yyvsp[(3) - (4)].IntVal) < 0 || (yyvsp[(4) - (4)].IntVal) < 0) {
      err() << "Invalid range: " << (yyvsp[(3) - (4)].IntVal) << "-" << (yyvsp[(4) - (4)].IntVal) << "!\n";
      exit(1);
    }
    (yyval.BitList) = (yyvsp[(1) - (4)].BitList);
    if ((yyvsp[(3) - (4)].IntVal) < (yyvsp[(4) - (4)].IntVal)) {
      for (int i = (yyvsp[(3) - (4)].IntVal); i <= (yyvsp[(4) - (4)].IntVal); ++i)
        (yyval.BitList)->push_back(i);
    } else {
      for (int i = (yyvsp[(3) - (4)].IntVal); i >= (yyvsp[(4) - (4)].IntVal); --i)
        (yyval.BitList)->push_back(i);
    }
  ;}
    break;

  case 45:
#line 491 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    { (yyval.BitList) = (yyvsp[(1) - (1)].BitList); std::reverse((yyvsp[(1) - (1)].BitList)->begin(), (yyvsp[(1) - (1)].BitList)->end()); ;}
    break;

  case 46:
#line 493 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    { (yyval.BitList) = 0; ;}
    break;

  case 47:
#line 493 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    { (yyval.BitList) = (yyvsp[(2) - (3)].BitList); ;}
    break;

  case 48:
#line 497 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.FieldList) = new std::vector<Init*>();
  ;}
    break;

  case 49:
#line 499 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.FieldList) = (yyvsp[(1) - (1)].FieldList);
  ;}
    break;

  case 50:
#line 503 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.FieldList) = new std::vector<Init*>();
    (yyval.FieldList)->push_back((yyvsp[(1) - (1)].Initializer));
  ;}
    break;

  case 51:
#line 506 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    ((yyval.FieldList) = (yyvsp[(1) - (3)].FieldList))->push_back((yyvsp[(3) - (3)].Initializer));
  ;}
    break;

  case 52:
#line 510 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  std::string DecName = *(yyvsp[(3) - (4)].StrVal);
  if (ParsingTemplateArgs) {
    if (CurRec) {
      DecName = CurRec->getName() + ":" + DecName;
    } else {
      assert(CurMultiClass);
    }
    if (CurMultiClass)
      DecName = CurMultiClass->Rec.getName() + "::" + DecName;
  }

  addValue(RecordVal(DecName, (yyvsp[(2) - (4)].Ty), (yyvsp[(1) - (4)].IntVal)));
  setValue(DecName, 0, (yyvsp[(4) - (4)].Initializer));
  (yyval.StrVal) = new std::string(DecName);
;}
    break;

  case 53:
#line 527 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  delete (yyvsp[(1) - (2)].StrVal);
;}
    break;

  case 54:
#line 529 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  setValue(*(yyvsp[(2) - (6)].StrVal), (yyvsp[(3) - (6)].BitList), (yyvsp[(5) - (6)].Initializer));
  delete (yyvsp[(2) - (6)].StrVal);
  delete (yyvsp[(3) - (6)].BitList);
;}
    break;

  case 59:
#line 538 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.SubClassRef) = new SubClassRefTy((yyvsp[(1) - (1)].Rec), new std::vector<Init*>());
  ;}
    break;

  case 60:
#line 540 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.SubClassRef) = new SubClassRefTy((yyvsp[(1) - (4)].Rec), (yyvsp[(3) - (4)].FieldList));
  ;}
    break;

  case 61:
#line 544 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.SubClassList) = new std::vector<SubClassRefTy>();
    (yyval.SubClassList)->push_back(*(yyvsp[(1) - (1)].SubClassRef));
    delete (yyvsp[(1) - (1)].SubClassRef);
  ;}
    break;

  case 62:
#line 549 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    ((yyval.SubClassList)=(yyvsp[(1) - (3)].SubClassList))->push_back(*(yyvsp[(3) - (3)].SubClassRef));
    delete (yyvsp[(3) - (3)].SubClassRef);
  ;}
    break;

  case 63:
#line 554 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.SubClassList) = new std::vector<SubClassRefTy>();
  ;}
    break;

  case 64:
#line 557 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    (yyval.SubClassList) = (yyvsp[(2) - (2)].SubClassList);
  ;}
    break;

  case 65:
#line 561 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  getActiveRec()->addTemplateArg(*(yyvsp[(1) - (1)].StrVal));
  delete (yyvsp[(1) - (1)].StrVal);
;}
    break;

  case 66:
#line 564 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  getActiveRec()->addTemplateArg(*(yyvsp[(3) - (3)].StrVal));
  delete (yyvsp[(3) - (3)].StrVal);
;}
    break;

  case 67:
#line 569 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {;}
    break;

  case 70:
#line 572 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    { (yyval.StrVal) = (yyvsp[(1) - (1)].StrVal); ;}
    break;

  case 71:
#line 572 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    { (yyval.StrVal) = new std::string(); ;}
    break;

  case 72:
#line 574 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  static unsigned AnonCounter = 0;
  if ((yyvsp[(1) - (1)].StrVal)->empty())
    *(yyvsp[(1) - (1)].StrVal) = "anonymous."+utostr(AnonCounter++);
  (yyval.StrVal) = (yyvsp[(1) - (1)].StrVal);
;}
    break;

  case 73:
#line 581 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  // If a class of this name already exists, it must be a forward ref.
  if ((CurRec = Records.getClass(*(yyvsp[(1) - (1)].StrVal)))) {
    // If the body was previously defined, this is an error.
    if (!CurRec->getValues().empty() ||
        !CurRec->getSuperClasses().empty() ||
        !CurRec->getTemplateArgs().empty()) {
      err() << "Class '" << CurRec->getName() << "' already defined!\n";
      exit(1);
    }
  } else {
    // If this is the first reference to this class, create and add it.
    CurRec = new Record(*(yyvsp[(1) - (1)].StrVal));
    Records.addClass(CurRec);
  }
  delete (yyvsp[(1) - (1)].StrVal);
;}
    break;

  case 74:
#line 599 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  CurRec = new Record(*(yyvsp[(1) - (1)].StrVal));
  delete (yyvsp[(1) - (1)].StrVal);
  
  if (!CurMultiClass) {
    // Top-level def definition.
    
    // Ensure redefinition doesn't happen.
    if (Records.getDef(CurRec->getName())) {
      err() << "def '" << CurRec->getName() << "' already defined!\n";
      exit(1);
    }
    Records.addDef(CurRec);
  } else {
    // Otherwise, a def inside a multiclass, add it to the multiclass.
    for (unsigned i = 0, e = CurMultiClass->DefPrototypes.size(); i != e; ++i)
      if (CurMultiClass->DefPrototypes[i]->getName() == CurRec->getName()) {
        err() << "def '" << CurRec->getName()
              << "' already defined in this multiclass!\n";
        exit(1);
      }
    CurMultiClass->DefPrototypes.push_back(CurRec);
  }
;}
    break;

  case 75:
#line 624 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
           for (unsigned i = 0, e = (yyvsp[(1) - (1)].SubClassList)->size(); i != e; ++i) {
             addSubClass((*(yyvsp[(1) - (1)].SubClassList))[i].first, *(*(yyvsp[(1) - (1)].SubClassList))[i].second);
             // Delete the template arg values for the class
             delete (*(yyvsp[(1) - (1)].SubClassList))[i].second;
           }
           delete (yyvsp[(1) - (1)].SubClassList);   // Delete the class list.
  
           // Process any variables on the let stack.
           for (unsigned i = 0, e = LetStack.size(); i != e; ++i)
             for (unsigned j = 0, e = LetStack[i].size(); j != e; ++j)
               setValue(LetStack[i][j].Name,
                        LetStack[i][j].HasBits ? &LetStack[i][j].Bits : 0,
                        LetStack[i][j].Value);
         ;}
    break;

  case 76:
#line 638 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
           (yyval.Rec) = CurRec;
           CurRec = 0;
         ;}
    break;

  case 77:
#line 643 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
                ParsingTemplateArgs = true;
            ;}
    break;

  case 78:
#line 645 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
                ParsingTemplateArgs = false;
            ;}
    break;

  case 79:
#line 647 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
        (yyval.Rec) = (yyvsp[(6) - (6)].Rec);
     ;}
    break;

  case 80:
#line 651 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  if (CurMultiClass == 0)  // Def's in multiclasses aren't really defs.
    (yyvsp[(3) - (3)].Rec)->resolveReferences();

  // If ObjectBody has template arguments, it's an error.
  assert((yyvsp[(3) - (3)].Rec)->getTemplateArgs().empty() && "How'd this get template args?");
  (yyval.Rec) = (yyvsp[(3) - (3)].Rec);
;}
    break;

  case 81:
#line 661 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  (yyval.Rec) = (yyvsp[(1) - (1)].Rec);
  // Copy the template arguments for the multiclass into the def.
  const std::vector<std::string> &TArgs = CurMultiClass->Rec.getTemplateArgs();
  
  for (unsigned i = 0, e = TArgs.size(); i != e; ++i) {
    const RecordVal *RV = CurMultiClass->Rec.getValue(TArgs[i]);
    assert(RV && "Template arg doesn't exist?");
    (yyval.Rec)->addValue(*RV);
  }
;}
    break;

  case 82:
#line 675 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  (yyval.RecList) = new std::vector<Record*>();
  (yyval.RecList)->push_back((yyvsp[(1) - (1)].Rec));
;}
    break;

  case 83:
#line 678 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  (yyval.RecList)->push_back((yyvsp[(2) - (2)].Rec));  
;}
    break;

  case 84:
#line 682 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  MultiClass *&MCE = MultiClasses[*(yyvsp[(1) - (1)].StrVal)];
  if (MCE) {
    err() << "multiclass '" << *(yyvsp[(1) - (1)].StrVal) << "' already defined!\n";
    exit(1);
  }
  MCE = CurMultiClass = new MultiClass(*(yyvsp[(1) - (1)].StrVal));
  delete (yyvsp[(1) - (1)].StrVal);
;}
    break;

  case 85:
#line 693 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
                                             ParsingTemplateArgs = true;
                                           ;}
    break;

  case 86:
#line 695 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
                                             ParsingTemplateArgs = false;
                                           ;}
    break;

  case 87:
#line 697 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  CurMultiClass = 0;
;}
    break;

  case 88:
#line 702 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    { CurDefmPrefix = (yyvsp[(2) - (2)].StrVal); ;}
    break;

  case 89:
#line 702 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  // To instantiate a multiclass, we need to first get the multiclass, then
  // instantiate each def contained in the multiclass with the SubClassRef
  // template parameters.
  MultiClass *MC = MultiClasses[(yyvsp[(5) - (6)].SubClassRef)->first->getName()];
  assert(MC && "Didn't lookup multiclass correctly?");
  std::vector<Init*> &TemplateVals = *(yyvsp[(5) - (6)].SubClassRef)->second;
  delete (yyvsp[(5) - (6)].SubClassRef);
  
  // Verify that the correct number of template arguments were specified.
  const std::vector<std::string> &TArgs = MC->Rec.getTemplateArgs();
  if (TArgs.size() < TemplateVals.size()) {
    err() << "ERROR: More template args specified than multiclass expects!\n";
    exit(1);
  }
  
  // Loop over all the def's in the multiclass, instantiating each one.
  for (unsigned i = 0, e = MC->DefPrototypes.size(); i != e; ++i) {
    Record *DefProto = MC->DefPrototypes[i];
    
    // Add the suffix to the defm name to get the new name.
    assert(CurRec == 0 && "A def is current?");
    CurRec = new Record(*(yyvsp[(2) - (6)].StrVal) + DefProto->getName());
    
    addSubClass(DefProto, std::vector<Init*>());
    
    // Loop over all of the template arguments, setting them to the specified
    // value or leaving them as the default if necessary.
    for (unsigned i = 0, e = TArgs.size(); i != e; ++i) {
      if (i < TemplateVals.size()) { // A value is specified for this temp-arg?
        // Set it now.
        setValue(TArgs[i], 0, TemplateVals[i]);
        
        // Resolve it next.
        CurRec->resolveReferencesTo(CurRec->getValue(TArgs[i]));
        
        // Now remove it.
        CurRec->removeValue(TArgs[i]);
        
      } else if (!CurRec->getValue(TArgs[i])->getValue()->isComplete()) {
        err() << "ERROR: Value not specified for template argument #"
        << i << " (" << TArgs[i] << ") of multiclassclass '"
        << MC->Rec.getName() << "'!\n";
        exit(1);
      }
    }
    
    // If the mdef is inside a 'let' expression, add to each def.
    for (unsigned i = 0, e = LetStack.size(); i != e; ++i)
      for (unsigned j = 0, e = LetStack[i].size(); j != e; ++j)
        setValue(LetStack[i][j].Name,
                 LetStack[i][j].HasBits ? &LetStack[i][j].Bits : 0,
                 LetStack[i][j].Value);
    
    
    // Ensure redefinition doesn't happen.
    if (Records.getDef(CurRec->getName())) {
      err() << "def '" << CurRec->getName() << "' already defined, "
            << "instantiating defm '" << *(yyvsp[(2) - (6)].StrVal) << "' with subdef '"
            << DefProto->getName() << "'!\n";
      exit(1);
    }
    Records.addDef(CurRec);
    
    CurRec->resolveReferences();

    CurRec = 0;
  }
  
  delete &TemplateVals;
  delete (yyvsp[(2) - (6)].StrVal);
  CurDefmPrefix = 0;
;}
    break;

  case 90:
#line 776 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {;}
    break;

  case 91:
#line 776 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {;}
    break;

  case 94:
#line 779 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
  LetStack.back().push_back(LetRecord(*(yyvsp[(1) - (4)].StrVal), (yyvsp[(2) - (4)].BitList), (yyvsp[(4) - (4)].Initializer)));
  delete (yyvsp[(1) - (4)].StrVal); delete (yyvsp[(2) - (4)].BitList);
;}
    break;

  case 97:
#line 787 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    { LetStack.push_back(std::vector<LetRecord>()); ;}
    break;

  case 99:
#line 790 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    LetStack.pop_back();
  ;}
    break;

  case 100:
#line 793 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {
    LetStack.pop_back();
  ;}
    break;

  case 101:
#line 797 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {;}
    break;

  case 102:
#line 797 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
    {;}
    break;


/* Line 1267 of yacc.c.  */
#line 2656 "FileParser.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 801 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"


int yyerror(const char *ErrorMsg) {
  err() << "Error parsing: " << ErrorMsg << "\n";
  exit(1);
}

