#                 -*- Autoconf -*-
# Process this file with autoconf to produce a c...

AC_PREREQ([2.62])
AC_INIT([hyrax], [1.6])
AC_CONFIG_AUX_DIR(conf)
AC_CONFIG_MACRO_DIR([conf])

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([-Wall foreign])
LT_PREREQ([2.2])
LT_INIT([dlopen])

bes_user=`whoami`
AC_SUBST([bes_user])

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

CATALINA_HOME=tomcat
AC_SUBST([CATALINA_HOME])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT