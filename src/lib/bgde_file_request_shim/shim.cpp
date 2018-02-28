#include "../../app/file_dialog/filedialog.h"

extern "C" void bgde_file_request(const char *request, char *response_buf, int buf_size)
{
    FileDialog dialog(request);
    QStringList files;
	
	/* show dialog */
	if (dialog.exec()) {
		/* success */
        files = dialog.selectedFiles();
	}
	
    QString response_qstr;

    for (QString file : files) {
        response_qstr += file + "\n";
    }

	/* copy result to C string */
    strncpy(response_buf, response_qstr.toStdString().c_str(), buf_size);
}
