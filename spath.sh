#!/bin/sh
#
# Source this in the directory to be 'prefix' or pass the directory
# as the first param. Make this a command using 'alias.' e.g.
# alias spath='source ~/bin/spath.sh'

# sourcing the bashrc resets PATH; we could save the old value but that
# has problems, too.
source ~/.bashrc

# Can't use ${1:-...}; positional params don't work in the ${:-} syntax
prefix=$1
export prefix=${prefix:-$PWD}
export PATH=$prefix/bin:$PATH
export TOMCAT_DIR=$prefix/apache-tomcat-6.0.29
