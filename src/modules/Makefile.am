ACLOCAL_AMFLAGS = -I conf

EXTRA_DIST = 

SUBDIRS = dap-server netcdf_handler freeform_handler fileout_netcdf \
	ncml_module gateway_module csv_handler hdf4_handler hdf5_handler

.PHONY: $(SUBDIRS)

.PHONY: modules
modules: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ 2>&1 | tee make.$@.log
	echo "%%% make status: $$?" 2>&1 | tee -a  make.$@.log
	$(MAKE) -C $@ install 2>&1 | tee -a  make.$@.log
	echo "%%% install status: $$?" 2>&1 | tee -a  make.$@.log

# .PHONY: modules-check
# modules-check: $(SUBDIRS)
# 	-for dir in $(SUBDIRS); do \
# 	    $(MAKE) -C $$dir check 2>&1 | tee -a make.$$dir.log; \
# 	    echo "%%% check status: $$?" 2>&1 | tee -a make.$$dir.log; \
# 	done

# The following is more efficient when doing parallel builds. The funky
# syntax for the SUBDIRS_CHECK variable appends '-check' to each value
# of $(SUBDIRS)

SUBDIRS_CHECK = $(SUBDIRS:%=%-check)

# ...but, oddly, this .PHONY breaks the rule that runs all of the *-check
# recipes
#.PHONY: $(SUBDIRS_CHECK)

.PHONY: modules-check
modules-check: $(SUBDIRS_CHECK)

%-check:
	$(MAKE) -C $* check 2>&1 | tee -a make.$*.log
	echo "%%% check status: $$?" 2>&1 | tee -a make.$*.log

SUBDIRS_DISTCHECK = $(SUBDIRS:%=%-distcheck)
#.PHONY: $(SUBDIRS_DISTCHECK)

.PHONY: modules-distcheck
modules-distcheck: $(SUBDIRS_DISTCHECK)

%-distcheck: 
	$(MAKE) DISTCHECK_CONFIGURE_FLAGS="$(DISTCHECK_CONFIGURE_FLAGS)" \
	        -C $* distcheck 2>&1 | tee -a  make.$*.log
	echo "%%% distcheck status: $$?" 2>&1 | tee -a  make.$*.log

SUBDIRS_RPM = $(SUBDIRS:%=%-rpm)
#.PHONY: $(SUBDIRS_RPM)

.PHONY: modules-rpm
modules-rpm: $(SUBDIRS_RPM)

# Note that abs_top_builddir is set to $prefix/src/modules so we set _topdir 
# to two levels up to get the rpms all deposited in the same place.
%-rpm: 
	export RPM_OPTIONS="--nodeps --define \"_topdir $(abs_top_builddir)/../../rpm\"";\
	$(MAKE) -C $* rpm 2>&1 | tee -a  make.$*.log
	echo "%%% rpm status: $$?" 2>&1 | tee -a  make.$*.log

SUBDIRS_PKG = $(SUBDIRS:%=%-pkg)
#.PHONY: $(SUBDIRS_PKG)

.PHONY: modules-pkg
modules-pkg: $(SUBDIRS_PKG)

%-pkg: 
	$(MAKE) -C $* pkg  2>&1 | tee -a make.$*.log
	echo "%%% pkg status: $$?" 2>&1 | tee -a  make.$*.log

.PHONY: cccc
cccc:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir $@; done

.PHONY: clean-local
clean-local:
	-rm *.log
