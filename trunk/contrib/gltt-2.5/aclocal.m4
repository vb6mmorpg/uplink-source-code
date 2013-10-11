dnl aclocal.m4 generated automatically by aclocal 1.4

dnl Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.





# CHECK FOR COMPILERS

AC_DEFUN(AC_CHECK_COMPILERS,
[
  AC_ARG_ENABLE(debug,[  --enable-debug          creates debugging code [default=no]],
  [
   if test $enableval = "no"; dnl
     then ac_use_debug_code="no"
     else ac_use_debug_code="yes"
   fi
  ], [ac_use_debug_code="no"])

dnl this was AC_PROG_CC. I had to include it manualy, since I had to patch it
  AC_MSG_CHECKING(for a C-Compiler)
  dnl if there is one, print out. if not, don't matter
  AC_MSG_RESULT($CC)

  if test -z "$CC"; then AC_CHECK_PROG(CC, gcc, gcc) fi
  if test -z "$CC"; then AC_CHECK_PROG(CC, cc, cc, , , /usr/ucb/cc) fi
  if test -z "$CC"; then AC_CHECK_PROG(CC, xlc, xlc) fi
  test -z "$CC" && AC_MSG_ERROR([no acceptable cc found in \$PATH])
  AC_PROVIDE([AC_PROG_CC])

  AC_PROG_CC_WORKS
  AC_PROG_CC_GNU

  if test $ac_cv_prog_gcc = yes; then
    have_GCC=yes
  else
    have_GCC=
  fi

  if test -z "$CFLAGS"; then
    if test "$ac_use_debug_code" = "yes"; then
      AC_PROG_CC_G
      if test $ac_cv_prog_cc_g = yes; then
        CFLAGS="-g"
      fi
    else
      if test "$have_GCC" = "yes"; then
        CFLAGS="-O2 -g"
      else
        CFLAGS=""
      fi
    fi

    if test "$have_GCC" = "yes"; then
     CFLAGS="$CFLAGS -Wall"
    fi

  fi

  if test -z "$LDFLAGS" && test "$ac_use_debug_code" = "no" && test "$have_GCC" = "yes"; then
     LDFLAGS=""
  fi


dnl this is AC_PROG_CPP. I had to include it here, since autoconf checks
dnl dependecies between AC_PROG_CPP and AC_PROG_CC (or is it automake?)

  AC_MSG_CHECKING(how to run the C preprocessor)
  # On Suns, sometimes $CPP names a directory.
  if test -n "$CPP" && test -d "$CPP"; then
    CPP=
  fi
  if test -z "$CPP"; then
  AC_CACHE_VAL(ac_cv_prog_CPP,
  [  # This must be in double quotes, not single quotes, because CPP may get
    # substituted into the Makefile and "${CC-cc}" will confuse make.
    CPP="${CC-cc} -E"
    # On the NeXT, cc -E runs the code through the compiler's parser,
    # not just through cpp.
    dnl Use a header file that comes with gcc, so configuring glibc
    dnl with a fresh cross-compiler works.
    AC_TRY_CPP([#include <assert.h>
    Syntax Error], ,
    CPP="${CC-cc} -E -traditional-cpp"
    AC_TRY_CPP([#include <assert.h>
    Syntax Error], , CPP=/lib/cpp))
    ac_cv_prog_CPP="$CPP"])dnl
    CPP="$ac_cv_prog_CPP"
  else
    ac_cv_prog_CPP="$CPP"
  fi
  AC_MSG_RESULT($CPP)
  AC_PROVIDE([AC_PROG_CPP])
  AC_SUBST(CPP)dnl


  AC_MSG_CHECKING(for a C++-Compiler)
  dnl if there is one, print out. if not, don't matter
  AC_MSG_RESULT($CXX)

  if test -z "$CXX"; then AC_CHECK_PROG(CXX, g++, g++) fi
  if test -z "$CXX"; then AC_CHECK_PROG(CXX, CC, CC) fi
  if test -z "$CXX"; then AC_CHECK_PROG(CXX, xlC, xlC) fi
  if test -z "$CXX"; then AC_CHECK_PROG(CXX, DCC, DCC) fi
  test -z "$CXX" && AC_MSG_ERROR([no acceptable C++-compiler found in \$PATH])

  AC_PROG_CXX_WORKS
  AC_PROG_CXX_GNU

  if test $ac_cv_prog_gxx = yes; then
    have_GXX=yes
  else
    AC_MSG_CHECKING(whether we are using SPARC CC)
    have_GXX=
    cat > conftest.C << EOF
#ifdef __SUNPRO_CC
   yes;
#endif
EOF

    ac_try="$CXX -E conftest.C"
    if { (eval echo configure:__online__: \"$ac_try\") 1>&5; (eval $ac_try) 2>&5; } | egrep yes >/dev/null 2>&1; then
      ok_ac_cv_prog_CC=yes
    else
      ok_ac_cv_prog_CC=no
    fi
    AC_MSG_RESULT($ok_ac_cv_prog_CC)
  fi

  if test -z "$CXXFLAGS"; then
    if test "$ac_use_debug_code" = "yes"; then
      AC_PROG_CXX_G
      if test $ac_cv_prog_cxx_g = yes; then
        CXXFLAGS="-g"
      fi
      if test "$ok_ac_cv_prog_CC" = "yes"; then
        CXXFLAGS="$CXXFLAGS -pto"
      fi
    else
      if test "$have_GXX" = "yes"; then
         CXXFLAGS="-O2 -g"
      else
         if test "$ok_ac_cv_prog_CC" = "yes"; then
            CXXFLAGS="-pto -O2"
         else
            CXXFLAGS=""
         fi
      fi
    fi

    if test "$have_GXX" = "yes"; then
       CXXFLAGS="$CXXFLAGS -Wall"
    fi
  fi

])


AC_DEFUN(AC_FIND_FILE,
[
$3=NO
for i in $2;
do
  for j in $1;
  do
    if test -r "$i/$j"; then
      $3=$i
      break 2
    fi
  done
done
])

AC_DEFUN(GLTT_PATH_X,
[
AC_MSG_CHECKING(for X)
AC_CACHE_VAL(ac_cv_have_x,
[# One or both of the vars are not set, and there is no cached value.
ac_x_includes=NO ac_x_libraries=NO
AC_PATH_X_DIRECT
AC_PATH_X_XMKMF
if test "$ac_x_includes" = NO || test "$ac_x_libraries" = NO; then
  AC_MSG_ERROR([Can't find X includes. Please check your installation and add the correct paths!])
else
  # Record where we found X for the cache.
  ac_cv_have_x="have_x=yes \
                ac_x_includes=$ac_x_includes ac_x_libraries=$ac_x_libraries"
fi])dnl
eval "$ac_cv_have_x"

if test "$have_x" != yes; then
  AC_MSG_RESULT($have_x)
  no_x=yes
else
  # If each of the values was on the command line, it overrides each guess.
  test "x$x_includes" = xNONE && x_includes=$ac_x_includes
  test "x$x_libraries" = xNONE && x_libraries=$ac_x_libraries

  # don't set -I/usr/X11R6/include please. It's boring. /S.Rehel
  # IRIX test doesn't support -ef ! Aaah GNU ... Aah linux!
  #
  if test ! -z "$x_includes" ; then
    xdir1=`cd $x_includes/X11 && /bin/pwd`
    xdir2=`cd /usr/include/X11 && /bin/pwd`
    if test "$xdir1" = "$xdir2" ; then
      x_includes=/usr/include
    fi
  fi

  # Update the cache value to reflect the command line values.
  ac_cv_have_x="have_x=yes \
                ac_x_includes=$x_includes ac_x_libraries=$x_libraries"
  AC_MSG_RESULT([libraries $x_libraries, headers $x_includes])

#  CPPFLAGS="$CPPFLAGS -L$x_libraries"
fi

if test -z "$x_includes" || \
   test "$x_includes" = "/usr/include" || \
   test "x$x_includes" = xNONE; then
  X_INCLUDES=""
  x_includes="."; dnl better than nothing :-
 else
  X_INCLUDES="-I$x_includes"
fi

if test -z "$x_libraries" || test "$x_libraries" = xNONE; then
  X_LDFLAGS=""
# It does bad things under IRIX 6.4 /SR Mar.12.1999
#  x_libraries="/usr/lib"; dnl better than nothing :-
  all_libraries=""
 else
  X_LDFLAGS="-L$x_libraries"
  all_libraries=$X_LDFLAGS
fi

AC_SUBST(X_INCLUDES)
AC_SUBST(X_LDFLAGS)
all_includes=$X_INCLUDES
])


AC_DEFUN(AC_PATH_TTF,
[
AC_MSG_CHECKING([for FreeType])
ac_ttf_includes=NO ac_ttf_libraries=NO
ttf_libraries=""
ttf_includes=""
AC_ARG_WITH(ttf-dir,
    [  --with-ttf-dir          where the root of freetype is installed ],
    [  ac_ttf_includes="$withval"/include
       ac_ttf_libraries="$withval"/lib
    ])

AC_ARG_WITH(ttf-includes,
    [  --with-ttf-includes     where the freetype includes are. ],
    [
       ac_ttf_includes="$withval"
    ])

AC_ARG_WITH(ttf-libraries,
    [  --with-ttf-libraries    where the freetype library is installed.],
    [  ac_ttf_libraries="$withval"
    ])

if test "$ac_ttf_includes" = NO || test "$ac_ttf_libraries" = NO; then

AC_CACHE_VAL(ac_cv_have_ttf,
[#try to guess ttf locations

ttf_incdirs="$ac_ttf_includes /usr/lib/freetype/include /usr/local/freetype/include /usr/include/freetype /usr/include /usr/local/include /usr/local/ttf $TTF_INC"
test -n "$TTFDIR" && ttf_incdirs="$TTFDIR/include $TTFDIR $ttf_incdirs"
AC_FIND_FILE(freetype.h, $ttf_incdirs, ttf_incdir)
ac_ttf_includes=$ttf_incdir

ttf_libdirs="$ac_ttf_libraries /usr/lib/freetype/lib /usr/local/freetype/lib /usr/lib/freetype /usr/lib /usr/local/lib /usr/local/ttf/lib $TTFLIB"
test -n "$TTFDIR" && ttf_libdirs="$TTFDIR/lib $TTFDIR $ttf_libdirs"
AC_FIND_FILE(libttf.so libttf.so.1.0.0 libttt.so.1 libttf.a libttf.sl, $ttf_libdirs, ttf_libdir)
ac_ttf_libraries=$ttf_libdir

ac_cxxflags_safe=$CXXFLAGS
ac_ldflags_safe=$LDFLAGS
ac_libs_safe=$LIBS

CXXFLAGS="$CXXFLAGS -I$ttf_incdir"
LDFLAGS="-L$ttf_libdir"
LIBS="$LIBS -lttf"

AC_LANG_CPLUSPLUS
cat > conftest.$ac_ext <<EOF
#include "confdefs.h"
#include "freetype.h"
int main() {
  TT_Engine engine;
  TT_Init_FreeType(&engine);
  return 0;
}
EOF

if AC_TRY_EVAL(ac_link) && test -s conftest; then
  rm -f conftest*
else
  echo "configure: failed program was:" >&AC_FD_CC
  cat conftest.$ac_ext >&AC_FD_CC
  ac_ttf_libraries="NO"
fi
rm -f conftest*
CXXFLAGS=$ac_cxxflags_safe
LDFLAGS=$ac_ldflags_safe
LIBS=$ac_libs_safe

if test "$ac_ttf_includes" = NO || test "$ac_ttf_libraries" = NO; then
  ac_cv_have_ttf="have_ttf=no"
  ac_ttf_notfound=""
  if test "$ac_ttf_includes" = NO; then
    if test "$ac_ttf_libraries" = NO; then
      ac_ttf_notfound="(headers and libraries)";
    else
      ac_ttf_notfound="(headers)";
    fi
  else
    ac_ttf_notfound="(libraries)";
  fi

  AC_MSG_ERROR([FreeType $ac_ttf_notfound not found. Please check your installation! ]);
else
  have_ttf="yes"
fi
])
else
  have_ttf="yes"
fi

eval "$ac_cv_have_ttf"

if test "$have_ttf" != yes; then
  AC_MSG_RESULT([$have_ttf]);
else
  ac_cv_have_ttf="have_ttf=yes \
    ac_ttf_includes=$ac_ttf_includes ac_ttf_libraries=$ac_ttf_libraries"
  AC_MSG_RESULT([libraries $ac_ttf_libraries, headers $ac_ttf_includes])

  ttf_libraries=$ac_ttf_libraries
  ttf_includes=$ac_ttf_includes
fi

AC_SUBST(ttf_libraries)
AC_SUBST(ttf_includes)

if test "$ttf_includes" = "$x_includes" || \
   test "$ttf_includes" = "/usr/include" || \
   test -z "$ttf_includes"; then
 TTF_INCLUDES="";
else
 TTF_INCLUDES="-I$ttf_includes"
 all_includes="$TTF_INCLUDES $all_includes"
fi

if test "$ttf_libraries" = "$x_libraries" || \
   test "$ttf_libraries" = "/usr/lib" || \
   test "$ttf_libraries" = "/usr/local/lib" || \
   test -z "$ttf_libraries"; then
 TTF_LDFLAGS=""
else
 TTF_LDFLAGS="-L$ttf_libraries"
 all_libraries="$TTF_LDFLAGS $all_libraries"
fi

AC_SUBST(TTF_INCLUDES)
AC_SUBST(TTF_LDFLAGS)
])

AC_DEFUN(AC_PATH_GLUT,
[
AC_MSG_CHECKING([for glut])
ac_glut_includes=NO ac_glut_libraries=NO
glut_libraries=""
glut_includes=""
AC_ARG_WITH(glut-dir,
    [  --with-glut-dir         where the root of glut is installed ],
    [  ac_glut_includes="$withval"/include
       ac_glut_libraries="$withval"/lib/glut
    ])

AC_ARG_WITH(glut-includes,
    [  --with-glut-includes    where the glut includes are. ],
    [
       ac_glut_includes="$withval"
    ])

AC_ARG_WITH(glut-libraries,
    [  --with-glut-libraries   where the glut library is installed.],
    [
       ac_glut_libraries="$withval"
    ])

if test "$ac_glut_includes" = NO || test "$ac_glut_libraries" = NO; then

AC_CACHE_VAL(ac_cv_have_glut,
[#try to guess glut locations

GLTT_MAKE_TARGET=lib_and_demos

glut_incdirs="$ac_glut_includes /usr/include /usr/local/include /usr/glut/include /usr/local/glut/include $HOME/glut/include $HOME/Mesa/include $GLUT_INC $gl_includes"
test -n "$GLUTDIR" && glut_incdirs="$GLUTDIR/include $GLUTDIR $glut_incdirs"
AC_FIND_FILE(GL/glut.h, $glut_incdirs, glut_incdir)
ac_glut_includes=$glut_incdir

glut_libdirs="$ac_glut_libraries /usr/lib /usr/local/lib /usr/glut/lib/glut /usr/local/glut/lib/glut $HOME/glut/lib/glut $HOME/Mesa/lib $GLUTLIB $gl_libraries"
test -n "$GLUTDIR" && glut_libdirs="$GLUTDIR/lib/glut $GLUTDIR $glut_libdirs"
AC_FIND_FILE(libglut.so libglut.so.3.6 libglut.so.3 libglut.a libglut.sl, $glut_libdirs, glut_libdir)
ac_glut_libraries=$glut_libdir

ac_cxxflags_safe=$CXXFLAGS
ac_ldflags_safe=$LDFLAGS
ac_libs_safe=$LIBS

CXXFLAGS="$CXXFLAGS -I$glut_incdir -I$gl_includes"
LDFLAGS="-L$glut_libdir"
LIBS="-lglut -L$gl_libraries -l${GL_LIBNAME}GL -l${GL_LIBNAME}GLU $LIBSOCKET $LIBS"

AC_LANG_CPLUSPLUS
cat > conftest.$ac_ext <<EOF
#include "confdefs.h"
#include "GL/glut.h"
int main( int argc, char** argv ) {
  glutInit(&argc, argv);
  return 0;
}
EOF

if AC_TRY_EVAL(ac_link) && test -s conftest; then
  rm -f conftest*
else
  echo "configure: failed program was:" >&AC_FD_CC
  cat conftest.$ac_ext >&AC_FD_CC
  ac_glut_libraries="NO"
fi
rm -f conftest*
CXXFLAGS=$ac_cxxflags_safe
LDFLAGS=$ac_ldflags_safe
LIBS=$ac_libs_safe

if test "$ac_glut_includes" = NO || test "$ac_glut_libraries" = NO; then
  ac_cv_have_glut="have_glut=no"
  ac_glut_notfound=""
  if test "$ac_glut_includes" = NO; then
    if test "$ac_glut_libraries" = NO; then
      ac_glut_notfound="(headers and libraries)";
    else
      ac_glut_notfound="(headers)";
    fi
  else
    ac_glut_notfound="(libraries)";
  fi

  AC_MSG_WARN([glut $ac_glut_notfound not found. Demos won't be built! ]);
  GLTT_MAKE_TARGET=lib_only
else
  have_glut="yes"
fi
ac_make_target=$GLTT_MAKE_TARGET
])
else
  have_glut="yes"
fi

eval "$ac_cv_have_glut"

if test "$have_glut" != yes; then
  AC_MSG_RESULT([$have_glut]);
else
  ac_cv_have_glut="have_glut=yes \
    ac_glut_includes=$ac_glut_includes ac_glut_libraries=$ac_glut_libraries ac_make_target=$GLTT_MAKE_TARGET"
  AC_MSG_RESULT([libraries $ac_glut_libraries, headers $ac_glut_includes])

  glut_libraries=$ac_glut_libraries
  glut_includes=$ac_glut_includes
  GLTT_MAKE_TARGET=$ac_make_target
fi

AC_SUBST(glut_libraries)
AC_SUBST(glut_includes)

if test "$glut_includes" = "$x_includes" || \
   test "$glut_includes" = "/usr/include" || \
   test -z "$glut_includes"; then
 GLUT_INCLUDES="";
else
 GLUT_INCLUDES="-I$glut_includes"
 all_includes="$GLUT_INCLUDES $all_includes"
fi

if test "$glut_libraries" = "$x_libraries" || \
   test "$glut_libraries" = "/usr/lib" || \
   test "$glut_libraries" = "/usr/local/lib" || \
   test -z "$glut_libraries"; then
 GLUT_LDFLAGS=""
else
 GLUT_LDFLAGS="-L$glut_libraries"
 all_libraries="$GLUT_LDFLAGS $all_libraries"
fi

AC_SUBST(GLUT_INCLUDES)
AC_SUBST(GLUT_LDFLAGS)
AC_SUBST(GLTT_MAKE_TARGET)
])


AC_DEFUN(AC_HAVE_GL,
[
AC_MSG_CHECKING([for GL])
ac_gl_includes=NO
ac_gl_libraries=NO
gl_libraries=""
gl_includes=""
AC_ARG_WITH(gl-dir,
    [  --with-gl-dir           where the root of OpenGL is installed ],
    [  ac_gl_includes="$withval"/include
       ac_gl_libraries="$withval"/lib
    ])

AC_ARG_WITH(gl-includes,
    [  --with-gl-includes      where the OpenGL includes are. ],
    [
       ac_gl_includes="$withval"
    ])

AC_ARG_WITH(gl-libraries,
    [  --with-gl-libraries     where the OpenGL library is installed.],
    [  ac_gl_libraries="$withval"
    ])

if test "$ac_gl_includes" = NO || test "$ac_gl_libraries" = NO; then

AC_CACHE_VAL(ac_cv_have_gl,
[#try to guess gl locations

gl_incdirs="$ac_gl_includes \
            /usr/include \
            /usr/X11R6/include \
            /usr/local/include \
            /usr/local/Mesa/include \
            $HOME/Mesa/include \
            $GL_INC"
test -n "$GLDIR" && gl_incdirs="$GLDIR/include $GLDIR $gl_incdirs"
AC_FIND_FILE(GL/gl.h, $gl_incdirs, gl_incdir)
ac_gl_includes=$gl_incdir

gl_libdirs="$ac_gl_libraries \
            /usr/lib \
            /usr/X11R6/lib \
            /usr/local/lib \
            /usr/local/Mesa/lib \
            $HOME/Mesa/lib \
            $GLLIB"
test -n "$GLDIR" && gl_libdirs="$GLDIR/lib $GLDIR $gl_libdirs"

GL_LIBNAME=""
AC_FIND_FILE(libGL.so libGL.a, $gl_libdirs, gl_libdir)
if test "$gl_libdir" = NO ; then
  AC_FIND_FILE(libMesaGL.so libMesaGL.a, $gl_libdirs, gl_libdir)
  ac_gl_libraries=$gl_libdir
  GL_LIBNAME="Mesa"
else
  ac_gl_libraries=$gl_libdir
  GL_LIBNAME=""
fi

AC_FIND_FILE(libGL.so libMesaGL.so libGL.a libMesaGL.a, $gl_libdirs, gl_libdir)
ac_gl_libraries=$gl_libdir

ac_cxxflags_safe=$CXXFLAGS
ac_ldflags_safe=$LDFLAGS
ac_libs_safe=$LIBS

CXXFLAGS="$CXXFLAGS -I$gl_incdir"
LDFLAGS="-L$gl_libdir"
LIBS="-lGL $X_LDFLAGS $LIBS"

AC_LANG_CPLUSPLUS
cat > conftest.$ac_ext <<EOF
#include "confdefs.h"
#include "GL/gl.h"
int main() {
  glEnd();
  return 0;
}
EOF

if AC_TRY_EVAL(ac_link) && test -s conftest; then
  rm -f conftest*
else
  rm -f conftest*
  LIBS="$ac_libs_safe -lMesaGL $X_LDFLAGS -lX11 -lXext -lm"

cat > conftest.$ac_ext <<EOF
#include "confdefs.h"
#include "GL/gl.h"
int main() {
  glEnd();
  return 0;
}
EOF

  if AC_TRY_EVAL(ac_link) && test -s conftest; then
    rm -f conftest*
   else
    echo "configure: failed program was:" >&AC_FD_CC
    cat conftest.$ac_ext >&AC_FD_CC
    ac_gl_libraries="NO"
   fi
fi

rm -f conftest*
CXXFLAGS=$ac_cxxflags_safe
LDFLAGS=$ac_ldflags_safe
LIBS=$ac_libs_safe

if test "$ac_gl_includes" = NO || test "$ac_gl_libraries" = NO; then
  ac_cv_have_gl="have_gl=no"
  ac_gl_notfound=""
  if test "$ac_gl_includes" = NO; then
    if test "$ac_gl_libraries" = NO; then
      ac_gl_notfound="(headers and libraries)";
    else
      ac_gl_notfound="(headers)";
    fi
  else
    ac_gl_notfound="(libraries)";
  fi

  AC_MSG_ERROR([OpenGL $ac_gl_notfound not found. Please check your installation! ]);
else
  have_gl="yes"
fi
])
else
  have_gl="yes"
fi

eval "$ac_cv_have_gl"

if test "$have_gl" != yes; then
  AC_MSG_RESULT([$have_gl]);
else
  ac_cv_have_gl="have_gl=yes \
    ac_gl_includes=$ac_gl_includes ac_gl_libraries=$ac_gl_libraries"
  AC_MSG_RESULT([libraries $ac_gl_libraries/lib${GL_LIBNAME}GL*, headers $ac_gl_includes/GL])

  gl_libraries=$ac_gl_libraries
  gl_includes=$ac_gl_includes
fi

AC_SUBST(gl_libraries)
AC_SUBST(gl_includes)

if test "$gl_includes" = "$x_includes" || \
   test "$gl_includes" = "/usr/include" || \
   test -z "$gl_includes"; then
 GL_INCLUDES="";
else
 GL_INCLUDES="-I$gl_includes"
 all_includes="$GL_INCLUDES $all_includes"
fi

if test "$gl_libraries" = "$x_libraries" || \
   test "$gl_libraries" = "/usr/lib" || \
   test "$gl_libraries" = "/usr/local/lib" || \
   test -z "$gl_libraries"; then
 GL_LDFLAGS=""
else
 GL_LDFLAGS="-L$gl_libraries"
 all_libraries="$GL_LDFLAGS $all_libraries"
fi

AC_SUBST(GL_INCLUDES)
AC_SUBST(GL_LDFLAGS)
AC_SUBST(GL_LIBNAME)
])

dnl just a wrapper to clean up configure.in
AC_DEFUN(GLTT_PROG_LIBTOOL,
[
dnl libtool is only for C, so I must force him
dnl to find the correct flags for C++

ac_save_cc=$CC
ac_save_cflags=$CFLAGS
CC=$CXX
CFLAGS=$CXXFLAGS
AM_PROG_LIBTOOL dnl for libraries
CC=$ac_save_cc
CFLAGS=$ac_save_cflags
])


# serial 40 AC_PROG_LIBTOOL
AC_DEFUN(AC_PROG_LIBTOOL,
[AC_REQUIRE([AC_LIBTOOL_SETUP])dnl

# Save cache, so that ltconfig can load it
AC_CACHE_SAVE

# Actually configure libtool.  ac_aux_dir is where install-sh is found.
CC="$CC" CFLAGS="$CFLAGS" CPPFLAGS="$CPPFLAGS" \
LD="$LD" LDFLAGS="$LDFLAGS" LIBS="$LIBS" \
LN_S="$LN_S" NM="$NM" RANLIB="$RANLIB" \
DLLTOOL="$DLLTOOL" AS="$AS" OBJDUMP="$OBJDUMP" \
${CONFIG_SHELL-/bin/sh} $ac_aux_dir/ltconfig --no-reexec \
$libtool_flags --no-verify $ac_aux_dir/ltmain.sh $host \
|| AC_MSG_ERROR([libtool configure failed])

# Reload cache, that may have been modified by ltconfig
AC_CACHE_LOAD

# This can be used to rebuild libtool when needed
LIBTOOL_DEPS="$ac_aux_dir/ltconfig $ac_aux_dir/ltmain.sh"

# Always use our own libtool.
LIBTOOL='$(SHELL) $(top_builddir)/libtool'
AC_SUBST(LIBTOOL)dnl

# Redirect the config.log output again, so that the ltconfig log is not
# clobbered by the next message.
exec 5>>./config.log
])

AC_DEFUN(AC_LIBTOOL_SETUP,
[AC_PREREQ(2.13)dnl
AC_REQUIRE([AC_ENABLE_SHARED])dnl
AC_REQUIRE([AC_ENABLE_STATIC])dnl
AC_REQUIRE([AC_ENABLE_FAST_INSTALL])dnl
AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_REQUIRE([AC_CANONICAL_BUILD])dnl
AC_REQUIRE([AC_PROG_RANLIB])dnl
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_PROG_LD])dnl
AC_REQUIRE([AC_PROG_NM])dnl
AC_REQUIRE([AC_PROG_LN_S])dnl
dnl

# Check for any special flags to pass to ltconfig.
libtool_flags="--cache-file=$cache_file"
test "$enable_shared" = no && libtool_flags="$libtool_flags --disable-shared"
test "$enable_static" = no && libtool_flags="$libtool_flags --disable-static"
test "$enable_fast_install" = no && libtool_flags="$libtool_flags --disable-fast-install"
test "$ac_cv_prog_gcc" = yes && libtool_flags="$libtool_flags --with-gcc"
test "$ac_cv_prog_gnu_ld" = yes && libtool_flags="$libtool_flags --with-gnu-ld"
ifdef([AC_PROVIDE_AC_LIBTOOL_DLOPEN],
[libtool_flags="$libtool_flags --enable-dlopen"])
ifdef([AC_PROVIDE_AC_LIBTOOL_WIN32_DLL],
[libtool_flags="$libtool_flags --enable-win32-dll"])
AC_ARG_ENABLE(libtool-lock,
  [  --disable-libtool-lock  avoid locking (might break parallel builds)])
test "x$enable_libtool_lock" = xno && libtool_flags="$libtool_flags --disable-lock"
test x"$silent" = xyes && libtool_flags="$libtool_flags --silent"

# Some flags need to be propagated to the compiler or linker for good
# libtool support.
case "$host" in
*-*-irix6*)
  # Find out which ABI we are using.
  echo '[#]line __oline__ "configure"' > conftest.$ac_ext
  if AC_TRY_EVAL(ac_compile); then
    case "`/usr/bin/file conftest.o`" in
    *32-bit*)
      LD="${LD-ld} -32"
      ;;
    *N32*)
      LD="${LD-ld} -n32"
      ;;
    *64-bit*)
      LD="${LD-ld} -64"
      ;;
    esac
  fi
  rm -rf conftest*
  ;;

*-*-sco3.2v5*)
  # On SCO OpenServer 5, we need -belf to get full-featured binaries.
  SAVE_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS -belf"
  AC_CACHE_CHECK([whether the C compiler needs -belf], lt_cv_cc_needs_belf,
    [AC_TRY_LINK([],[],[lt_cv_cc_needs_belf=yes],[lt_cv_cc_needs_belf=no])])
  if test x"$lt_cv_cc_needs_belf" != x"yes"; then
    # this is probably gcc 2.8.0, egcs 1.0 or newer; no need for -belf
    CFLAGS="$SAVE_CFLAGS"
  fi
  ;;

ifdef([AC_PROVIDE_AC_LIBTOOL_WIN32_DLL],
[*-*-cygwin* | *-*-mingw*)
  AC_CHECK_TOOL(DLLTOOL, dlltool, false)
  AC_CHECK_TOOL(AS, as, false)
  AC_CHECK_TOOL(OBJDUMP, objdump, false)
  ;;
])
esac
])

