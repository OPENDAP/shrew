
ACLOCAL_AMFLAGS = -I conf

EXTRA_DIST = 

# Edit here and in the hyrax-* targets
SUBDIRS = src/libdap src/bes src/modules

# These will be passed to all builds if
# configure gets --enable-debug
# Modify as needed...
debug_flags = --enable-developer CXXFLAGS="-g3 -O0 -W -Wall -Wcast-align -fno-defer-pop"

olfs_version_arg = -DHYRAX_VERSION=1.6.2 -DOLFS_VERSION=nightly.build

prefix_arg = --prefix=$(prefix) --disable-option-checking

# Only use these if you need them and make sure to double check the values. 
# If they are wrong, configure in the modules dir will fail and that means 
# other parts of the build will fail too.
#netcdf_arg = --with-netcdf=$(prefix)/deps/netcdf-3.6.3
#hdf4_arg = --with-hdf4=$(prefix)/deps/hdf-4.2.5 --with-hdfeos2=$(prefix)/deps/hdf-4.2.5 --enable-short-name
#hdf5_arg = --with-hdf5=$(prefix)/deps/hdf5-1.8.5-patch1
#icu_arg = --with-icu-prefix=$(prefix)/deps/icu-3.6

modules_arg = $(netcdf_arg) $(hdf4_arg) $(hdf5_arg) $(icu_arg)

# Users may change add the external lib variables 
# to specify locations of the external libraries as needed.
release_params = $(prefix_arg) $(modules_arg)
# release_params = $(prefix_arg) 

debug_params = $(debug_flags) $(prefix_arg) $(modules_arg)
# debug_params = $(debug_flags) $(prefix_arg) 

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
	cd src/olfs && ant $(olfs_version_arg)

