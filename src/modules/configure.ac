#                 -*- Autoconf -*-
# Process this file with autoconf to produce a c...

AC_PREREQ([2.62])
AC_INIT([hyrax modules], [1.6])
AC_CONFIG_AUX_DIR([conf])
AC_CONFIG_MACRO_DIR([conf])

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([-Wall foreign])
LT_PREREQ([2.2])
LT_INIT([dlopen])

AC_CONFIG_SUBDIRS([dap-server netcdf_handler freeform_handler fileout_netcdf ncml_module gateway_module csv_handler hdf4_handler hdf5_handler fits_handler xml_data_handler gdal_handler fileout_gdal fielout_json ugrid_functions])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
