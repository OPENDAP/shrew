#!/bin/sh
#
# Read the values for the svn externals property from a text file and 
# set it.

if test -n "$1"
then
    ext="$1"
else
    ext="externals.txt"
fi

svn propset svn:externals --file $ext .
