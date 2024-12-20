// testwidget.cpp
#include "testwidget.h"

TestWidget::TestWidget(QWidget *parent)
    : QWidget(parent)
{
    connect(this, &TestWidget::mySignal, this, &TestWidget::onMySignal);
}

void TestWidget::onMySignal()
{
    qDebug() << "Signal received!";
}
