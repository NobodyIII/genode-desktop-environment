#include <QMessageBox>

#include "filedialog.h"

/* special characters in requests:
 * newline (\n)
 * double-quotes (")
 */

/* request format examples:
 * cs filter_name:"PNG Images (*.png)|JPEG Images (*.jpg *.jpeg)"
 */

FileDialog::FileDialog(QString request):
    QFileDialog(0)
{
    setOption(DontUseNativeDialog);
    _readOnlyBox = new QCheckBox("Read-only");
    _autoExtenBox = new QCheckBox("Auto file extension");

//    connect(this, SIGNAL(filterSelected(QString)), this, SLOT(_handleFilterSelect(QString)));
    _parseRequest(request);

    _autoExtenBox->setChecked(true);

    switch (_mode) {
    case Create_File:
        //TODO: handle distinction between create and create_or_modify
        _readOnlyBox->setChecked(false);
        _readOnlyBox->setDisabled(true);
        _autoExtenBox->setVisible(true);
        setFileMode(AnyFile);
        setAcceptMode(AcceptSave);
        setWindowTitle("Create File");
        break;

    case Create_or_Modify_File:
        _readOnlyBox->setChecked(false);
        _readOnlyBox->setDisabled(true);
        _autoExtenBox->setVisible(true);
        setFileMode(AnyFile);
        setAcceptMode(AcceptSave);
        setWindowTitle("Create/Modify File");
        break;

    case Modify_File:
        _readOnlyBox->setChecked(false);
        _readOnlyBox->setDisabled(true);
        _autoExtenBox->setVisible(false);
        setFileMode(ExistingFile);
        setAcceptMode(AcceptSave);
        setWindowTitle("Modify File");
        break;

    case Open_File:
        _readOnlyBox->setChecked(false);
        _readOnlyBox->setDisabled(false);
        _autoExtenBox->setVisible(false);
        setFileMode(ExistingFile);
        setAcceptMode(AcceptSave);
        setWindowTitle("Open File");
        break;

    case Read_File:
        _readOnlyBox->setChecked(true);
        _readOnlyBox->setDisabled(true);
        _autoExtenBox->setVisible(false);
        setFileMode(ExistingFile);
        setAcceptMode(AcceptOpen);
        setWindowTitle("Read File");
        break;

    case Modify_Files:
        _readOnlyBox->setChecked(false);
        _readOnlyBox->setDisabled(true);
        _autoExtenBox->setVisible(false);
        setFileMode(ExistingFiles);
        setAcceptMode(AcceptSave);
        setWindowTitle("Modify Files");
        break;

    case Open_Files:
        _readOnlyBox->setChecked(false);
        _readOnlyBox->setDisabled(false);
        _autoExtenBox->setVisible(false);
        setFileMode(ExistingFiles);
        setAcceptMode(AcceptSave);
        setWindowTitle("Open Files");
        break;

    case Read_Files:
        _readOnlyBox->setChecked(true);
        _readOnlyBox->setDisabled(true);
        _autoExtenBox->setVisible(false);
        setFileMode(ExistingFiles);
        setAcceptMode(AcceptOpen);
        setWindowTitle("Read Files");
        break;

    case Modify_Directory:
        _readOnlyBox->setChecked(false);
        _readOnlyBox->setDisabled(true);
        _autoExtenBox->setVisible(false);
        setFileMode(Directory);
        setAcceptMode(AcceptSave);
        setWindowTitle("Modify Directory");
        break;

    case Open_Directory:
        _readOnlyBox->setChecked(false);
        _readOnlyBox->setDisabled(false);
        _autoExtenBox->setVisible(false);
        setFileMode(Directory);
        setAcceptMode(AcceptSave);
        setWindowTitle("Open Directory");
        break;

    case Read_Directory:
        _readOnlyBox->setChecked(true);
        _readOnlyBox->setDisabled(true);
        _autoExtenBox->setVisible(false);
        setFileMode(Directory);
        setAcceptMode(AcceptOpen);
        setWindowTitle("Read Directory");
        break;

    }

    QGridLayout *grid = dynamic_cast<QGridLayout*>(layout());
    if (grid)
    {
       int new_row = grid->rowCount();
       grid->addWidget(_autoExtenBox, new_row, 0, 1, grid->columnCount(), Qt::AlignLeft);
       grid->addWidget(_readOnlyBox, new_row, 0, 2, grid->columnCount(), Qt::AlignRight);
       grid->setRowStretch(new_row, 0);
    } else {
        qWarning() << "unable to add read-only checkbox to layout";
    }
}