# AC_LIBTOOL_DLOPEN - enable checks for dlopen support
AC_DEFUN(AC_LIBTOOL_DLOPEN, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])])

# AC_LIBTOOL_WIN32_DLL - declare package support for building win32 dll's
AC_DEFUN(AC_LIBTOOL_WIN32_DLL, [AC_BEFORE([$0], [AC_LIBTOOL_SETUP])])

# AC_ENABLE_SHARED - implement the --enable-shared flag
# Usage: AC_ENABLE_SHARED[(DEFAULT)]
#   Where DEFAULT is either `yes' or `no'.  If omitted, it defaults to
#   `yes'.
AC_DEFUN(AC_ENABLE_SHARED, [dnl
define([AC_ENABLE_SHARED_DEFAULT], ifelse($1, no, no, yes))dnl
AC_ARG_ENABLE(shared,
changequote(<<, >>)dnl
<<  --enable-shared[=PKGS]  build shared libraries [default=>>AC_ENABLE_SHARED_DEFAULT],
changequote([, ])dnl
[p=${PACKAGE-default}
case "$enableval" in
yes) enable_shared=yes ;;
no) enable_shared=no ;;
*)
  enable_shared=no
  # Look at the argument we got.  We use all the common list separators.
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:,"
  for pkg in $enableval; do
    if test "X$pkg" = "X$p"; then
      enable_shared=yes
    fi
  done
  IFS="$ac_save_ifs"
  ;;
esac],
enable_shared=AC_ENABLE_SHARED_DEFAULT)dnl
])

