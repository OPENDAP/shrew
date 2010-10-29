#                 -*- Autoconf -*-
# Process this file with autoconf to produce a c...

AC_PREREQ([2.63])
AC_INIT([hyrax], [1.6.2])
AC_CONFIG_AUX_DIR(conf)
AC_CONFIG_MACRO_DIR([conf])

dnl Specify at the top-level configure whether
dnl we want to compile with debug CPPFLAGS
dnl which will minimally be "-g3 -O0".
dnl [TODO Consider taking the options as an 
dnl arg string...  We can't set CPPFLAGS directly
dnl in the configure invocation since they get lost
dnl in Makefile.am with explicit calls to bes and libdap
dnl configure.]
AC_ARG_ENABLE([debug],
[AS_HELP_STRING([--enable-debug],
[Build a debug (-g3 -O0) version of Hyrax 1.6.2 (default is no)])],
[case "${enableval}" in
       yes) build_hyrax_debug=true ;;
       no)  build_hyrax_debug=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
     esac],
[build_hyrax_debug=false]
)
AM_CONDITIONAL([BUILD_HYRAX_DEBUG], [test x$build_hyrax_debug = xtrue])

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([-Wall foreign])
LT_PREREQ([2.2])
LT_INIT([dlopen])

bes_user=`whoami`
AC_SUBST([bes_user])
AC_MSG_NOTICE(Set the BES user to $bes_user)

if groups | grep 'wheel'
then
    bes_group=wheel
elif groups | grep 'admin'
then
    bes_group=admin
else
    bes_group=`whoami`
fi

AC_SUBST([bes_group])
AC_MSG_NOTICE(Set the BES group to $bes_group)

if test -d tomcat -o -L tomcat
then
    CATALINA_HOME="`pwd`/tomcat"
elif ls -1d apache-tomcat-*
then
    tc=`ls -1d apache-tomcat-*`
    CATALINA_HOME="`pwd`/`echo $tc |  sed 's@.a\(apache-.*\)/@\1@'`"
else
    AC_MSG_WARN(I could not find a local tomcat servlet engine directory)
fi

if test -n "$CATALINA_HOME"
then
    AC_SUBST([CATALINA_HOME])
    AC_MSG_NOTICE(Set the CATALINA_HOME to $CATALINA_HOME)
else
    AC_MSG_WARN(I did not set CATALINA_HOME to any value)
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT