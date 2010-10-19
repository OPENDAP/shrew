ACLOCAL_AMFLAGS = -I conf

EXTRA_DIST = 

SUBDIRS = dap-server netcdf_handler hdf4_handler hdf5_handler \
	freeform_handler fileout_netcdf ncml_module

modules:
	for dir in $(SUBDIRS); do \
		$(MAKE) $(MFLAGS) -C $$dir >> make.$$dir.log 2>&1; \
		echo "%%% make status: $$?" >> make.$$dir.log; \
		$(MAKE) $(MFLAGS) -C $$dir install >> make.$$dir.log 2>&1; \
		echo "%%% install status: $$?" >> make.$$dir.log; \
	done

modules-check:
	-for dir in $(SUBDIRS); do \
		$(MAKE) $(MFLAGS) -C $$dir check >> make.$$dir.log 2>&1; \
		echo "%%% check status: $$?" >> make.$$dir.log; \
	done

modules-distcheck:
	-for dir in $(SUBDIRS); do \
		$(MAKE) $(MFLAGS) -C $$dir distcheck >> make.$$dir.log 2>&1; \
		echo "%%% distcheck status: $$?" >> make.$$dir.log; \
	done

modules-rpm:
	export RPM_OPTIONS="--nodeps --define \"_topdir $prefix/rpm\"";\
	for dir in $(SUBDIRS); do \
		$(MAKE) $(MFLAGS) -C $$dir rpm  >> make.$$dir.log 2>&1; \
		echo "%%% rpm status: $$?" >> make.$$dir.log; \
	done

modules-pkg:
	for dir in $(SUBDIRS); do \
		$(MAKE) $(MFLAGS) -C $$dir pkg  >> make.$$dir.log 2>&1; \
		echo "%%% pkg status: $$?" >> make.$$dir.log; \
	done

cccc:
	for dir in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$dir $@; done

.PHONY: install modules modules-check modules-distcheck modules-rpm \
	modules-pkg cccc