# AC_DISABLE_SHARED - set the default shared flag to --disable-shared
AC_DEFUN(AC_DISABLE_SHARED, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
AC_ENABLE_SHARED(no)])

# AC_ENABLE_STATIC - implement the --enable-static flag
# Usage: AC_ENABLE_STATIC[(DEFAULT)]
#   Where DEFAULT is either `yes' or `no'.  If omitted, it defaults to
#   `yes'.
AC_DEFUN(AC_ENABLE_STATIC, [dnl
define([AC_ENABLE_STATIC_DEFAULT], ifelse($1, no, no, yes))dnl
AC_ARG_ENABLE(static,
changequote(<<, >>)dnl
<<  --enable-static[=PKGS]  build static libraries [default=>>AC_ENABLE_STATIC_DEFAULT],
changequote([, ])dnl
[p=${PACKAGE-default}
case "$enableval" in
yes) enable_static=yes ;;
no) enable_static=no ;;
*)
  enable_static=no
  # Look at the argument we got.  We use all the common list separators.
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:,"
  for pkg in $enableval; do
    if test "X$pkg" = "X$p"; then
      enable_static=yes
    fi
  done
  IFS="$ac_save_ifs"
  ;;
esac],
enable_static=AC_ENABLE_STATIC_DEFAULT)dnl
])

