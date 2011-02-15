
ACLOCAL_AMFLAGS = -I conf

EXTRA_DIST = 

# Edit here and in the hyrax-* targets
SUBDIRS = src/libdap src/bes src/modules

# These will be passed to all builds if
# configure gets --enable-debug
# Modify as needed...
debug_flags = --enable-developer CXXFLAGS="-g3 -O0 -W -Wall -Wcast-align -fno-defer-pop"

prefix = --prefix=$(abs_top_builddir)

netcdf = --with-netcdf=$(abs_top_builddir)/deps/netcdf-3.6.3
hdf4 = --with-hdf4=$(abs_top_builddir)/deps/hdf-4.2.5
hdf5 = --with-hdf5=$(abs_top_builddir)/deps/hdf5-1.8.5-patch1
icu = --with-icu-prefix=$(abs_top_builddir)/deps/icu-3.6

# Users may change add the external lib variables 
# to specify locations of the external libraries as needed.
release_params = $(prefix) $(netcdf) $(hdf4) $(hdf5) $(icu)
# release_params = $(prefix) 

debug_params = $(debug_flags) $(prefix) $(netcdf) $(hdf4) $(hdf5) $(icu)
# debug_params = $(debug_flags) $(prefix) 

# Set params depending on whether configure was called with --enable-debug
if BUILD_HYRAX_DEBUG
params = $(debug_params)
else
params = $(release_params)
endif

# Print if we are building debug early in build
debug_msg:
if BUILD_HYRAX_DEBUG
	@echo "* -------------------------------------*"
	@echo "* Building debug version of Hyrax...   *"
	@echo "* -------------------------------------*"
endif

# The src/* directories are repeated here because the first builds
# are special; we must run configure, make and make install first for
# libdap and then for bes and then for the modules. Other targets are
# like 'make' are normal automake targets and will work once the
# initial build is done.

world: check-webapps-privileges autoreconf hyrax

hyrax: check-webapps-privileges daemon olfs

olfs: check-webapps-privileges olfs-compile olfs-install

olfs-compile:
	cd src/olfs && ant

olfs-install: check-webapps-privileges
	cd src/olfs && cp build/dist/*.war $(CATALINA_HOME)/webapps

# How do we run the OLFS-specific tests?
olfs-check:
	echo "Don't know how to check the complete Hyrax server"

daemon: libdap bes modules install-data-local

daemon-check: libdap-check bes-check modules-check bes-configuration-tests

libdap: debug_msg
	echo "%%% make $@ invoked at `date`..." | tee make.$@.log
	(cd src/$@ && ./configure $(params) ) 2>&1 | tee -a make.$@.log 
	echo "%%% configure status: $$?" | tee -a make.$@.log
	$(MAKE) $(MFLAGS) -C src/$@ 2>&1 | tee -a make.$@.log 
	echo "%%% make status: $$?" | tee -a make.$@.log
	$(MAKE) $(MFLAGS) -C src/$@ install 2>&1 | tee -a make.$@.log 
	echo "%%% install status: $$?" | tee -a make.$@.log

libdap-check:
	-$(MAKE) $(MFLAGS) -C src/libdap check 2>&1 | tee -a make.libdap.log 
	echo "%%% check status: $$?" | tee -a make.libdap.log

libdap-distcheck:
	-$(MAKE) $(MFLAGS) -C src/libdap distcheck 2>&1 | tee -a make.libdap.log 
	echo "%%% distcheck status: $$?" | tee -a make.libdap.log

libdap-rpm:
	-RPM_OPTIONS="--nodeps --define \"_topdir $prefix/rpm\"";\
	$(MAKE) $(MFLAGS) -C src/libdap rpm 2>&1 | tee -a make.libdap.log 
	echo "%%% rpm status: $$?" | tee -a make.libdap.log

libdap-pkg:
	-$(MAKE) $(MFLAGS) -C src/libdap pkg 2>&1 | tee -a make.libdap.log 
	echo "%%% pkg status: $$?" | tee -a make.libdap.log

bes: debug_msg
	echo "%%% make $@ invoked at `date`..." | tee make.$@.log
	(cd src/$@ && ./configure $(params) --enable-developer) \
		2>&1 | tee -a make.$@.log 
	echo "%%% configure status: $$?" | tee -a make.$@.log
	$(MAKE) $(MFLAGS) -C src/$@ 2>&1 | tee -a make.$@.log 
	echo "%%% make status: $$?" | tee -a make.$@.log
	$(MAKE) $(MFLAGS) -C src/$@ install 2>&1 | tee -a make.$@.log 
	echo "%%% install status: $$?" | tee -a make.$@.log

bes-check:
	-$(MAKE) $(MFLAGS) -C src/libdap check 2>&1 | tee -a make.bes.log 
	echo "%%% check status: $$?" | tee -a make.bes.log

bes-distcheck:
	-$(MAKE) $(MFLAGS) -C src/bes distcheck 2>&1 | tee -a make.bes.log 
	echo "%%% distcheck status: $$?" | tee -a make.bes.log

bes-rpm:
	-RPM_OPTIONS="--nodeps --define \"_topdir $prefix/rpm\"";\
	$(MAKE) $(MFLAGS) -C src/bes rpm 2>&1 | tee -a make.bes.log 
	echo "%%% rpm status: $$?" | tee -a make.bes.log

bes-pkg:
	-$(MAKE) $(MFLAGS) -C src/bes pkg 2>&1 | tee -a make.bes.log 
	echo "%%% pkg status: $$?" | tee -a make.bes.log

modules: debug_msg
	(cd src/$@ && ./configure $(params))
	-$(MAKE) $(MFLAGS) -C src/$@ modules

modules-check:
	-$(MAKE) $(MFLAGS) -C src/modules modules-check

modules-distcheck:
	-$(MAKE) $(MFLAGS) -C src/modules modules-distcheck

modules-rpm:
	-$(MAKE) $(MFLAGS) -C src/modules modules-rpm

modules-pkg:
	-$(MAKE) $(MFLAGS) -C src/modules modules-pkg

bes-configuration-tests:
	besctl start
	-$(MAKE) $(MFLAGS) -C src/bes bes-configuration-tests
	besctl stop

autoreconf:
	for d in $(SUBDIRS); do (cd $${d} && autoreconf -i -v); done

rpm:	libdap-rpm bes-rpm modules-rpm

pkg:	libdap-pkg bes-pkg modules-pkg

cccc:
	for d in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $${d} $@; done

clean-local:
	cd src/olfs && ant clean

install-data-local: 
	@echo "Bes will run as user " $(bes_user) " group " $(bes_group)
	cat etc/bes/bes.conf \
		| sed -e "s/user_name/$(bes_user)/g" \
		      -e "s/group_name/$(bes_group)/g" \
		      -e "s@\.\/bes\.log@$(abs_top_builddir)/var/bes.log@g" > tmp.conf
	mv tmp.conf etc/bes/bes.conf

check-webapps-privileges:
	if [[ ! -w "$(CATALINA_HOME)/webapps" ]]; then \
		echo "This operation requires write privileges on $(CATALINA_HOME)/webapps"; \
		exit 1; \
	fi


.PHONY: world hyrax olfs olfs-compile olfs-install olfs-check 
.PHONY: daemon daemon-check libdap libdap-check bes bes-check 
.PHONY: modules modules-check bes-configuration-tests bes-conf autoreconf 
.PHONY: rpm cccc check-webapps-privileges debug_msg pkg

