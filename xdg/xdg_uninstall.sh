#!/bin/bash

# This script will uninstall mimetypes, icons and desktop file
#
# if runned as regular user this will uninstall locally from :
#    $HOME/.local/share/mime/
#    $HOME/.local/share/applications/
#    $HOME/.local/share/icons/
#
# if runned as root this will uninstall system-wide from :
#    /usr/share/mime
#    /usr/share/applications
#    /usr/share/icons

if [ $# != 1 ]; then
  echo "You must provide a name"
  exit 1
fi

name=$1

# echo "Uninstalling mimetypes"
# xdg-mime uninstall fstlapp-$name-mimetypes.xml

echo "Uninstalling desktop file"
xdg-desktop-menu uninstall fstlapp-$name.desktop

echo "Uninstalling apps icons"
iclist="fstlapp-$name"
for im in $iclist
do
  xdg-icon-resource uninstall --theme hicolor --context apps --size 16 $im
  xdg-icon-resource uninstall --theme hicolor --context apps --size 22 $im
  xdg-icon-resource uninstall --theme hicolor --context apps --size 32 $im
  xdg-icon-resource uninstall --theme hicolor --context apps --size 48 $im
  xdg-icon-resource uninstall --theme hicolor --context apps --size 64 $im
  xdg-icon-resource uninstall --theme hicolor --context apps --size 128 $im
  xdg-icon-resource uninstall --theme hicolor --context apps --size 256 $im
done

# echo "Uninstalling mimetypes icons"
# iclist="`cat fstlapp-$name-mimetypes.xml | grep "icon name" | sed 's/^.*"\(.*\)".*$/\1/'`"
# for im in $iclist
# do
#   xdg-icon-resource uninstall --theme hicolor --context mimetypes --size 16 $im
#   xdg-icon-resource uninstall --theme hicolor --context mimetypes --size 22 $im
#   xdg-icon-resource uninstall --theme hicolor --context mimetypes --size 32 $im
#   xdg-icon-resource uninstall --theme hicolor --context mimetypes --size 48 $im
#   xdg-icon-resource uninstall --theme hicolor --context mimetypes --size 64 $im
# done
