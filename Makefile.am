
ACLOCAL_AMFLAGS = -I conf

EXTRA_DIST = 

# Edit here and in the hyrax-* targets
SUBDIRS = src/libdap src/bes src/modules

# These will be passed to all builds if
# configure gets --enable-debug
# Modify as needed...
debug_flags = --enable-developer CXXFLAGS="-g3 -O0 -W -Wall -Wcast-align -fno-defer-pop"

prefix = --prefix=$(abs_top_builddir)
netcdf = --with-netcdf=/usr/local/netcdf
hdf4 = --with-hdf4=/usr/local/hdf4
hdf5 = --with-hdf5=/usr/local/hdf5
icu = --with-icu-prefix=/usr/local/icu

# Users may change add the external lib variables 
# to specify locations of the external libraries as needed.
# release_params = $(prefix) $(netcdf) $(hdf4) $(hdf5) # $(icu)
release_params = $(prefix) 

#debug_params = $(debug_flags) $(prefix) $(netcdf) $(hdf4) $(hdf5) $(icu)
debug_params = $(debug_flags) $(prefix) 

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
	cd src/$@ && ./configure $(params) > make.$@.log 2>&1
	echo "%%% configure status: $$?" >> make.$@.log
	$(MAKE) $(MFLAGS) -C src/$@ >> make.$@.log 2>&1
	echo "%%% make status: $$?" >> make.$@.log
	$(MAKE) $(MFLAGS) -C src/$@ install >> make.$@.log 2>&1
	echo "%%% install status: $$?" >> make.$@.log

libdap-check:
	$(MAKE) $(MFLAGS) -C src/libdap check >> make.libdap.log 2>&1
	echo "%%% check status: $$?" >> make.libdap.log

libdap-distcheck:
	$(MAKE) $(MFLAGS) -C src/libdap distcheck >> make.libdap.log 2>&1
	echo "%%% distcheck status: $$?" >> make.libdap.log

libdap-rpm:
	RPM_OPTIONS="--nodeps --define \"_topdir $prefix/rpm\"";\
	$(MAKE) $(MFLAGS) -C src/libdap rpm >> make.libdap.log 2>&1
	echo "%%% rpm status: $$?" >> make.libdap.log

bes: debug_msg
	cd src/$@ && ./configure $(params) --enable-developer >> make.$@.log 2>&1
	echo "%%% configure status: $$?" >> make.$@.log
	$(MAKE) $(MFLAGS) -C src/$@ >> make.$@.log 2>&1
	echo "%%% make status: $$?" >> make.$@.log
	$(MAKE) $(MFLAGS) -C src/$@ install >> make.$@.log 2>&1
	echo "%%% install status: $$?" >> make.$@.log

bes-check:
	$(MAKE) $(MFLAGS) -C src/libdap check >> make.bes.log 2>&1
	echo "%%% check status: $$?" >> make.bes.log

bes-distcheck:
	$(MAKE) $(MFLAGS) -C src/bes distcheck >> make.bes.log 2>&1
	echo "%%% distcheck status: $$?" >> make.bes.log

bes-rpm:
	RPM_OPTIONS="--nodeps --define \"_topdir $prefix/rpm\"";\
	$(MAKE) $(MFLAGS) -C src/bes rpm >> make.bes.log 2>&1
	echo "%%% rpm status: $$?" >> make.bes.log

modules: debug_msg
	cd src/$@ && ./configure $(params)
	$(MAKE) $(MFLAGS) -C src/$@ modules-install

modules-check:
	$(MAKE) $(MFLAGS) -C src/modules modules-check

bes-configuration-tests:
	besctl start
	-$(MAKE) $(MFLAGS) -C src/bes bes-configuration-tests
	besctl stop

autoreconf:
	for d in $(SUBDIRS); do (cd $${d} && autoreconf -i -v); done

modules-rpm:
	$(MAKE) $(MFLAGS) -C src/modules modules-rpm

rpm:	libdap-rpm bes-rpm modules-rpm

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
		echo "This operation requires root privileges"; \
		exit 1; \
	fi


.PHONY: world hyrax olfs olfs-compile olfs-install olfs-check 
.PHONY: daemon daemon-check libdap libdap-check bes bes-check 
.PHONY: modules modules-check bes-configuration-tests bes-conf autoreconf 
.PHONY: rpm cccc check-webapps-privileges debug_msg
