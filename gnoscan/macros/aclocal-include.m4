# This macro adds the name macrodir to the set of directories
# that `aclocal' searches for macros.  
#
# Taken from the emma package, and whoever they may have taken it
# from.
#
# serial 1

dnl AM_ACLOCAL_INCLUDE(macrodir)
AC_DEFUN([AM_ACLOCAL_INCLUDE],
[
	AM_CONDITIONAL(INSIDE_CBE_COMMON, test x = y)

	for k in $1 ; do ACLOCAL="$ACLOCAL -I $k" ; done
])
