
ACLOCAL_AMFLAGS = -I conf

EXTRA_DIST = 

# Edit here and in the hyrax-* targets
SUBDIRS = src/libdap src/bes src/modules

# These will be passed to all builds if
# configure gets --enable-debug
# Modify as needed...
debug_flags = --enable-developer CXXFLAGS="-g3 -O0 -W -Wall -Wcast-align -fno-defer-pop" LDFLAGS=-g

olfs_version_arg = -DHYRAX_VERSION=1.8.2 -DOLFS_VERSION=1.9.3

prefix_arg = --prefix=$(prefix)

# We have two different sets of these Makefile variables for OSX and Linux.
# Doing this simplifes managing checkins and the like.
# Only use these if you need them and make sure to double check the values. 
# If they are wrong, configure in the modules dir will fail and that means 
# other parts of the build will fail too.

if BUILD_OSX
netcdf_arg =--with-netcdf=$(prefix)/deps/netcdf-4.1.2
hdf4_arg = --with-hdf4=$(prefix)/deps/hdf-4.2.5 --with-hdfeos2=$(prefix)/deps/hdf-eos2
# hdf4_arg = --with-hdf4=$(prefix)/deps/hdf-4.2.5 
hdf5_arg = --with-hdf5=$(prefix)/deps/hdf5-1.8.6
icu_arg = --with-icu-prefix=$(prefix)/deps/icu-3.6
fits_arg = --with-cfits=$(prefix)/deps/cfitsio
else
netcdf_arg = 
hdf4_arg = --with-hdf4=$(prefix)/deps/hdf-4.2.6
hdf5_arg =
icu_arg =
fits_arg = 
endif

modules_arg = --disable-option-checking $(netcdf_arg) $(hdf4_arg) $(hdf5_arg) $(icu_arg) $(fits_arg)

# Users may change add the external lib variables 
# to specify locations of the external libraries as needed.
release_params = $(prefix_arg) $(modules_arg)

debug_params = $(debug_flags) $(prefix_arg) $(modules_arg)

# Set params depending on whether configure was called with --enable-debug
if BUILD_HYRAX_DEBUG
params = $(debug_params)
else
params = $(release_params)
endif

# Print if we are building debug early in build
.PHONY:
debug_msg:
if BUILD_HYRAX_DEBUG
	@echo "* -------------------------------------*"
	@echo "* Building debug version of Hyrax...   *"
	@echo "* -------------------------------------*"
endif

# This is really for the OS/X build, but it could be used for linux
# too. See above for the conditional variables passed to/for the deps code
.PHONY: universe
universe: depends-install 
	$(MAKE) world

# Note that for a parallel build (make -j N where N > 1) all of the
# dependencies are run in parallel, then the actions are rule in series.
# Thus for the hyrax target, each of check-web..., daemon and olfs are
# run at the same time while doe daemon, libdap, bes, ..., are all built one
# after the other.

.PHONY: world
world: autoreconf 
	$(MAKE) hyrax

.PHONY: hyrax
hyrax: check-webapps-privileges daemon

.PHONY: show_env showEnv
show_env:
showEnv:
	@echo "prefix: " $(prefix) 
	@echo "PATH: " $(PATH)
	@echo "CATALINA_HOME: " $(CATALINA_HOME) 
	@echo "TOMCAT_DIR: " $(TOMCAT_DIR)
	@echo "PACKAGE: " @PACKAGE@ 
	@echo "PACKAGE_VERSION: " @PACKAGE_VERSION@

# Note that the bes Makefile.am seems broken WRT parallel builds. jhrg 4/27/11
.PHONY: daemon
daemon: 
	$(MAKE) libdap 
	$(MAKE) -j 1 bes
	$(MAKE) modules-configure 
	$(MAKE) modules 
	$(MAKE) install-data-local

daemon-check: libdap-check bes-check modules-check bes-configuration-tests

.PHONY: depends depends-install
depends:
depends-install:
	@echo "Building and installing dependencies."
	(cd src/dependencies && ./depends-builder install $(prefix) 2>&1; \
		echo "%%% $@ status: $$?") | tee make.$@.log

