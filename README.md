# This is my work on a desktop environment for Genode. Currently, the only component is a graphical file manager, which mostly works fairly well.

## Here is my vision of its future core components:
file_manager - used as a file manager and a file dialog provider; runs as a semi-privileged service  
service_router - handles requests for various Genode services; uses per-application policies; provides each application with its own private filesystem, attaching files selected in file dialogs  
text_editor - plain text editor; uses file_manager open/save dialogs; maybe provides syntax highlighting  
archive_manager - basic component; uses file_manager open/save dialogs  
login_gui - allows users to login; ideally also can lock screen  
panel - provides application menu, task manager, and tray for various applets  

## Desirable future components:
desktop - provides wallpaper and application icons  
installer - used to securely install new applications, giving them private directories and default policies as determined by user  
wifi_applet - simple panel applet to interface with wifi_drv  
notification_applet - provides notification service for applications  
calculator - basic calculator
mount handler - securely provides filesystems and filesystem list to file_manager  

Please feel free to contact me if you want to contribute in any way. Ideas, bug reports, and additional code are welcome.

NOTES:  
1. The file manager is designed to build on Linux with the TEST_LINUX flag, to make GUI development easier.  
2. The file manager has some GUI issues on Genode that don't appear on Linux, so they're probably due to deficiencies in Genode's Qt5 port.
