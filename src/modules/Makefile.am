ACLOCAL_AMFLAGS = -I conf

EXTRA_DIST = 

SUBDIRS = dap-server netcdf_handler hdf4_handler hdf5_handler \
	freeform_handler fileout_netcdf ncml_module

modules:
	for dir in $(SUBDIRS); do \
		$(MAKE) $(MFLAGS) -C $$dir 2>&1 | tee make.$$dir.log; \
		echo "%%% make status: $$?" | tee -a make.$$dir.log; \
		$(MAKE) $(MFLAGS) -C $$dir install 2>&1 | tee -a make.$$dir.log; \
		echo "%%% install status: $$?" | tee -a make.$$dir.log; \
	done

modules-check:
	-for dir in $(SUBDIRS); do \
		$(MAKE) $(MFLAGS) -C $$dir check  2>&1 | tee -a make.$$dir.log; \
		echo "%%% check status: $$?" | tee -a make.$$dir.log; \
	done

modules-distcheck:
	-for dir in $(SUBDIRS); do \
		$(MAKE) $(MFLAGS) -C $$dir distcheck  2>&1 | tee -a make.$$dir.log; \
		echo "%%% distcheck status: $$?" | tee -a make.$$dir.log; \
	done

modules-rpm:
	export RPM_OPTIONS="--nodeps --define \"_topdir $prefix/rpm\"";\
	for dir in $(SUBDIRS); do \
		$(MAKE) $(MFLAGS) -C $$dir rpm  2>&1 | tee -a make.$$dir.log; \
		echo "%%% rpm status: $$?" | tee -a make.$$dir.log; \
	done

modules-pkg:
	for dir in $(SUBDIRS); do \
		$(MAKE) $(MFLAGS) -C $$dir pkg  2>&1 | tee -a make.$$dir.log; \
		echo "%%% pkg status: $$?" | tee -a make.$$dir.log; \
	done

cccc:
	for dir in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$dir $@; done

.PHONY: install modules modules-check modules-distcheck modules-rpm \
	modules-pkg cccc
