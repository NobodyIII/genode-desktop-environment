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
    enum OpenFlags {
        DEFAULT = 0x00,
        READ_ONLY_OPTION_DISABLED = 0x01, /* read-only checkbox is disabled */
        READ_ONLY_OPTION_SET = 0x02, /* read-only checkbox is checked by default */

        DIRECTORY = 0x04, /* to be used in conjunction with another mode */
        EXISTING_SINGLE = 0x08,
        EXISTING_MULTI = 0x10,

        /* combinations of flags */
        READ_ONLY = 0x06, /* read-only checkbox is set and disabled */
        WRITE_REQUIRED = 0x02, /* read-only checkbox is unset and disabled */
    };
    explicit FileDialog(QString title, OpenFlags mode, QString filter);

signals:

public slots:

private:
    QCheckBox *readOnlyBox;
};

#endif // FILEDIALOG_H
