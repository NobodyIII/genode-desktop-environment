#ifndef BGDE_FILE_REQUEST_HELPER_QT_H
#define BGDE_FILE_REQUEST_HELPER_QT_H

#include <QStringList>
#include <BGDE/file_request.h>

namespace BGDE_Qt
{
	const int FILE_REQUEST_BUF_SIZE = 1024;
	
	QStringList makeFileRequest(QString request)
	{
		char response[FILE_REQUEST_BUF_SIZE];
		bgde_file_request(request.toStdString().c_str(), response, sizeof(response));
		QString response_qstr(response);
		QStringList filenames = response_qstr.split('\n', QString::SkipEmptyParts);
		return filenames;
	}
	
	QString openFile(QString filter = "", QString default_name="")
	{
		QStringList result = makeFileRequest(QString("of filter:\"%s\" default_name:\"%s\"").arg(filter, default_name));
		
		if (result.size() == 0)
			return "";
		else
			return result[0];
	}
	
	QString saveFile(QString filter = "", QString default_name="")
	{
		QStringList result = makeFileRequest(QString("sf filter:\"%s\" default_name:\"%s\"").arg(filter, default_name));
		
		if (result.size() == 0)
			return "";
		else
			return result[0];
	}
}

#endif
