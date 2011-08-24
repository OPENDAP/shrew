#!/bin/sh
#
# Use vmware virtual machine for the nightly builds. Assume that the code
# has been already checked out and the nbuild script set up.

# Not needed for linux hosts
# export PATH=/Library/Application\ Support/VMware\ Fusion/:$PATH

# export host_type="-T fusion"
export host_type=
# export guest=/Users/jimg/OPeNDAP_Build_CentOS_5.2_i386.vmwarevm
export guest=/home/jimg/vmware/CentOS_5.6_32-bit/CentOS_5.6_32-bit.vmx
export guest_user=opendap
export guest_pass=builds

# If the VMware gui thing is already running, the we have to use 'gui' with
# start or vmrun reports an error that the "Error: The file is already in use"
# I think that 'nogui' only works when the VMware application is not running.

vmrun $host_type start $guest nogui

vmrun $host_type -gu $guest_user -gp $guest_pass runScriptInGuest $guest \
/bin/sh "cd /home/opendap/hyrax_1.7_release && sh -vx ./nbuild_shrew.sh > nbuild.log 2>&1"

vmrun $host_type suspend $guest soft
