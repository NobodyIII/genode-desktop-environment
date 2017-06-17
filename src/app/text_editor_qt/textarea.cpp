#include "textarea.h"

TextArea::TextArea(QWidget *parent) :
    QPlainTextEdit(parent)
{
    lineNumArea = new LineNumberArea(this);
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    connect(this, SIGNAL(blockCountChanged(int)), lineNumArea, SLOT(update()));
    connect(this, SIGNAL(updateRequest(QRect,int)), lineNumArea, SLOT(update()));
}

void TextArea::resizeEvent(QResizeEvent *event)
{
    QRect cr = contentsRect();
    lineNumArea->setGeometry(QRect(cr.left(), cr.top(), lineNumArea->sizeHint().width(), cr.height()));
    setViewportMargins(lineNumArea->sizeHint().width(), 0, 0, 0);
    lineNumArea->update();
}
