## This is my work on a desktop environment for Genode. Currently, the only component is a graphical file manager, which mostly works fairly well.

### Here is my vision of its future core components:
file_manager_qt - simple file manager  
file_dialog - used for opening/saving files; handles requests from fs_filter  
fs_filter - handles filesystem requests; communicates with file_dialog  
text_editor - plain text editor; uses file_dialog for open/save; maybe provides syntax highlighting  
archive_manager - basic component; uses file_dialog for open/extract  
login_gui - allows users to login; ideally also can lock screen  
panel - provides application menu, task manager, and tray for various applets  

### Desirable future components:
desktop - provides wallpaper and application icons  
installer - used to securely install new applications, giving them private directories and default policies as determined by user  
wifi_applet - simple panel applet to interface with wifi_drv  
notification_applet - provides notification service for applications  
calculator - basic calculator  
mount handler - securely provides filesystems and filesystem list to file_manager  

Please feel free to contact me if you want to contribute in any way. Ideas, bug reports, and additional code are welcome.

##### NOTES:  
1. The file manager is incomplete, but should be functional
