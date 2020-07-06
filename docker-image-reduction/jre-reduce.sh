#!/bin/bash

RC_SUCCESS=0
RC_FAILURE=1

JAVA_HOME="$1"
if [ -z "$JAVA_HOME" ]
then
  echo "USAGE: $(basename $0) {JRE relative path}"
  exit $RC_FAILURE
fi

JAVA_HOME="$PWD/$JAVA_HOME"

#
# Remove optional JRE files according to
# http://www.oracle.com/technetwork/java/javase/jre-8-readme-2095710.html
#
echo "JRE size before removing optional files: $(du -sh $JAVA_HOME)"
echo

cd $JAVA_HOME

find lib/ext -type f ! -name 'sun*' -print -delete
rm -rfv bin/rmid \
        bin/rmiregistry \
        bin/tnameserv \
        bin/keytool \
        bin/kinit \
        bin/klist \
        bin/ktab \
        bin/policytool \
        bin/orbd \
        bin/servertool \
        bin/javaws \
        lib/javaws.jar \
        lib/jfr \
        lib/jfr.jar \
        lib/oblique-fonts/* \
        lib/desktop/* \
        plugin/* \
        THIRDPARTYLICENSEREADME-JAVAFX.txt \
        lib/ant-javafx.jar \
        lib/javafx.properties \
        lib/jfxswt.jar \
        lib/amd64/libdecora_sse.so \
        lib/amd64/libprism_common.so \
        lib/amd64/libprism_es2.so \
        lib/amd64/libprism_sw.so \
        lib/amd64/libfxplugins.so \
        lib/amd64/libglass.so \
        lib/amd64/libgstreamer-lite.so \
        lib/amd64/libjavafx_font_freetype.so \
        lib/amd64/libjavafx_font_pango.so \
        lib/amd64/libjavafx_font_t2k.so \
        lib/amd64/libjavafx-font.so \
        lib/amd64/libjavafx-iio.so \
        lib/amd64/libjfxmedia.so \
        lib/amd64/libjfxwebkit.so \
        lib/amd64/libprism-es2.so

echo
echo "JRE size after removing optional files: $(du -sh $JAVA_HOME)"

exit $RC_SUCCESS
