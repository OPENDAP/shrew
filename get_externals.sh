#!/bin/sh
#
# This was called 'set_externals.sh' but that name seemed to fit a 
# script that actually set the svn externals property more...

while read svn_dir src_dir
do
    rev=`svn info $src_dir | grep Revision | cut -d ' ' -f 2`
    # echo $svn_dir
    # echo $src_dir
    # echo $rev
    echo "$svn_dir@$rev $src_dir"
done