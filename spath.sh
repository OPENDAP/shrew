#!/bin/bash
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

# Can't use ${1:-...}; positional params don't work in the ${:-} syntax
prefix=$1
export prefix=${prefix:-$PWD}
export PATH=$prefix/bin:$PATH

# This is needed for the linux builds; if using the deps libraries
# on linux, those directories also need to be on LD_LIBRARY_PATH.
export LD_LIBRARY_PATH=$prefix/lib

# Find tomcat - it might not be installed in $prefix yet.
unset tc
export tc=`ls -1 $prefix/src/dependencies/downloads/apache-tomcat-*`

if test -f $prefix/apache-tomcat-6.0.29
then
    export TOMCAT_DIR=$prefix/apache-tomcat-6.0.29
elif test -f $tc
then
    tc=${tc##/*/}
    tc=${tc%.tar.gz}
    export TOMCAT_DIR=$prefix/$tc
else
    echo "Can't find tomcat..."
fi

# Added because references to CATALINA_HOME will determine which
# copy of tomcat ./apache-tomcat-*/bin/startup.sh actually starts.
export CATLINA_HOME=$TOMCAT_DIR