# AC_DISABLE_STATIC - set the default static flag to --disable-static
AC_DEFUN(AC_DISABLE_STATIC, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
AC_ENABLE_STATIC(no)])


# AC_ENABLE_FAST_INSTALL - implement the --enable-fast-install flag
# Usage: AC_ENABLE_FAST_INSTALL[(DEFAULT)]
#   Where DEFAULT is either `yes' or `no'.  If omitted, it defaults to
#   `yes'.
AC_DEFUN(AC_ENABLE_FAST_INSTALL, [dnl
define([AC_ENABLE_FAST_INSTALL_DEFAULT], ifelse($1, no, no, yes))dnl
AC_ARG_ENABLE(fast-install,
changequote(<<, >>)dnl
<<  --enable-fast-install[=PKGS]  optimize for fast installation [default=>>AC_ENABLE_FAST_INSTALL_DEFAULT],
changequote([, ])dnl
[p=${PACKAGE-default}
case "$enableval" in
yes) enable_fast_install=yes ;;
no) enable_fast_install=no ;;
*)
  enable_fast_install=no
  # Look at the argument we got.  We use all the common list separators.
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:,"
  for pkg in $enableval; do
    if test "X$pkg" = "X$p"; then
      enable_fast_install=yes
    fi
  done
  IFS="$ac_save_ifs"
  ;;
esac],
enable_fast_install=AC_ENABLE_FAST_INSTALL_DEFAULT)dnl
])

