#!/bin/sh
#
# $Id: nightly_dods_build.sh,v 1.3.2.13 2004/06/14 20:57:23 jimg Exp $
#
# Build code checked out from svn using te shrew project 
#
# Usage: copy the .in file to a plain file, set the build recorder
# username and password and the os name. Then run the script

source nbuild_info.sh

# First build, then process all of the log files and upload the results

export PATH=/usr/local/bin:$PATH
source spath.sh

# Clean, update and initialize shrew
if test "$do_the_build" = "yes"
then
    rm make.*.log
    rm src/modules/make.*.log
    rm -rf bin etc include lib share var

    make -k distclean
    svn update --accept theirs-full
    autoreconf --verbose --force --install
    ./configure --prefix=$prefix --enable-developer

    make dependencies -j $slots

    make world -j $slots

    make daemon-check -j $slots

    make libdap-distcheck -j $slots
    make bes-distcheck -j $slots
    make modules-distcheck -j $slots

    # Now run the packaging targets
    if test "$make_rpm" = "yes"
    then
	make rpm -j $slots
    fi

    if test "$make_pkg" = "yes"
    then
	make pkg
    fi

fi

if test "$process_the_logs" = "yes"
then

host_full=`hostname`
host="`hostname | sed 's/\([^\.]*\)\..*/\1/'`"

if test -n "$os_name"
then
    host=${host}_${os_name}
fi

date=`date +%Y.%m.%d`
platform=`conf/config.guess`
target=all

for build_name in libdap bes dap-server fileout_netcdf freeform_handler \
    hdf4_handler hdf5_handler ncml_module netcdf_handler gateway_module \
    csv_handler fits_handler xml_data_handler gdal_handler fileout_gdal
do
    make_log=${host}.${platform}.${build_name}.${target}.${date}

    echo "Build of ${build_name} using target ${target} on `date`" > $make_log
    echo "Built on ${host_full}, ${platform} (`uname -a`)" >> $make_log

    if test "$build_name" = "libdap" -o "$build_name" = "bes"
    then
	cat make.$build_name.log >> $make_log
    else
	cat src/modules/make.$build_name.log >> $make_log
    fi

    echo "_______________________________________________________" >> $make_log
    echo "Build completed at `date`." >> $make_log 2>&1

    # For all these status codes, 0 indicates success, 1 failure and N/A
    # means not applicable. 
    build_status=`grep '^%%% make status: [0-9]*$' $make_log | sed 's@.*: \([0-9]*\)@\1@'`
    install_status=`grep '^%%% install status: [0-9]*$' $make_log | sed 's@.*: \([0-9]*\)@\1@'`
    check_status=`grep '^%%% check status: [0-9]*$' $make_log | sed 's@.*: \([0-9]*\)@\1@'`
    distcheck_status=`grep '^%%% distcheck status: [0-9]*$' $make_log | sed 's@.*: \([0-9]*\)@\1@'`

    # here we just do minimal sanity checking; if the above code found a 
    # value, it's assumed to be correct.
    if test -z "$build_status"; then build_status=1; fi
    if test -z "$check_status"; then check_status=1; fi
    if test -z "$install_status"; then install_status=1; fi
    if test -z "$distcheck_status"; then distcheck_status=1; fi
      
    if test "$make_rpm" = "yes"
    then
	rpm_status=`grep '^%%% rpm status: [0-9]*$' $make_log | sed 's@.*: \([0-9]*\)@\1@'`
        if test -z "$rpm_status"; then rpm_status=1; fi
    else
	rpm_status="N/A"
    fi

    if test "$make_pkg" = "yes"
    then
	pkg_status=`grep '^%%% pkg status: [0-9]*$' $make_log | sed 's@.*: \([0-9]*\)@\1@'`
        if test -z "$pkg_status"; then pkg_status=1; fi
    else
	pkg_status="N/A"
    fi

    results="compile: $build_status, check: $check_status, install: $install_status, distcheck: $distcheck_status, rpm: $rpm_status, pkg: $pkg_status"

    echo "Results: $results" >> $make_log 2>&1

    # now record the build on the test machine...
    if test "$record_build" = "yes"
    then
	curl --digest --user $USER_PW "http://test.opendap.org/cgi-bin/build_recorder.pl?host=${host}&build=${build_name}&platform=${platform}&date=${date}&compile=${build_status}&check=${check_status}&install=${install_status}&distcheck=${distcheck_status}&rpm=${rpm_status}&pkg=${pkg_status}" > /dev/null 2>&1

        # Copy the log file to test.opendap.org

        curl --digest --user $USER_PW -F name=${make_log} -F uploaded_file=@${make_log} http://test.opendap.org/cgi-bin/build_log_copy.pl > /dev/null 2>&1
    fi

    # keep some log file copies on the local host too...

    if test ! -d $logs_archive
    then
	mkdir $logs_archive
    fi

    very_old=`find $logs_archive -ctime +10`
    if test -n "$very_old"
    then
	rm -f $very_old
    fi

    mv $make_log $logs_archive 2> /dev/null

done

fi