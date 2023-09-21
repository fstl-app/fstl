#!/bin/bash

# For a package installation (rpm or deb), we must proceed a different way
# This script takes two arguments, the first one is the installation
# prefix and the second is the name

if [ $# != 2 ]; then
  echo "You must provide two arguments"
  exit 1
fi

base=$1
name=$2

# echo "Drop mimetypes file in /usr/share/mime/packages/"
# mkdir -p $base/usr/share/mime/packages/
# cp fstlapp-$name-mimetypes.xml $base/usr/share/mime/packages/

echo "Drop desktop file in /usr/share/applications/"
mkdir -p $base/usr/share/applications/
cp fstlapp-$name.desktop $base/usr/share/applications/

slist="16 22 32 48 64 128 256"
echo "Installing apps icons"
iclist="fstlapp-$name"
for im in $iclist
do
  for s in $slist
  do
    mkdir -p $base/usr/share/icons/hicolor/${s}x${s}/apps
    cp icons/${im}_${s}x${s}.png $base/usr/share/icons/hicolor/${s}x${s}/apps/$im.png
  done
done

# echo "Installing mimetypes icons"
# iclist="`cat fstlapp-$name-mimetypes.xml | grep "icon name" | sed 's/^.*"\(.*\)".*$/\1/'`"
# for im in $iclist
# do
#   for s in $slist
#   do
#     mkdir -p $base/usr/share/icons/hicolor/${s}x${s}/mimetypes
#     cp icons/${im}_${s}x${s}.png $base/usr/share/icons/hicolor/${s}x${s}/mimetypes/$im.png
#   done
# done

#
# Put this in the post installation and post uninstallation scripts
#
#echo "Updating mime database"
#update-mime-database /usr/share/mim
#
#echo "Updating desktop database"
#update-desktop-database
