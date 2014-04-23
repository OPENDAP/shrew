ACLOCAL_AMFLAGS = -I conf

EXTRA_DIST = 

# List these like this because, at least for now, tests for the
# response-builder modules often depend on the format readers. jhrg
# 12/31/12 

FORMAT_READERS = netcdf_handler freeform_handler csv_handler \
	hdf4_handler hdf5_handler fits_handler gdal_handler

RESPONSE_BUILDERS = dap-server xml_data_handler fileout_netcdf fileout_gdal

PROCESSORS = ncml_module gateway_module ugrid_functions

SUBDIRS =  $(FORMAT_READERS) $(RESPONSE_BUILDERS) $(PROCESSORS)

.PHONY: modules
modules: $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	($(MAKE) -C $@ 2>&1; echo "%%% make status: $$?") | tee make.$@.log
	($(MAKE) -C $@ install 2>&1; echo "%%% install status: $$?") | tee -a  make.$@.log

# The following is more efficient when doing parallel builds. The funky
# syntax for the SUBDIRS_CHECK variable appends '-check' to each value
# of $(SUBDIRS)

# Don't use .PHONY on SUBDIRS_CHECK below.
SUBDIRS_CHECK = $(SUBDIRS:%=%-check)

.PHONY: modules-check
modules-check: $(SUBDIRS_CHECK)

%-check:
	($(MAKE) -C $* check 2>&1; echo "%%% check status: $$?") \
	    | tee -a make.$*.log

SUBDIRS_DISTCHECK = $(SUBDIRS:%=%-distcheck)

.PHONY: modules-distcheck
modules-distcheck: $(SUBDIRS_DISTCHECK)

%-distcheck: 
	($(MAKE) DISTCHECK_CONFIGURE_FLAGS="$(DISTCHECK_CONFIGURE_FLAGS)" \
	        -C $* distcheck 2>&1; echo "%%% distcheck status: $$?") \
	        | tee -a  make.$*.log

SUBDIRS_RPM = $(SUBDIRS:%=%-rpm)

.PHONY: modules-rpm
modules-rpm: $(SUBDIRS_RPM)

# Note that abs_top_builddir is set to $prefix/src/modules so we set _topdir 
# to two levels up to get the rpms all deposited in the same place.
#
# Special case fileout_gdal because centos 6 doen't have a RPM for 1.10.0,
# Which this module needs. I want to make sure there are no other issues
# with the rpm build because it should work for machines with a custom
# rpm install.
%-rpm: 
	if test -z "`echo $$old_path`"; then \
		export old_path=`echo $$PATH`; \
		export PATH=$$prefix/deps/gdal-1.10.0/bin:$$PATH; \
	fi; \
	export RPM_OPTIONS="--nodeps --define \"_topdir $(abs_top_builddir)/../../rpm\"";\
	($(MAKE) -C $* rpm 2>&1; echo "%%% rpm status: $$?") \
	    | tee -a  make.$*.log

SUBDIRS_PKG = $(SUBDIRS:%=%-pkg)

.PHONY: modules-pkg
modules-pkg: $(SUBDIRS_PKG)

%-pkg: 
	($(MAKE) -C $* pkg  2>&1; echo "%%% pkg status: $$?") \
	    | tee -a make.$*.log

.PHONY: cccc
cccc:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir $@; done

.PHONY: clean-local
clean-local:
	-rm *.log
