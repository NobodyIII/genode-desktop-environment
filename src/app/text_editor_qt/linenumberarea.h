#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QWidget>

class TextArea;

class LineNumberArea : public QWidget
{
    Q_OBJECT
private:
    TextArea *textWidget;

public:
    explicit LineNumberArea(TextArea *parent);

    QSize sizeHint() const override {
        return QSize(5 * fontMetrics().width('9'), 0);
    }

    void paintEvent(QPaintEvent *event) override;


signals:

public slots:

};

#endif // LINENUMBERAREA_H
