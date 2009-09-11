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
    CATALINA_HOME=tomcat
elif ls -1d apache-tomcat-*
then
    tc=`ls -1d apache-tomcat-*`
    CATALINA_HOME=`echo $tc |  sed 's@.a\(apache-.*\)/@\1@'`
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