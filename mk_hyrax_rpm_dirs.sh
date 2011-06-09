#!/bin/sh
#
# Make the directories for a $dist os for the hyrax RPMs
#
# How to use this: Make the rpms; sign the rpms; transfer the RPMs to
# this machine and put them in the pub/binary/tmp directory. Then run
# the script and enter the component version numbers at the prompt.
#
# jhrg 9/17/2010

do_32_bit="yes"
do_64_bit=

dist=centos5.2
x32=i386
x64=x86_64

pub=/var/www/web_site/pub/binary
# Put the rpms and sig files here before running this script
tmp=$pub/tmp

parts="bes csv_handler dap-server fileout_netcdf freeform_handler \
    gateway_module hdf4_handler hdf5_handler libdap ncml_module netcdf_handler"

# parts="libdap bes hdf4_handler dap-server hdf5_handler fileout_netcdf \
# ncml_module freeform_handler netcdf_handler"

for p in $parts
  do
  # Read the version numbers using the files and a pattern
  f=`ls $tmp/$p-[0-9]*.rpm`
  ver=`echo $f | sed "s@.*-\([0-9]*\.[0-9]*\.[0-9]*\)[-.].*@\1@"`

  # Old way: read the numbers manually
  # read -p "$p version: " ver junk

  echo "ver: $ver"
  # handle the 32-bit case
  if test -n "$do_32_bit"
      then
      d=$p/$dist/${ver}_$x32
      if test ! -d $d
	  then
	  echo "mkdir -p $d"
	  mkdir -p $d
      else
	  echo "$d exists"
      fi

      echo "Copying $tmp/$p*-$ver-?.$x32.rpm* to $d"
      cp $tmp/$p*-$ver-?.$x32.rpm* $d
  fi


  # now do the 64-bit case
  if test -n "$do_64_bit"
      then
      d=$p/$dist/${ver}_$x64
      if test ! -d $d
	  then
	  echo "mkdir -p $d"
	  mkdir -p $d
      else
	  echo "$d exists"
      fi

      echo "Copying $tmp/$p*-$ver-?.$x64.rpm* to $d"
      cp $tmp/$p*-$ver-?.$x64.rpm* $d
  fi
done

