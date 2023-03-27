#ifndef QWIDGETWITHCLICK_H
#define QWIDGETWITHCLICK_H

#include <QWidget>
#include <QDebug>

class QWidgetWithClick : public QWidget {

  Q_OBJECT

  public:
    QWidgetWithClick(QWidget *parent) : QWidget(parent) {};
    QWidgetWithClick() : QWidget() {};

  private:
    void mousePressEvent(QMouseEvent *event) override {
      emit clicked();
    }

    void paintEvent(QPaintEvent * e) override
    {
      QStyleOption opt;
      opt.init(this);
      QPainter p(this);
      style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

    void resizeEvent(QResizeEvent* event) override {
      emit resized();
    }

  signals:
    void clicked();
    void resized();
};

#endif