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
# I'm not sure this is true... jhrg 1/2/13
# We do need this for icu-3.6 on AWS EC2 instances. jhrg 3/5/13
export LD_LIBRARY_PATH=$prefix/lib:$prefix/deps/icu-3.6/lib

# I removed the apache tomcat dist from dependencies/downloads 
# because it was causing bloat. Assume that a typical nightly build
# has both the tar.gz and directory for tomcat. jhrg 4/28/14
tc=`ls -d -1 $prefix/apache-tomcat-* | grep -v '.*\.tar\.gz'`
if test -n "$tc"
then
    export TOMCAT_DIR=$tc
    export CATALINA_HOME=$TOMCAT_DIR
else
    echo "Can't find tomcat..."
fi