void FileDialog::accept()
{
    qDebug() << "default suffix: " << defaultSuffix();
    //TODO: fill stub
    const QStringList chosen_files = selectedFiles();
    QString chosen_file = chosen_files.first();

    /* add file extension if needed */
    if (_autoExtenBox->isChecked())
        chosen_file = _addFileExten(chosen_file);

    QFileInfo info(chosen_file);

    /* handle basic cases common to the different modes */
    switch(fileMode()) {
    case AnyFile:
    case ExistingFile:
    case ExistingFiles:
        if (info.isDir()) {
            /* enter directory rather than selecting it */
            setDirectory(info.absoluteFilePath());
            return;
        }
        break;

    case Directory:
    case DirectoryOnly:
        if (!info.isDir()) {
            /* don't allow user to select files when asked for a directory */
            return;
        }
    }

    QDir parent_dir = info.absoluteDir();
    QFile file(chosen_file);

    switch (_mode) {
    case Create_File:
        /* check and warn if file already exists */
        if (info.exists()) {
            QMessageBox::StandardButton ask_result = QMessageBox::question(this, windowTitle(), QString("File \"%1\" already exists. Do you want to overwrite it?").arg(chosen_file));
            if (ask_result == QMessageBox::No)
                return;
        }

        /* try to create file */
        if (file.open(QFile::WriteOnly)) {
            /* successfully created file; now close it */
            file.close();
        } else {
            /* warn if unable to create file */
            QMessageBox::warning(this, windowTitle(), QString("Cannot create file \"%1\"").arg(chosen_file));
            return;
        }
        break;

    case Create_or_Modify_File:
        /* try to create file if it doesn't exist */
        if (!info.exists()){
            if (file.open(QFile::WriteOnly)) {
                /* successfully created file; now close it */
                file.close();
            } else {
                /* warn if unable to create file */
                QMessageBox::warning(this, windowTitle(), QString("Cannot create file \"%1\"").arg(chosen_file));
                return;
            }
        } else if (!info.isWritable()) {
            /* fail if file isn't writable */
            QMessageBox::warning(this, windowTitle(), QString("File \"%1\" cannot be modified.").arg(chosen_file));
            return;
        }
        break;

    case Modify_File:
    if (!info.exists()) {
        /* fail if file doesn't exist */
        QMessageBox::warning(this, windowTitle(), QString("File \"%1\" does not exist.").arg(chosen_file));
        return;
    } else if (!info.isWritable()) {
        /* fail if file isn't writable */
        QMessageBox::warning(this, windowTitle(), QString("File \"%1\" cannot be modified.").arg(chosen_file));
        return;
    }
    break;

    case Open_File:
        if (!info.exists()) {
            /* fail if file doesn't exist */
            QMessageBox::warning(this, windowTitle(), QString("File \"%1\" does not exist.").arg(chosen_file));
            return;
        }
        /* at least for now, don't warn about opening read-only files with the read-only box unchecked */
        break;

    case Read_File:
        if (!info.exists()) {
            /* fail if file doesn't exist */
            QMessageBox::warning(this, windowTitle(), QString("File \"%1\" does not exist.").arg(chosen_file));
            return;
        }
        break;

    case Modify_Files:
    case Open_Files:
    case Read_Files:
    case Modify_Directory:
    case Open_Directory:
    case Read_Directory:
        break;
    }

    QDialog::accept();
//    QFileDialog::accept();
}

void FileDialog::_parseRequest(const QString request)
{
    //TODO: fill stub

    //FIXME: remove dummy code
    _mode = Create_File;
    QStringList name_filters;
    name_filters << "PNG Images (*.png)" << "JPEG Images (*.jpg *.jpeg)";
    setNameFilters(name_filters);
}

QString FileDialog::_addFileExten(const QString file_path)
{
    QString filter_str = selectedNameFilter();

    /* select the wildcard part of the filter string */
    filter_str = filter_str.mid(filter_str.indexOf('('));
    filter_str.truncate(filter_str.lastIndexOf(')'));

    /* convert filter string to file extension list */
    QStringList filters = filter_str.split(' ', QString::SkipEmptyParts);

    /* if there is an extensionless ("*") filter, don't add an extension */
    if (filters.contains("*"))
        return file_path;

    /* get everything after last slash */
    QString file_name = file_path.mid(file_path.lastIndexOf('/'));

    /* does the file already have an extension? */
    int has_exten = (file_name.lastIndexOf('.') == -1);
    if (has_exten)
        return file_path;

    /* now we add the first file extension from the list, as long as it's viable */
    QString filter = filters.first();
    int pos = filter.indexOf("*.");
    if (pos == -1)
        /* this isn't an extension filter; we only support extension filters, so just return the original path */
        return file_path;

    QString exten = filter.mid(pos);

    return file_path + exten;
}
