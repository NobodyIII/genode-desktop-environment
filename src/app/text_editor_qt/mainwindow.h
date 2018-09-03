#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

class TextArea;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool isModified();

private:
    Ui::MainWindow *ui;
    TextArea *mainEditor;
    bool modified = false;
    QString filename;

    bool _save();

    void closeEvent(QCloseEvent *event);

    static QList<MainWindow *> _windows;
    static void _closeAll();

public slots:
    bool saveFile();
    bool saveFileAs();
    void openFile();
    void newWindow();
    void closeAll();
    void updateTitle();
    void setModified(bool modified);
};

#endif // MAINWINDOW_H