# AC_ENABLE_FAST_INSTALL - set the default to --disable-fast-install
AC_DEFUN(AC_DISABLE_FAST_INSTALL, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
AC_ENABLE_FAST_INSTALL(no)])

# AC_PROG_LD - find the path to the GNU or non-GNU linker
AC_DEFUN(AC_PROG_LD,
[AC_ARG_WITH(gnu-ld,
[  --with-gnu-ld           assume the C compiler uses GNU ld [default=no]],
test "$withval" = no || with_gnu_ld=yes, with_gnu_ld=no)
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_REQUIRE([AC_CANONICAL_BUILD])dnl
ac_prog=ld
if test "$ac_cv_prog_gcc" = yes; then
  # Check if gcc -print-prog-name=ld gives a path.
  AC_MSG_CHECKING([for ld used by GCC])
  ac_prog=`($CC -print-prog-name=ld) 2>&5`
  case "$ac_prog" in
    # Accept absolute paths.
changequote(,)dnl
    [\\/]* | [A-Za-z]:[\\/]*)
      re_direlt='/[^/][^/]*/\.\./'
changequote([,])dnl
      # Canonicalize the path of ld
      ac_prog=`echo $ac_prog| sed 's%\\\\%/%g'`
      while echo $ac_prog | grep "$re_direlt" > /dev/null 2>&1; do
	ac_prog=`echo $ac_prog| sed "s%$re_direlt%/%"`
      done
      test -z "$LD" && LD="$ac_prog"
      ;;
  "")
    # If it fails, then pretend we aren't using GCC.
    ac_prog=ld
    ;;
  *)
    # If it is relative, then search for the first ld in PATH.
    with_gnu_ld=unknown
    ;;
  esac
