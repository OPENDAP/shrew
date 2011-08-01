#!/bin/sh
#
# Use vmware virtual machine for the nightly builds. Assume that the code
# has been already checked out and the nbuild script set up.

export PATH=/Library/Application\ Support/VMware\ Fusion/:$PATH

export host_type=fusion
export guest=/Users/jimg/OPeNDAP_Build_CentOS_5.2_i386.vmwarevm
export guest_user=opendap
export guest_pass=build

# If the VMware gui thing is already running, the we have to use 'gui' with
# start or vmrun reports an error that the "Error: The file is already in use"
# I think that 'nogui' only works when the VMware application is not running.

vmrun -T $host_type start $guest nogui

vmrun -T $host_type -gu $guest_user -gp $guest_pass runScriptInGuest $guest \
/bin/sh "cd /home/opendap/hyrax_1.7_release && sh -vx ./nbuild_shrew.sh > nbuild.log 2>&1"

vmrun -T $host_type suspend $guest soft
