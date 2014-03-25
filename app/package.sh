#!/bin/sh
cd ../build
macdeployqt fstl.app
cd fstl.app/Contents/PlugIns
rm -rf accessible audio imageformats mediaservice playlistformats position printsupport qml1tooling sensorgestures sensors
cd ../Frameworks
rm -rf QtDeclarative.framework QtMultimedia.framework QtMultimediaWidgets.framework QtNetwork.framework QtPositioning.framework QtQml.framework QtQuick.framework QtScript.framework QtSensors.framework QtSql.framework QtXmlPatterns.framework
cd ../Resources
rm empty.lproj
cd ../../..
cp -r fstl.app ..
cd ..
zip -r fstl_mac.zip fstl.app README.md