olfs-install: check-webapps-privileges olfs-compile
	@if [[ ! -d "$(TOMCAT_DIR)/webapps" ]]; then \
		echo "ERROR!! $(TOMCAT_DIR)/webapps is missing."; \
		echo "ERROR!! There is no place to put the OLFS web application archive! OLFS NOT INSTALLED."; \
	else \
		cp src/olfs/build/dist/*.war $(TOMCAT_DIR)/webapps;\
		echo "OLFS installed to: " $(TOMCAT_DIR)/webapps; \
	fi


olfs-check:
	@echo "Checking for Hyrax Service..."
	@-curl http://localhost:8080/opendap/version;\
	if [ "$$?" -ne "0" ]; then \
		echo "ERROR! Hyrax must be running at http://localhost:8080/opendap/ for these tests to work."; \
	else \
		echo "Running OLFS Tests."; \
		cd src/olfs && ./make-check; \
	fi
	
olfs-osx-package:
	cd src/olfs && ant $(olfs_version_arg) -DCATALINA_HOME=$(TOMCAT_DIR) osx-package

showEnv:
	@echo "prefix: " $(prefix) 
	@echo "CATALINA_HOME: " $(CATALINA_HOME) 
	@echo "TOMCAT_DIR: " $(TOMCAT_DIR)

	


daemon: libdap bes modules-configure modules install-data-local

daemon-check: libdap-check bes-check modules-check bes-configuration-tests

depends:
	@echo "Building dependencies."
	(cd src/dependencies && ./depends-builder build $(prefix) ) > make.$@.log 2>&1
	echo "%%% $@ status: $$?" >> make.$@.log

depends-check: 
	@echo "Building and checking dependencies."
	(cd src/dependencies && ./depends-builder check $(prefix) ) > make.$@.log 2>&1
	echo "%%% $@ status: $$?" >> make.$@.log

depends-install:
	@echo "Building and installing dependencies."
	(cd src/dependencies && ./depends-builder install $(prefix) ) > make.$@.log 2>&1
	(./configure --prefix=$(prefix) ) >> make.$@.log 2>&1
	echo "%%% $@ status: $$?" >> make.$@.log

# This runs the depends-install task/target/action thingy from inside
# the mlPackage script (so no need to run it here as a separate call
# to make).
depends-pkg:
	@echo "Building OS-X Package for Hyrax dependencies."
	(cd src/dependencies && ./mkPackage @PACKAGE_VERSION@ $(prefix) ) > make.$@.log 2>&1
	echo "%%% mkPackage status: $$?" >> make.$@.log

libdap: debug_msg
	(cd src/$@ && ./configure $(params) ) > make.$@.log 2>&1
	echo "%%% configure status: $$?" >> make.$@.log
	$(MAKE) $(MFLAGS) -C src/$@ >> make.$@.log 2>&1
	echo "%%% make status: $$?" >> make.$@.log
	$(MAKE) $(MFLAGS) -C src/$@ install >> make.$@.log 2>&1
	echo "%%% install status: $$?" >> make.$@.log

libdap-check:
	-$(MAKE) $(MFLAGS) -C src/libdap check >> make.libdap.log 2>&1
	echo "%%% check status: $$?" >> make.libdap.log

libdap-distcheck:
	-$(MAKE) $(MFLAGS) -C src/libdap distcheck >> make.libdap.log 2>&1
	echo "%%% distcheck status: $$?" >> make.libdap.log

libdap-rpm:
	export RPM_OPTIONS="--nodeps --define \"_topdir $(abs_top_builddir)/rpm\"";\
	$(MAKE) $(MFLAGS) -C src/libdap rpm >> make.libdap.log 2>&1
	echo "%%% rpm status: $$?" >> make.libdap.log

libdap-pkg:
	-$(MAKE) $(MFLAGS) -C src/libdap pkg >> make.libdap.log 2>&1
	echo "%%% pkg status: $$?" >> make.libdap.log

bes: debug_msg
	(cd src/$@ && ./configure $(params) --enable-developer) \
		>> make.$@.log 2>&1
	echo "%%% configure status: $$?" >> make.$@.log
	$(MAKE) $(MFLAGS) -C src/$@ >> make.$@.log 2>&1
	echo "%%% make status: $$?" >> make.$@.log
	$(MAKE) $(MFLAGS) -C src/$@ install >> make.$@.log 2>&1
	echo "%%% install status: $$?" >> make.$@.log

bes-check:
	-$(MAKE) $(MFLAGS) -C src/libdap check >> make.bes.log 2>&1
	echo "%%% check status: $$?" >> make.bes.log

bes-distcheck:
	-$(MAKE) $(MFLAGS) -C src/bes distcheck >> make.bes.log 2>&1
	echo "%%% distcheck status: $$?" >> make.bes.log

bes-rpm:
	export RPM_OPTIONS="--nodeps --define \"_topdir $(abs_top_builddir)/rpm\"";\
	$(MAKE) $(MFLAGS) -C src/bes rpm >> make.bes.log 2>&1
	echo "%%% rpm status: $$?" >> make.bes.log

bes-pkg:
	-$(MAKE) $(MFLAGS) -C src/bes pkg >> make.bes.log 2>&1
	echo "%%% pkg status: $$?" >> make.bes.log

# If we don't ignore errors here, a problem with any single configure
# script on the host will halt all the remaining configure and make operations
# Then make check will run, but will fail for some/most handlers since 
# a bunch of the configure scripts never ran. It's also very confusing to 
# debug.
modules-configure:
	(cd src/modules && ./configure $(params))

modules: debug_msg
	-$(MAKE) $(MFLAGS) -C src/modules modules

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
	for d in $(SUBDIRS); do (cd $${d} && autoreconf -i -f -v); done

rpm:	libdap-rpm bes-rpm modules-rpm

pkg:	libdap-pkg bes-pkg modules-pkg

cccc:
	for d in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $${d} $@; done

clean-local:
	cd src/olfs && ant clean

install-data-local: 
	@echo "Bes will run as user " $(bes_user) " group " $(bes_group)
	cat $(prefix)/etc/bes/bes.conf \
		| sed -e "s/user_name/$(bes_user)/g" \
		      -e "s/group_name/$(bes_group)/g" \
		      -e "s@\.\/bes\.log@$(abs_top_builddir)/var/bes.log@g" > tmp.conf
	mv tmp.conf $(prefix)/etc/bes/bes.conf

check-webapps-privileges:
	if [[ -d "$(TOMCAT_DIR)/webapps" && ! -w "$(TOMCAT_DIR)/webapps" ]]; then \
		echo "WARNING! This operation requires write permission on $(TOMCAT_DIR)/webapps"; \
	fi


.PHONY: world hyrax olfs olfs-compile olfs-install olfs-check olfs-osx-package 
.PHONY: daemon daemon-check libdap libdap-check bes bes-check 
.PHONY: modules modules-check bes-configuration-tests bes-conf autoreconf 
.PHONY: rpm cccc check-webapps-privileges debug_msg pkg

