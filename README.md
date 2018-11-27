# This repository is dead. Anything reasonably stable will be moved to BGG or BGDE.

## I started this before I really knew how to use Git properly. I made several branches without a good naming scheme, and I made this repository into a bit of a mess. I intended to rename the project anyway, so I decided to start fresh rather than trying to clean up this repository.

## ------------------------------------

## This is my work on a desktop environment for Genode. Most of the components haven't been written yet, but some are written and functional.

### Status key:
[x] = The component is usable and available in the master branch on Github.  
[w] = I have started working on the component; it may or may not be available on Github.  

### Here is my vision of its future core components:
[x] file_manager - simple file manager  
[w] file_dialog - used for opening/saving files; handles requests from fs_filter  
[w] fs_filter - handles filesystem requests; communicates with file_dialog  
[x] text_editor - plain text editor; uses file_dialog for open/save; maybe provides syntax highlighting  
[x] archive_manager - basic archive manager capable of displaying and extracting archive contents, as well as creating archives and adding and deleting files; uses file_dialog for open/extract  
[ ] login_gui - allows users to login; ideally also can lock screen  
[ ] panel - provides application menu, task manager, and tray for various applets  
[ ] launcher - starts applications (by generating the init config rom) and handles service routing and policies  
[ ] file_ident - identifies files based on file extension  
[ ] icon_provider - provides icons from icon theme as well as icons for installed applications and file types  

### Planned core libraries:
[ ] file_request - opens files (as selected by the user); makes requests to fs_filter  
[ ] file_ident - identifies files; makes requests to file_ident (and possibly file_ident_magic)  
[ ] icon - accesses icons; makes requests to icon_provider  
[ ] launch_extern - launches external applications; can open files with the appropriate applications; makes requests to launcher  

### Additional planned libraries:
[ ] notify - provides notifications to the user; makes requests to notification_applet

### Desirable future components:
[ ] desktop - provides wallpaper and application icons  
[ ] installer - used to securely install new applications, giving them private directories and default policies as determined by user  
[ ] wifi_applet - simple panel applet to interface with wifi_drv  
[ ] notification_applet - provides notification service for applications  
[ ] calculator - basic calculator  
[ ] mount_handler - provides filesystems and filesystem list to file_manager  
[ ] file_ident_magic - uses libmagic to determine file types


Please feel free to contact me if you want to contribute in any way. Ideas, bug reports, and additional code are welcome.

##### NOTES:  
1. The file manager, text editor, and archive manager are incomplete, but should provide the core functionality needed for most everyday uses.
2. Everything here is designed with security in mind. However, functionality will come before security (chrononlogically). For the time being, you should not expect any of these components or libraries to be more secure than their counterparts in other desktop environments.
