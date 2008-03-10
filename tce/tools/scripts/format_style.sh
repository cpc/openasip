#!/bin/bash
#
# Uses 'astyle' to format the code to cleanup some of the
# issues not matching the style code. Does not fix everything.
#
# The filename is given as argument.
#
# requires astyle v.1.9 or above

target=$(mktemp)
backupdir=$(mktemp -d)

# -a = Attach brackets (if (something) { <-)
# -p = Insert space paddings around operators only: a+b -> a + b
# 
# -U = Remove empty spaces from if brackets: if ( foo ) -> if (foo)
# -V = Convert tabs to spaces.
# -s4 = 4 spaces per indentation
# -S = Indent switch case-statements.


cp $1 $backupdir/$1
echo "backup written to $backupdir"
astyle -apUV -s4 -S < $1 > $target && mv $target $1 


