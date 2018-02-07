#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QFileDialog>
#include <QCheckBox>
#include <QGridLayout>
#include <QDebug>

class FileDialog : public QFileDialog
{
    Q_OBJECT
public:
    /* "Open" is used to indicate that the file will be read, and can optionally be written to */
    enum DialogMode {
        Create_File,
        Create_or_Modify_File,
        Modify_File,
        Open_File,
        Read_File,
        Modify_Files,
        Open_Files,
        Read_Files,
        Modify_Directory,
        Open_Directory,
        Read_Directory,
    };
    explicit FileDialog(QString request);

signals:

public slots:
//    void _handleFilterSelect(QString filter);

protected:
    void accept() override;

private:
    DialogMode _mode;
    QCheckBox *_readOnlyBox;
    QCheckBox *_autoExtenBox;

    void _parseRequest(const QString request);
    QString _addFileExten(const QString file_path);
};

#endif // FILEDIALOG_H
