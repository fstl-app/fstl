#!/bin/bash

# This script will install mimetypes, icons and desktop file, 
# it takes a name in argument
#
# if runned as regular user this will install locally in :
#    $HOME/.local/share/mime/
#    $HOME/.local/share/applications/
#    $HOME/.local/share/icons/
#
# if runned as root this will install system-wide in :
#    /usr/share/mime
#    /usr/share/applications
#    /usr/share/icons

if [ $# != 1 ]; then
  echo "You must provide an application name"
  exit 1
fi

name=$1

# echo "Installing mimetypes"
# xdg-mime install fstlapp-$name-mimetypes.xml

echo "Installing desktop file"
xdg-desktop-menu install fstlapp-$name.desktop

echo "Installing apps icons"
iclist="fstlapp-$name"
for im in $iclist
do
  xdg-icon-resource install --theme hicolor --context apps --size 16 icons/${im}_16x16.png $im
  xdg-icon-resource install --theme hicolor --context apps --size 22 icons/${im}_22x22.png $im
  xdg-icon-resource install --theme hicolor --context apps --size 32 icons/${im}_32x32.png $im
  xdg-icon-resource install --theme hicolor --context apps --size 48 icons/${im}_48x48.png $im
  xdg-icon-resource install --theme hicolor --context apps --size 64 icons/${im}_64x64.png $im
  xdg-icon-resource install --theme hicolor --context apps --size 128 icons/${im}_128x128.png $im
  xdg-icon-resource install --theme hicolor --context apps --size 256 icons/${im}_256x256.png $im
done

# echo "Installing mimetypes icons"
# iclist="`cat fstlapp-$name-mimetypes.xml | grep "icon name" | sed 's/^.*"\(.*\)".*$/\1/'`"
# for im in $iclist
# do
#   xdg-icon-resource install --theme hicolor --context mimetypes --size 16 icons/${im}_16x16.png $im
#   xdg-icon-resource install --theme hicolor --context mimetypes --size 22 icons/${im}_22x22.png $im
#   xdg-icon-resource install --theme hicolor --context mimetypes --size 32 icons/${im}_32x32.png $im
#   xdg-icon-resource install --theme hicolor --context mimetypes --size 48 icons/${im}_48x48.png $im
#   xdg-icon-resource install --theme hicolor --context mimetypes --size 64 icons/${im}_64x64.png $im
# done

