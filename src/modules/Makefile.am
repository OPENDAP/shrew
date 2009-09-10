ACLOCAL_AMFLAGS = -I conf

EXTRA_DIST = 

SUBDIRS = dap-server netcdf_handler hdf4_handler hdf5_handler freeform_handler fileout_netcdf

bes-conf:
	for dir in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$dir $@; done

rpm:
	for dir in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$dir $@; done

.PHONY: bes-conf rpm
