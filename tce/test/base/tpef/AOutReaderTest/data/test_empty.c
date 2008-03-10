/**
 * @file test_empty.c
 *
 * A.out file without text section.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 */


/* 
   Wasn't needed, because gcc adds its own compiler information symbols 
   (actually N_TEXT symbols which is a bit annoying, because value can point to
    text section that doesn't necessarily exist).

   extern int justOneSymbol; 
*/
