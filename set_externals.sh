#!/bin/sh
#
# NB: I'm not sure if this is used anymore. See the trac page about
# "Making a Branch of Shrew for a Server Release"jhrg 8/1/11 

while read svn_dir src_dir
do
    rev=`svn info $src_dir | grep Revision | cut -d ' ' -f 2`
    # echo $svn_dir
    # echo $src_dir
    # echo $rev
    echo "$svn_dir@$rev $src_dir"
done