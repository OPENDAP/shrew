#!/bin/sh
#
# populate a clean VM so that it can run Hyrax using the BES from RPMs
# and the OLFS from a tgz distribution. This script should be run on
# an instantiated VM. You need to be root to run this - or you need to
# be able to run yum and rpm an install things...
#
# Options: v: verbose; n: do nothing but print what would have been done

args=`getopt vn $*`
if [ $? != 0 ]
then
    echo "Usage: $0 [options] where options are:"
    echo "-v: verbose"
    echo "-n: print what would be done"
    exit 2
fi

set -- $args

# Set verbose and do_nothing to false
vernose="no"; quiet="--quiet"; silent="--silent"
do_nothing="no"
for i in $*
do
    case "$i"
	in
        -v)
            verbose="yes"; quiet=""; silent="";
            shift;;
        -n)
            do_nothing="yes";
            shift;;
        --)
            shift; break;;
    esac
done

function do_command {
    if test "$do_nothing" = "yes"
    then
	echo "$1"
    else
	$1
    fi
}

do_command "yum --assumeyes $quiet install openssl libuuid readline zlib libjpeg libxml2 curl"

# bare unix w/o wget and emacs is no fun... (I use wget to get stuff later on)
do_command "yum --assumeyes $quiet install wget emacs"

# ANU does not need to do this since their 'bare' VMs seem to have this already
# TODO Check this
# rpm --install http://mirror.pnl.gov/epel/6/i386/epel-release-6-8.noarch.rpm

# install the handler prereqs, except that gdal 1.10 is not picked up by this
# TODO Check this
do_command "yum --assumeyes $quiet install netcdf hdf hdf5 libicu cfitsio"

# Grab the BES distribution RPMs I put these here instead of a separate file
# because I wanted all of this script in just one place. Yes, it's messy...
package_base="http://www.opendap.org/pub/binary/"

packages="libdap/centos6.4/3.13.0_x86_64/libdap-3.13.0-1.x86_64.rpm \
bes/centos6.4/3.13.1_x86_64/bes-3.13.1-1.el6.x86_64.rpm \
dap-server/centos6.4/4.1.4_x86_64/dap-server-4.1.4-1.x86_64.rpm \
netcdf_handler/centos6.4/3.10.4_x86_64/netcdf_handler-3.10.4-1.x86_64.rpm \
freeform_handler/centos6.4/3.8.8_x86_64/freeform_handler-3.8.8-1.x86_64.rpm \
ncml_module/centos6.4/1.2.4_x86_64/ncml_module-1.2.4-1.x86_64.rpm \
fileout_netcdf/centos6.4/1.2.2_x86_64/fileout_netcdf-1.2.2-1.x86_64.rpm \
ncml_module/centos6.4/1.2.4_x86_64/ncml_module-1.2.4-1.x86_64.rpm \
gateway_module/centos6.4/1.1.2_x86_64/gateway_module-1.1.2-1.x86_64.rpm \
csv_handler/centos6.4/1.0.4_x86_64/csv_handler-1.0.4-1.x86_64.rpm \
xml_data_handler/centos6.4/1.0.4_x86_64/xml_data_handler-1.0.4-1.x86_64.rpm \
fits_handler/centos6.4/1.0.11_x86_64/fits_handler-1.0.11-1.x86_64.rpm"

# GDAL is broken and so is HDF5
# gdal_handler/centos6.4/0.9.4_x86_64/gdal_handler-0.9.4-1.x86_64.rpm
# fileout_gdal/centos6.4/0.9.3_x86_64/fileout_gdal-0.9.3-1.x86_64.rpm
#
# You're going to need this too for GDAL: /gdal-1.10.0-5.11.x86_64.rpm 
# because 1.10.x is not in EPEL yet.
#
# Our hdf5 package has a rpm problem - fix it later
# hdf5_handler/centos6.4/2.2.2_x86_64/hdf5_handler-2.2.2-1.x86_64.rpm

# THG builds a version of the hdf4 handler that is really cool, use it
hdf4_handler="http://hdfeos.org/software/hdf4_handler/rpm/hyrax-1.9.3/CentOS6/hdf4_handler-3.11.5-1.he2.x86_64.rpm"

for package in $packages
do
    if test "$verbose" = "yes"
    then
        echo "Copying package $package"
    fi
    # NB: I looked at using wget for this, and it's likely possible, but
    # I hope we will have a more mainstream way of installing hyrax from a
    # yum repository in a few months, so this is OK for now. jhrg 10/3/14
    do_command "curl --remote-name $silent ${package_base}${package}"
done

if test "$verbose" = "yes"
then
    echo "Copying package $hdf4_handler"
fi
do_command "curl --remote-name $silent $hdf4_handler"

# load the RPMs 
do_command "rpm --install $quiet *.rpm"

# load the prereqs for the OLFS
do_command "yum --assumeyes $quiet install java-1.6.0-openjdk"

# Get tomcat packages in a tar ball; we should switch to the RPM package.
tomcat=http://apache.arvixe.com/tomcat/tomcat-7/v7.0.56/bin/apache-tomcat-7.0.56.tar.gz
tc_base=`echo $tomcat | sed 's@.*\(apache-tomcat-.*\)\.tar\.gz@\1@g'`
echo "tc_base: $tc_base"

if test "$verbose" = "yes"
then
    echo "Copying package $tomcat"
fi
do_command "curl --remote-name $silent $tomcat"

# And the opendap.war file
opendap=http://www.opendap.org/pub/olfs/olfs-1.11.3-webapp.tgz
olfs_base=`echo $opendap | sed 's@.*\(olfs-.*-webapp\)\.tgz@\1@g'`
if test "$verbose" = "yes"
then
    echo "Copying package $opendap"
fi
do_command "curl --remote-name $silent $opendap"

# Set up the tomcat/OLFS stuff
if test "$verbose" = "yes"
then
    echo  "Unpacking tomcat and installing the OLFS servlet for Hyrax"
fi

do_command "tar -xzf $tc_base.tar.gz"
do_command "tar -xzf $olfs_base.tgz"
do_command "cp $olfs_base/opendap.war $tc_base/webapps/"
