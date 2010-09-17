#!/bin/sh
#
# Make the directories for a $dist os for the hyrax RPMs
#
# How to use this: Make the rpms; sign the rpms; transfer the RPMs to
# the web site and put them in the pub/binary/tmp directory. Then run
# the script on the web site in the pub/binary directory and enter the
# component version numbers at the prompt. 
#
# I've added this to the shrew project so the script doesn't get lost.
#
# jhrg 9/17/2010

dist=centos5.2
x32=i386
x64=x86_64

pub=/var/www/web_site/pub/binary
# Put the rpms and sig files here before running this script
tmp=$pub/tmp

parts="libdap bes hdf4_handler dap-server hdf5_handler fileout_netcdf \
ncml_module freeform_handler netcdf_handler"

for p in $parts
do
  read -p "$p version: " ver junk

  echo "ver: $ver"
  # handler the 32-bit case

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

  # now do the 64-bit case

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

done

