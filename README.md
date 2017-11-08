## This is my work on a desktop environment for Genode. Most of the components haven't been written yet, but some are written and functional.

### Status key:
[x] = The component is usable and available in the master branch on Github.  
[w] = I have started working on the component; it may or may not be available on Github.  

### Here is my vision of its future core components:
[x] file_manager_qt - simple file manager  
file_dialog - used for opening/saving files; handles requests from fs_filter  
[w] fs_filter - handles filesystem requests; communicates with file_dialog  
[x] text_editor - plain text editor; uses file_dialog for open/save; maybe provides syntax highlighting  
[x] archive_manager - basic archive manager capable of displaying and extracting archive contents, as well as creating archives and adding and deleting files; uses file_dialog for open/extract  
login_gui - allows users to login; ideally also can lock screen  
panel - provides application menu, task manager, and tray for various applets  
launcher - securely starts applications (by generating the init config rom) and handles service routing and policies

### Desirable future components:
desktop - provides wallpaper and application icons  
installer - used to securely install new applications, giving them private directories and default policies as determined by user  
wifi_applet - simple panel applet to interface with wifi_drv  
notification_applet - provides notification service for applications  
calculator - basic calculator  
mount handler - securely provides filesystems and filesystem list to file_manager  

Please feel free to contact me if you want to contribute in any way. Ideas, bug reports, and additional code are welcome.

##### NOTES:  
1. The file manager and text editor are incomplete, but should be functional.
