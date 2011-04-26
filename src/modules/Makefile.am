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

.PHONY: modules-check
modules-check: $(SUBDIRS)
	-for dir in $(SUBDIRS); do \
	    $(MAKE) -C $$dir check 2>&1 | tee -a make.$$dir.log; \
	    echo "%%% check status: $$?" 2>&1 | tee -a make.$$dir.log; \
	done

# 	$(MAKE) -C $@ check 2>&1 | tee -a make.$@.log
# 	echo "%%% check status: $$?" 2>&1 | tee -a make.$@.log

.PHONY: modules-distcheck
modules-distcheck: $(SUBDIRS)
	-for dir in $(SUBDIRS); do \
	    $(MAKE) DISTCHECK_CONFIGURE_FLAGS="$(DISTCHECK_CONFIGURE_FLAGS)"\
                    -C $$dir distcheck 2>&1 | tee -a  make.$$dir.log; \
	    echo "%%% distcheck status: $$?" 2>&1 | tee -a  make.$$dir.log; \
	done

# 	$(MAKE) DISTCHECK_CONFIGURE_FLAGS="$(DISTCHECK_CONFIGURE_FLAGS)" \
#	        -C $@ distcheck 2>&1 | tee -a  make.$@.log
# 	echo "%%% distcheck status: $$?" 2>&1 | tee -a  make.$@.log

# Note that abs_top_builddir is set to $prefix/src/modules so we set _topdir 
# to two levels up to get the rpms all deposited in the same place.
.PHONY: modules-rpm
modules-rpm:
	export RPM_OPTIONS="--nodeps --define \"_topdir $(abs_top_builddir)/../../rpm\"";\
	for dir in $(SUBDIRS); do \
	    $(MAKE) -C $$dir rpm 2>&1 | tee -a  make.$$dir.log; \
	    echo "%%% rpm status: $$?" 2>&1 | tee -a  make.$$dir.log; \
	done

.PHONY: modules-pkg
modules-pkg:
	for dir in $(SUBDIRS); do \
	    $(MAKE) -C $$dir pkg  2>&1 | tee -a make.$$dir.log; \
	    echo "%%% pkg status: $$?" 2>&1 | tee -a  make.$$dir.log; \
	done

.PHONY: cccc
cccc:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir $@; done

.PHONY: clean-local
clean-local:
	-rm *.log
