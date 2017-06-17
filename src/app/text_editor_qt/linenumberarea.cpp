#include <QPainter>
#include <QTextBlock>

#include "linenumberarea.h"
#include "textarea.h"

LineNumberArea::LineNumberArea(TextArea *parent) :
    QWidget(parent)
{
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    setContentsMargins(2, 2, 2, 2);
    textWidget = parent;
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::black);

    for (QTextBlock block = textWidget->firstVisibleBlock(); (block.isValid() && block.isVisible()); block = block.next()) {
        QRectF bounds = textWidget->blockBoundingGeometry(block);
        bounds.translate(textWidget->contentOffset());
        painter.drawText(contentsRect().left(), bounds.top(), contentsRect().width(), fontMetrics().height(), Qt::AlignRight, QString::number(block.blockNumber()));
    }
}