elif test "$with_gnu_ld" = yes; then
  AC_MSG_CHECKING([for GNU ld])
else
  AC_MSG_CHECKING([for non-GNU ld])
fi
AC_CACHE_VAL(ac_cv_path_LD,
[if test -z "$LD"; then
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}${PATH_SEPARATOR-:}"
  for ac_dir in $PATH; do
    test -z "$ac_dir" && ac_dir=.
    if test -f "$ac_dir/$ac_prog" || test -f "$ac_dir/$ac_prog$ac_exeext"; then
      ac_cv_path_LD="$ac_dir/$ac_prog"
      # Check to see if the program is GNU ld.  I'd rather use --version,
      # but apparently some GNU ld's only accept -v.
      # Break only if it was the GNU/non-GNU ld that we prefer.
      if "$ac_cv_path_LD" -v 2>&1 < /dev/null | egrep '(GNU|with BFD)' > /dev/null; then
	test "$with_gnu_ld" != no && break
      else
	test "$with_gnu_ld" != yes && break
      fi
    fi
  done
  IFS="$ac_save_ifs"
else
  ac_cv_path_LD="$LD" # Let the user override the test with a path.
fi])
LD="$ac_cv_path_LD"
if test -n "$LD"; then
  AC_MSG_RESULT($LD)
