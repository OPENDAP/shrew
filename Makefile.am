
ACLOCAL_AMFLAGS = -I conf

EXTRA_DIST = 

# Edit here and in the hyrax-* targets
SUBDIRS = src/libdap src/bes src/modules

prefix = --prefix=$(abs_top_builddir)
netcdf = --with-netcdf=/usr/local/netcdf
hdf4 = --with-hdf4=/usr/local/hdf4
hdf5 = --with-hdf5=/usr/local/hdf5

config_params = $(prefix) $(netcdf) $(hdf4) $(hdf5)

# The src/* directories are repeated here because the first builds
# are special; we must run configure, make and make install first for
# libdap and then for bes and then for the modules. Other targets are
# like 'make' are normal automake targets and will work once the
# initial build is done. I think...

hyrax-base:
	for d in src/libdap src/bes; do \
	    (cd $$d && autoreconf -i && ./configure $(prefix)) \
	    && $(MAKE) $(MFLAGS) -C $$d \
	    && $(MAKE) $(MFLAGS) -C $$d install; \
	done

hyrax-modules:
	    (cd src/modules && autoreconf -i \
		&& ./configure $(config_params) \
	        && $(MAKE) $(MFLAGS) -C install)

hyrax: hyrax-base hyrax-modules

autoreconf:
	for d in $(SUBDIRS); do (cd $$d && autoreconf -i); done

rpm:
	for dir in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$dir $@; done

.PHONY: hyrax hyrax-base hyrax-modules autoconf rpm
