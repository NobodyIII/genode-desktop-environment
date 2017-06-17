#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H

#include <QPlainTextEdit>

#include "linenumberarea.h"

class TextArea : public QPlainTextEdit
{
    Q_OBJECT
    friend class LineNumberArea;

public:
    explicit TextArea(QWidget *parent = 0);

signals:

public slots:

private:
    void resizeEvent(QResizeEvent *event);

    LineNumberArea *lineNumArea;
};

#endif // TEXTWIDGET_H
