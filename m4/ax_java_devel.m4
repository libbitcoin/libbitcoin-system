# Java discovery wrapper, defines JAVA_CPPFLAGS.

AC_DEFUN([AX_JAVA_DEVEL], [

    AX_PROG_JAVAC
    AX_PROG_JAVA
    AX_PROG_JAR
    AX_JNI_INCLUDE_DIR
    AC_PROG_MKDIR_P
    
    JAVA_CPPFLAGS=
    for JNI_INCLUDE_DIR in $JNI_INCLUDE_DIRS; do
      JAVA_CPPFLAGS="$JAVA_CPPFLAGS -I$JNI_INCLUDE_DIR"
    done

    # Set CPP compile flags.
    AC_SUBST([JAVA_CPPFLAGS])

    # Install .class files to ${datadir}/java.
    AC_SUBST([javaexecdir], [${datadir}/java])

    # Install .jar files to ${datadir}/java (also).
    AC_SUBST([jarexecdir], [${datadir}/java])

    # Build .class files in hidden directory.
    AC_SUBST([java_builddir], [.class])
    $MKDIR_P $java_builddir

    # Build .jar files in hidden directory.
    AC_SUBST([jar_builddir], [.jar])
    $MKDIR_P $jar_builddir

    # Instruct Automake to build .class files in ${java_builddir}.
    AC_SUBST([JAVAROOT], [${java_builddir}])
    
    ## Automake doesn't clean the modified JAVAROOT or jar builds,
    ## so add following pattern in Makefile.am instead:
    #
    # dist_noinst_JAVA = \
    #   [java files listed here]
    #
    # CLEANFILES = ${java_builddir}/*.class
    #
    # distclean-local:
    #	rm -rf ${java_builddir} ${jar_builddir}
    #
    ## Automake doesn't build .jar files,
    ## so use following pattern in Makefile.am instead:
    #
    # nodist_jarexec_DATA = \
    #   ${jar_builddir}/[jar name here]-${VERSION}.jar
    #
    # ${nodist_jarexec_DATA}: classnoinst.stamp
    #	${JAR} cf ${JARFLAGS} ${nodist_jarexec_DATA} -C ${java_builddir} .
    #
    # CLEANFILES += ${jar_builddir}/*.jar
])
