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

    _parseRequest(request);

    connect(this,SIGNAL(filterSelected(QString)), this, SLOT(updateSuffix()));
    connect(_autoExtenBox, SIGNAL(toggled(bool)), this, SLOT(updateSuffix()));

    /* initialize to adding extension, triggering updateSuffix() in the process */
    _autoExtenBox->setChecked(true);

    switch (_mode) {
    case Create_File:
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
        qWarning() << "unable to add auto-extension and read-only checkboxes to layout";
    }
}

void FileDialog::accept()
{

    QStringList chosen_files = selectedFiles();
    QString first_file = chosen_files.first();

    QFileInfo first_info(first_file);

    /* handle basic cases common to the different modes */
    switch(fileMode()) {
    case AnyFile:
    case ExistingFile:
    case ExistingFiles:
        if (first_info.isDir()) {
            /* enter directory rather than selecting it */
            setDirectory(first_info.absoluteFilePath());
            return;
        }
        break;

    case Directory:
    case DirectoryOnly:
        if (!first_info.isDir()) {
            /* don't allow user to select files when asked for a directory */
            return;
        }
    }

    for (QString filename : chosen_files) {
        QFileInfo info(filename);

        switch (_mode) {
        case Create_File:
            if (!_verifyFileCreate(info))
                return;
            break;

        case Create_or_Modify_File:
            if (!_verifyFileWritable(info, false, true))
                return;
            break;

        case Modify_File:
        case Modify_Files:
            if (!_verifyFileWritable(info, false))
                return;
            break;
        break;

        case Open_File:
        case Open_Files:
            /* at least for now, don't warn about opening read-only files with the read-only box unchecked */
            if (!_verifyFileExists(info, false))
                return;
            break;

        case Read_File:
        case Read_Files:
            if (!_verifyFileExists(info, false))
                return;
            break;

        case Modify_Directory:
            if (!_verifyFileWritable(info, true))
                return;
            break;

        case Open_Directory:
            /* at least for now, don't warn about opening read-only files with the read-only box unchecked */
            if (!_verifyFileExists(info, true))
                return;
            break;

        case Read_Directory:
            if (!_verifyFileExists(info, false))
                return;
            break;
        }
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
    name_filters << "PNG Images (*.png)" << "JPEG Images (*.jpg *.jpeg)" << "All Files (*)";
    setNameFilters(name_filters);
}

bool FileDialog::_verifyFileExists(QFileInfo info, bool is_dir)
{
    if (!info.exists()) {
        /* fail if file doesn't exist */
        QString type = is_dir? "Directory" : "File";
        QMessageBox::warning(this, windowTitle(), QString("%1 \"%2\" does not exist.").arg(type, info.fileName()));
        return false;
    }
    return true;
}

bool FileDialog::_verifyFileCreate(QFileInfo info)
{
    /* check and warn if file already exists */
    if (info.exists()) {
        QMessageBox::StandardButton ask_result = QMessageBox::question(this, windowTitle(), QString("File \"%1\" already exists. Do you want to overwrite it?").arg(info.fileName()));
        if (ask_result == QMessageBox::No)
            return false;
    }

    QFile file(info.absoluteFilePath());
    /* try to create file */
    if (file.open(QFile::WriteOnly)) {
        /* successfully created file; now close it */
        file.close();
        return true;
    } else {
        /* warn if unable to create file */
        QMessageBox::warning(this, windowTitle(), QString("Cannot create file \"%1\"").arg(info.fileName()));
        return false;
    }
}

bool FileDialog::_verifyFileWritable(QFileInfo info, bool is_dir, bool allow_create)
{
    /* ensure file exists and is writable */

    /* ensure we weren't given bad arguments (this is not the place to be creating a directory) */
    Q_ASSERT(!(is_dir && allow_create));

    /* try creating file if it doesn't exist */
    if (allow_create) {
        if (!info.exists())
            return _verifyFileCreate(info);
    }

    /* we aren't creating a file, so fail if it doesn't exist */
    _verifyFileExists(info, is_dir);

    /* fail if file isn't writable */
    if (!info.isWritable()) {
        QString type = is_dir? "Directory" : "File";
        QMessageBox::warning(this, windowTitle(), QString("%1 \"%2\" isn't writable.").arg(type, info.fileName()));
        return false;
    }

    /* file passed all checks, so return true */
    return true;
}

void FileDialog::updateSuffix()
{
    /* clear previous value */
    setDefaultSuffix("");

    /* don't append extension (suffix) if the user deselected that option */
    if (!_autoExtenBox->isChecked())
        return;

    QString filter_str = selectedNameFilter();

    /* select the wildcard part of the filter string */
    /* QString safely and quietly handles bad values */
    filter_str = filter_str.mid(filter_str.indexOf('(') + 1);
    filter_str.truncate(filter_str.lastIndexOf(')'));

    /* convert filter string to file extension list */
    QStringList filters = filter_str.split(' ', QString::SkipEmptyParts);

    if (filters.size() == 0)
        return; /* no valid filter */

    QString first_filter = filters.first();

    if (first_filter.at(0) != '*') {
        qWarning() << "invalid filter: " << first_filter;
        return;
    }

    /* remove leading asterisk */
    setDefaultSuffix(first_filter.mid(1));
    qDebug() << "changed suffix to \"" << defaultSuffix() << "\"";
}
