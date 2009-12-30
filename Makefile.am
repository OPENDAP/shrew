
ACLOCAL_AMFLAGS = -I conf

EXTRA_DIST = 

# Edit here and in the hyrax-* targets
SUBDIRS = src/libdap src/bes src/modules

prefix = --prefix=$(abs_top_builddir)
netcdf = --with-netcdf=/usr/local/netcdf
hdf4 = --with-hdf4=/usr/local/hdf4
hdf5 = --with-hdf5=/usr/local/hdf5

params = $(prefix) $(netcdf) $(hdf4) $(hdf5)

# The src/* directories are repeated here because the first builds
# are special; we must run configure, make and make install first for
# libdap and then for bes and then for the modules. Other targets are
# like 'make' are normal automake targets and will work once the
# initial build is done. I think...

world: autoreconf hyrax

hyrax: daemon olfs
hyrax-check: check daemon-check olfs-check

olfs: olfs-compile olfs-install

olfs-compile:
	cd src/olfs && ant

olfs-install:
	cd src/olfs && cp build/dist/*.war $(CATALINA_HOME)/webapps

# How do we run the OLFS-specific tests?
olfs-check:
	echo "Don't know how to check the complete Hyrax server"

daemon: libdap bes modules install-data-local

# redefine the automake 'check' target
daemon-check: libdap-check bes-check modules-check bes-configuration-tests
	
libdap:
	cd src/$@ && ./configure $(prefix)
	if $(MAKE) $(MFLAGS) -C src/libdap install; then \
	    echo "`date`: libdap: install: 0" >> build.log; \
	else \
	    echo "`date`: libdap: install: 1" >> build.log; \
	fi

libdap-check:
	-if $(MAKE) $(MFLAGS) -C src/libdap check; then \
	    echo "`date`: libdap: check: 0" >> build.log; \
	else \
	    echo "`date`: libdap: check: 1" >> build.log; \
	fi

bes:
	cd src/$@ && ./configure $(prefix) --enable-developer
	if $(MAKE) $(MFLAGS) -C src/$@ install; then \
	    echo "`date`: $@: install: 0" >> build.log; \
	else \
	    echo "`date`: $@: install: 1" >> build.log; \
	fi

bes-check:
	-if $(MAKE) $(MFLAGS) -C src/bes check; then \
	    echo "`date`: bes: check: 0" >> build.log; \
	else \
	    echo "`date`: bes: check: 1" >> build.log; \
	fi

modules:
	cd src/$@ && ./configure $(params)
	$(MAKE) $(MFLAGS) -C src/$@ install

modules-check:
	$(MAKE) $(MFLAGS) -C src/modules modules-check
	
bes-configuration-tests:
	besctl start
	-$(MAKE) $(MFLAGS) -C src/bes bes-configuration-tests
	besctl stop

bes-conf:
	$(MAKE) $(MFLAGS) -C src/modules $@

autoreconf:
	for d in $(SUBDIRS); do (cd $${d} && autoreconf -i -v); done

rpm:
	for d in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $${d} $@; done

cccc:
	for d in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $${d} $@; done

clean-local:
	cd src/olfs && ant clean

install-data-local: bes-conf
	@echo "Bes will run as user:" $(bes_user) " group:" $(bes_group)
	cat etc/bes/bes.conf \
		| sed -e "s/user_name/$(bes_user)/g" \
		      -e "s/group_name/$(bes_group)/g" \
		      -e "s@\.\/bes\.log@$(abs_top_builddir)/var/bes.log@g" > tmp.conf
	mv tmp.conf etc/bes/bes.conf

.PHONY: world hyrax daemon libdap bes modules bes-conf autoconf rpm cccc