.PHONY: depends-pkg
depends-pkg:
	@echo "Building OS-X Package for Hyrax dependencies."
	(cd src/dependencies && ./mkPackage @PACKAGE_VERSION@ $(prefix) 2>&1; \
		echo "%%% mkPackage status: $$?" ) | tee make.$@.log

.PHONY: olfs
olfs: olfs-install

.PHONY: olfs-compile
olfs-compile:
	(cd src/olfs && ant $(olfs_version_arg) )

.PHONY: olfs-install
olfs-install: check-webapps-privileges olfs-compile
	@if test ! -d "$(TOMCAT_DIR)/webapps"; then \
		echo "ERROR: $(TOMCAT_DIR)/webapps is missing."; \
	else \
		cp src/olfs/build/dist/*.war $(TOMCAT_DIR)/webapps;\
		echo "OLFS installed to: " $(TOMCAT_DIR)/webapps; \
	fi

.PHONY: olfs-check
olfs-check:
	@echo "Checking for Hyrax Service..."
	@-curl http://localhost:8080/opendap/version;\
	if test "$$?" -ne "0"; then \
		echo "ERROR! Hyrax must be running at http://localhost:8080/opendap/ for these tests to work."; \
	else \
		echo "Running OLFS Tests."; \
		(cd src/olfs && ./make-check); \
	fi

.PHONY: olfs-osx-package
olfs-osx-package:
	(cd src/olfs && \
	 ant $(olfs_version_arg) -DCATALINA_HOME=$(TOMCAT_DIR) osx-package)

.PHONY: libdap
libdap: debug_msg
	(cd src/$@ && ./configure $(params) 2>&1; echo "%%% configure status: $$?") | tee make.$@.log
	($(MAKE) $(MFLAGS) -C src/$@ 2>&1; echo "%%% make status: $$?") | tee -a make.$@.log
	($(MAKE) $(MFLAGS) -C src/$@ install 2>&1; echo "%%% install status: $$?") | tee -a make.$@.log

.PHONY: libdap-check
libdap-check:
	-($(MAKE) $(MFLAGS) -C src/libdap check 2>&1; echo "%%% check status: $$?") | tee -a make.libdap.log

.PHONY: libdap-distcheck
libdap-distcheck:
	-($(MAKE) $(MFLAGS) -C src/libdap distcheck 2>&1; echo "%%% distcheck status: $$?") | tee -a make.libdap.log 2>&1

.PHONY: libdap-rpm
libdap-rpm:
	export RPM_OPTIONS="--nodeps --define \"_topdir $(abs_top_builddir)/rpm\"";\
	($(MAKE) $(MFLAGS) -C src/libdap rpm 2>&1; echo "%%% rpm status: $$?") | tee -a make.libdap.log

.PHONY: libdap-pkg
libdap-pkg:
	@echo "Building OS-X Package for libdap."
	-($(MAKE) $(MFLAGS) -C src/libdap pkg 2>&1; echo "%%% pkg status: $$?") | tee -a make.libdap.log

.PHONY: bes
bes: debug_msg
	(cd src/$@ && ./configure $(params) 2>&1; echo "%%% configure status: $$?") | tee make.$@.log
	($(MAKE) $(MFLAGS) -C src/$@ 2>&1; echo "%%% make status: $$?") | tee -a make.$@.log
	($(MAKE) $(MFLAGS) -C src/$@ install 2>&1; echo "%%% install status: $$?") | tee -a make.$@.log

.PHONY: bes-check
bes-check:
	-($(MAKE) $(MFLAGS) -j 1 -C src/bes check 2>&1; echo "%%% check status: $$?") | tee -a  make.bes.log

.PHONY: bes-distcheck
bes-distcheck:
	-($(MAKE) $(MFLAGS) -j 1 -C src/bes distcheck 2>&1; echo "%%% distcheck status: $$?") | tee -a make.bes.log

.PHONY: bes-rpm
bes-rpm:
	export RPM_OPTIONS="--nodeps --define \"_topdir $(abs_top_builddir)/rpm\"";\
	($(MAKE) $(MFLAGS) -C src/bes rpm 2>&1; echo "%%% rpm status: $$?") | tee -a  make.bes.log

.PHONY: bes-pkg
bes-pkg:
	@echo "Building OS-X Package for BES."
	-($(MAKE) $(MFLAGS) -j 1 -C src/bes pkg 2>&1; echo "%%% pkg status: $$?") | tee -a make.bes.log

# If we don't ignore errors here, a problem with any single configure
# script on the host will halt all the remaining configure and make operations
# Then make check will run, but will fail for some/most handlers since 
# a bunch of the configure scripts never ran. It's also very confusing to 
# debug.
.PHONY: modules-configure
modules-configure:
	(cd src/modules && ./configure $(params))

.PHONY: modules
modules: debug_msg
	-$(MAKE) $(MFLAGS) -C src/modules modules

.PHONY: modules-check
modules-check:
	-$(MAKE) $(MFLAGS) -C src/modules modules-check

.PHONY: modules-distcheck
modules-distcheck:
	-$(MAKE) $(MFLAGS) DISTCHECK_CONFIGURE_FLAGS="$(modules_arg)" -C src/modules modules-distcheck

.PHONY: modules-rpm
modules-rpm:
	-$(MAKE) $(MFLAGS) -C src/modules modules-rpm

.PHONY: modules-pkg
modules-pkg:
	-$(MAKE) $(MFLAGS) -C src/modules modules-pkg

.PHONY: bes-configuration-tests
bes-configuration-tests:
	besctl start \
		&& $(MAKE) $(MFLAGS) -C src/bes bes-configuration-tests \
		&& besctl stop

.PHONY: autoreconf
autoreconf:
	for d in $(SUBDIRS); do (cd $${d} && autoreconf -i -f -v); done

.PHONY: rpm
rpm:	libdap-rpm bes-rpm modules-rpm

#
# Build the Hyrax meta-package for OS-X
#
PACKAGEMAKER=/Developer/usr/bin/packagemaker

# I removed depends-pkg since that rebuilds the entire dependency
# subtree. I want to move the deps so that they are not built every
# time we run a nightly build and/or have to debug this Makefile.am.
# Also, removed the metapackage build code since it's very hard to
# make general enough to work in a nightly build. jhrg

.PHONY: pkg
pkg: depends-pkg libdap-pkg bes-pkg modules-pkg olfs-osx-package collect-pkgs

.PHONY: collect-pkgs
collect-pkgs:
	@echo "Removing previous package collection..." 
	@-rm -rf osx-packages 
	@-mkdir -p osx-packages 
	@echo "Collecting packages" 
	@find src -name \*.pkg -exec cp -R {} osx-packages \;
	@echo "OS-X Packages have been collected" 

# I don't know how this works... jhrg
.PHONY: meta-pkg
meta-pkg:
	@echo "Removing previous pmdoc..."
	@rm -rf hyrax-package.pmdoc
	@echo "Unpacking prototype pmdoc..."
	@tar -xvf hyrax-package.pmdoc.proto.tar	
	@echo "Installing pmdoc..."
	@for i in `ls -1 hyrax-package.pmdoc/* | grep -v contents`; do \
		echo "Fixing: " $${i}; \
		cat $${i} | sed "s#_PACKAGES_DIR_#`pwd`/osx-packages#g"> foo ;\
		mv foo $${i}; \
	done
	@echo "Building Meta Package..."
	${PACKAGEMAKER} --doc hyrax-package.pmdoc --resources OSX_Resources --out osx-packages/@PACKAGE@-@PACKAGE_VERSION@.pkg

.PHONY: cccc
cccc:
	for d in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $${d} $@; done

.PHONY: clean-local
clean-local:
	-cd src/olfs && ant clean
	-rm *.log

.PHONY: install-data-local
install-data-local: 
	@echo "Bes will run as user " $(bes_user) " group " $(bes_group)
	cat $(prefix)/etc/bes/bes.conf \
	    | sed -e "s/user_name/$(bes_user)/g" \
		  -e "s/group_name/$(bes_group)/g" \
		  -e "s@\.\/bes\.log@$(abs_top_builddir)/var/bes.log@g" \
            > tmp.conf
	mv tmp.conf $(prefix)/etc/bes/bes.conf

.PHONY: check-webapps-privileges
check-webapps-privileges:
	@if test -d "$(TOMCAT_DIR)/webapps" && ! -w "$(TOMCAT_DIR)/webapps"; \
        then \
	    echo "WARNING! This operation requires write permission on $(TOMCAT_DIR)/webapps"; \
	fi
