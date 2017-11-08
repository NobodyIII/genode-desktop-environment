#include "filedialog.h"
FileDialog::FileDialog(QString title, OpenFlags flags, QString filter):
    QFileDialog(0)
{
    setOption(DontUseNativeDialog);
    readOnlyBox = new QCheckBox("Read-only");

    if (flags & READ_ONLY_OPTION_SET)
        readOnlyBox->setChecked(true);
    if (flags & READ_ONLY_OPTION_DISABLED)
        readOnlyBox->setDisabled(true);

    if (flags & DIRECTORY)
        setFileMode(Directory);
    else if (flags & EXISTING_MULTI)
        setFileMode(ExistingFiles);
    else if (flags & EXISTING_SINGLE)
        setFileMode(ExistingFile);

    QGridLayout *grid = dynamic_cast<QGridLayout*>(layout());
    if (grid)
    {
       int new_row = grid->rowCount();
       grid->addWidget(readOnlyBox, new_row, 0, 1, grid->columnCount(), Qt::AlignRight);
       grid->setRowStretch(new_row, 0);
    } else {
        qWarning() << "unable to add read-only checkbox to layout";
    }
}
