#!/bin/sh
set -x -e

cd ../build
make clean
rm -rf fstl.app
make -j8

APP=fstl

# Pull out framework paths info with otool
MACDEPLOYQT=`otool -L $APP.app/Contents/MacOS/fstl | sed -n -e "s:\(.*\)lib/QtCore.*:\1/bin/macdeployqt:gp"`

$MACDEPLOYQT $APP.app

# Delete unused Qt plugins
cd fstl.app/Contents/PlugIns
rm -rf accessible audio imageformats mediaservice playlistformats position printsupport qml1tooling sensorgestures sensors

fix_qt () {
    echo "Fixing Qt for $1"
    for LIB in $( otool -L $1 | sed -n -e "s:\(.*Qt.*.framework[^ ]*\).*:\1:gp" )
    do
        RENAMED=`echo $LIB | sed -n -e "s:.*\(Qt.*\)\.framework.*:@executable_path/../Frameworks/\1.framework/Versions/5/\1:gp"`
        install_name_tool -change $LIB $RENAMED $1
    done
}

# Remap platform links
cd platforms
fix_qt libqcocoa.dylib

# Delete unused Qt frameworks
cd ../../Frameworks
rm -rf QtDeclarative.framework QtMultimedia.framework QtMultimediaWidgets.framework QtNetwork.framework QtPositioning.framework QtQml.framework QtQuick.framework QtScript.framework QtSensors.framework QtSql.framework QtXmlPatterns.framework Qt3DCore.framework Qt3DRender.framework QtLocation.framework QtSerialBus.framework QtSerialPort.framework

# Clean up remaining Qt frameworks
for LIB in $( ls|sed -n -e "s:\(Qt.*\)\.framework:\1:gp" )
do
    fix_qt $LIB.framework/Versions/Current/$LIB
done

cd ../Resources
rm empty.lproj

# Create a disk image
cd ../../..
mkdir $APP
cp ../README.md ./$APP/README.txt
cp -R $APP.app ./$APP
hdiutil create $APP.dmg -volname "$APP" -srcfolder $APP
rm -rf $APP
mv $APP.dmg ..
