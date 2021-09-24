#include "progressBar.h"
#include <QPainter>

ProgressBar::ProgressBar(QWidget* parent) : QWidget(parent)
{
    setMinimumHeight(15);
    setMaximumHeight(50);
}

void ProgressBar::setLevel(qreal level)
{
    if (m_level != level) {
        m_level = level;
        update();
    }
}

void ProgressBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    // draw level
    qreal widthLevel = m_level * width();
    painter.fillRect(0, 0, static_cast<int>(widthLevel), height(), Qt::green);
    // clear the rest of the control
    painter.fillRect(static_cast<int>(widthLevel), 0, width(), height(), Qt::gray);
}
