Linux :
-----------
desktop file and application icons installation.
This tells the system that fstl knows to open stl files and allow stl to
be launched using windows key.

Install :
./xdg_install.sh fstl

Uninstall :
./xdg_uninstall.sh fstl

if runned as regular user this will install locally in :
   $HOME/.local/share/mime/
   $HOME/.local/share/applications/
   $HOME/.local/share/icons/

if runned as root this will install system-wide in :
   /usr/share/mime
   /usr/share/applications
   /usr/share/icons

Third script xdg_package_install.sh is to be used when building deb or rpm package.
