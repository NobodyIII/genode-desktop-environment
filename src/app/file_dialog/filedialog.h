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
    void updateSuffix();

protected:
    void accept() override;

private:
    DialogMode _mode;
    QCheckBox *_readOnlyBox;
    QCheckBox *_autoExtenBox;

    void _parseRequest(const QString request);

    bool _verifyFileCreate(QFileInfo info);
    bool _verifyFileExists(QFileInfo info, bool is_dir);
    bool _verifyFileWritable(QFileInfo info, bool is_dir, bool allow_create = false);
};

#endif // FILEDIALOG_H
