// testwidget.h
#ifndef TESTWIDGET_H
#define TESTWIDGET_H

#include <QMetaObject>
#include <QWidget>

class TestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TestWidget(QWidget *parent = nullptr);

signals:
    void mySignal();

public slots:
    void onMySignal();
};

#endif // TESTWIDGET_H
