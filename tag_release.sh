#!/bin/sh
#
# Run this in shrew's top level dir to tag the code in the given copy
# of shrew. This will read each of the configure.ac files to get the 
# version numbeer, then run svn cp <branch>/<thing>/$1 <tag>/<thing>/<ver>

if test $# != 1
then
    echo "Usage tar_release.sh <branch_name>"
    exit 1
fi

branch_name=$1
modules="libdap bes dap-server csv_handler freeform_handler netcdf_handler \
hdf4_handler hdf5_handler fits_handler gdal_handler ncml_module \
xml_data_handler fileout_gdal fileout_netcdf"

for m in $modules
do
    # AC_INIT(libdap, 3.12.1, opendap-tech@opendap.org)
    if test -f src/$m/configure.ac
    then
	conf_file="src/$m/configure.ac"
    elif test -f src/modules/$m/configure.ac
    then
	conf_file="src/modules/$m/configure.ac"
    else
	echo "Could not find configure.ac to extract the version ($m)"
	exit 1
    fi

    ver=`grep 'AC_INIT' $conf_file | \
	 sed 's@AC_INIT.*,[ \t]*\([0-9]*\.[0-9]*\.[0-9]*\).*@\1@g'`

    if echo "$ver" | grep '[0-9]*\.[0-9]*\.[0-9]*'
    then
	echo "Tagging $m as version $ver"
    else
	echo "Version number error; $ver is not not valid ($m)"
	exit 1
    fi

    svn cp https://scm.opendap.org/svn/branch/$m/$branch_name \
	   https://scm.opendap.org/svn/tags/$m/$ver \
          -m "Tagged as version $ver"
done