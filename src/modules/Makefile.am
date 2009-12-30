ACLOCAL_AMFLAGS = -I conf

EXTRA_DIST = 

SUBDIRS = dap-server netcdf_handler hdf4_handler hdf5_handler \
	freeform_handler fileout_netcdf

install:
	for dir in $(SUBDIRS); do \
		if $(MAKE) $(MFLAGS) -C $$dir install; then \
	    	echo "`date`: $$dir: install: 0" >> build.log; \
		else \
	    	echo "`date`: $$dir: install: 1" >> build.log; \
		fi; \
	done

modules-check:
	-for dir in $(SUBDIRS); do \
		if $(MAKE) $(MFLAGS) -C $$dir check; then \
	    	echo "`date`: $$dir: check: 0" >> build.log; \
		else \
	    	echo "`date`: $$dir: check: 1" >> build.log; \
		fi; \
	done

bes-conf:
	for dir in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$dir $@; done

rpm:
	for dir in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$dir $@; done

cccc:
	for dir in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$dir $@; done

.PHONY: bes-conf rpm cccc