else
  AC_MSG_RESULT(no)
fi
test -z "$LD" && AC_MSG_ERROR([no acceptable ld found in \$PATH])
AC_SUBST(LD)
AC_PROG_LD_GNU
])

AC_DEFUN(AC_PROG_LD_GNU,
[AC_CACHE_CHECK([if the linker ($LD) is GNU ld], ac_cv_prog_gnu_ld,
[# I'd rather use --version here, but apparently some GNU ld's only accept -v.
if $LD -v 2>&1 </dev/null | egrep '(GNU|with BFD)' 1>&5; then
  ac_cv_prog_gnu_ld=yes
else
  ac_cv_prog_gnu_ld=no
fi])
])

# AC_PROG_NM - find the path to a BSD-compatible name lister
AC_DEFUN(AC_PROG_NM,
[AC_MSG_CHECKING([for BSD-compatible nm])
AC_CACHE_VAL(ac_cv_path_NM,
[if test -n "$NM"; then
  # Let the user override the test.
  ac_cv_path_NM="$NM"
else
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}${PATH_SEPARATOR-:}"
  for ac_dir in $PATH /usr/ccs/bin /usr/ucb /bin; do
    test -z "$ac_dir" && ac_dir=.
    if test -f $ac_dir/nm || test -f $ac_dir/nm$ac_exeext ; then
      # Check to see if the nm accepts a BSD-compat flag.
      # Adding the `sed 1q' prevents false positives on HP-UX, which says:
      #   nm: unknown option "B" ignored
      if ($ac_dir/nm -B /dev/null 2>&1 | sed '1q'; exit 0) | egrep /dev/null >/dev/null; then
	ac_cv_path_NM="$ac_dir/nm -B"
	break
      elif ($ac_dir/nm -p /dev/null 2>&1 | sed '1q'; exit 0) | egrep /dev/null >/dev/null; then
	ac_cv_path_NM="$ac_dir/nm -p"
	break
      else
	ac_cv_path_NM=${ac_cv_path_NM="$ac_dir/nm"} # keep the first match, but
	continue # so that we can try to find one that supports BSD flags
      fi
    fi
  done
  IFS="$ac_save_ifs"
  test -z "$ac_cv_path_NM" && ac_cv_path_NM=nm
fi])
NM="$ac_cv_path_NM"
AC_MSG_RESULT([$NM])
AC_SUBST(NM)
])

