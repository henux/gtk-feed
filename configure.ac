# Process this file with autoconf to produce a configure script.

# Copyright (C) 2008 Henri Häkkinen
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

AC_INIT([gtk-feed], [0.0], [henux@users.sourceforge.net])
AM_INIT_AUTOMAKE([-Wall -Werror])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for libraries.
PKG_CHECK_MODULES([GLIB], [gthread-2.0 >= 2.0])
AC_SUBST([GLIB_CFLAGS])
AC_SUBST([GLIB_LIBS])
PKG_CHECK_MODULES([GTK2], [gtk+-2.0 >= 2.0])
AC_SUBST([GTK2_CFLAGS])
AC_SUBST([GTK2_LIBS])
PKG_CHECK_MODULES([XML2], [libxml-2.0 >= 2.0])
AC_SUBST([XML2_CFLAGS])
AC_SUBST([XML2_LIBS])

# Checks for header files.
# Checks for typedefs, structures, and compiler characteristics.
# Checks for library functions.

# Output files.
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
  Makefile
  data/Makefile
  src/Makefile
])
AC_OUTPUT
