#!/bin/sh
#
# Source this in the directory to be 'prefix' or pass the directory
# as the first param. Make this a command using 'alias.' e.g.
# alias spath='source ~/bin/spath.sh'

# Save PATH so that if this is run several times in the same shell it
# doesn't just build up more and more junk there.
if test -n "$orig_path"
then
    export PATH=$orig_path
fi

export orig_path=$PATH

# Set 'prefix' to either the first argument or the current working directory.
# Can't use ${1:-...}; positional params don't work in the ${:-} syntax
prefix=$1
export prefix=${prefix:-$PWD}
export PATH=$prefix/bin:$PATH

# This is needed for the linux builds; if using the deps libraries
# on linux, those directories also need to be on LD_LIBRARY_PATH.
# I'mm not sure this is true... jhrg 1/2/13
export LD_LIBRARY_PATH=$prefix/lib

# Find tomcat - it might not be installed in $prefix yet.
# If there's more than one tomcat direcotry or more than one tar.gz
# file, this will fail. jhrg 10/9/12
unset tc
export tc=`ls -1 $prefix/src/dependencies/downloads/apache-tomcat-*`

unset tcp
# export tcp=`ls -1d $prefix/apache-tomcat-[0-9]*.[0-9]*.[0-9]*`
export tcp=`ls -1 -d $prefix/apache-tomcat-* | grep 'apache-tomcat-[0-9]*\.[0-9]*\.[0-9]*$'`

if test -d $tcp
then
    export TOMCAT_DIR=$tcp
    export CATALINA_HOME=$TOMCAT_DIR
elif test -f $tc
then
    tc=${tc##/*/}
    tc=${tc%.tar.gz}
    export TOMCAT_DIR=$prefix/$tc
    export CATALINA_HOME=$TOMCAT_DIR
else
    echo "Can't find tomcat..."
fi