# AC_CHECK_LIBM - check for math library
AC_DEFUN(AC_CHECK_LIBM,
[AC_REQUIRE([AC_CANONICAL_HOST])dnl
LIBM=
case "$host" in
*-*-beos* | *-*-cygwin*)
  # These system don't have libm
  ;;
*-ncr-sysv4.3*)
  AC_CHECK_LIB(mw, _mwvalidcheckl, LIBM="-lmw")
  AC_CHECK_LIB(m, main, LIBM="$LIBM -lm")
  ;;
*)
  AC_CHECK_LIB(m, main, LIBM="-lm")
  ;;
esac
])

# AC_LIBLTDL_CONVENIENCE[(dir)] - sets LIBLTDL to the link flags for
# the libltdl convenience library, adds --enable-ltdl-convenience to
# the configure arguments.  Note that LIBLTDL is not AC_SUBSTed, nor
# is AC_CONFIG_SUBDIRS called.  If DIR is not provided, it is assumed
# to be `${top_builddir}/libltdl'.  Make sure you start DIR with
# '${top_builddir}/' (note the single quotes!) if your package is not
# flat, and, if you're not using automake, define top_builddir as
# appropriate in the Makefiles.
AC_DEFUN(AC_LIBLTDL_CONVENIENCE, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
  case "$enable_ltdl_convenience" in
  no) AC_MSG_ERROR([this package needs a convenience libltdl]) ;;
  "") enable_ltdl_convenience=yes
      ac_configure_args="$ac_configure_args --enable-ltdl-convenience" ;;
  esac
  LIBLTDL=ifelse($#,1,$1,['${top_builddir}/libltdl'])/libltdlc.la
  INCLTDL=ifelse($#,1,-I$1,['-I${top_builddir}/libltdl'])
])

# AC_LIBLTDL_INSTALLABLE[(dir)] - sets LIBLTDL to the link flags for
# the libltdl installable library, and adds --enable-ltdl-install to
# the configure arguments.  Note that LIBLTDL is not AC_SUBSTed, nor
# is AC_CONFIG_SUBDIRS called.  If DIR is not provided, it is assumed
# to be `${top_builddir}/libltdl'.  Make sure you start DIR with
# '${top_builddir}/' (note the single quotes!) if your package is not
# flat, and, if you're not using automake, define top_builddir as
# appropriate in the Makefiles.
# In the future, this macro may have to be called after AC_PROG_LIBTOOL.
AC_DEFUN(AC_LIBLTDL_INSTALLABLE, [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
  AC_CHECK_LIB(ltdl, main,
  [test x"$enable_ltdl_install" != xyes && enable_ltdl_install=no],
  [if test x"$enable_ltdl_install" = xno; then
     AC_MSG_WARN([libltdl not installed, but installation disabled])
   else
     enable_ltdl_install=yes
   fi
  ])
  if test x"$enable_ltdl_install" = x"yes"; then
    ac_configure_args="$ac_configure_args --enable-ltdl-install"
    LIBLTDL=ifelse($#,1,$1,['${top_builddir}/libltdl'])/libltdl.la
    INCLTDL=ifelse($#,1,-I$1,['-I${top_builddir}/libltdl'])
  else
    ac_configure_args="$ac_configure_args --enable-ltdl-install=no"
    LIBLTDL="-lltdl"
    INCLTDL=
  fi
])

dnl old names
AC_DEFUN(AM_PROG_LIBTOOL, [indir([AC_PROG_LIBTOOL])])dnl
AC_DEFUN(AM_ENABLE_SHARED, [indir([AC_ENABLE_SHARED], $@)])dnl
AC_DEFUN(AM_ENABLE_STATIC, [indir([AC_ENABLE_STATIC], $@)])dnl
AC_DEFUN(AM_DISABLE_SHARED, [indir([AC_DISABLE_SHARED], $@)])dnl
AC_DEFUN(AM_DISABLE_STATIC, [indir([AC_DISABLE_STATIC], $@)])dnl
AC_DEFUN(AM_PROG_LD, [indir([AC_PROG_LD])])dnl
AC_DEFUN(AM_PROG_NM, [indir([AC_PROG_NM])])dnl

dnl This is just to silence aclocal about the macro not being used
ifelse([AC_DISABLE_FAST_INSTALL])dnl

