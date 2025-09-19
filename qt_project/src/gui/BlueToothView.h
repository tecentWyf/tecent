/****************************************Copyright (c)****************************************************
**
**                                       Giettek InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  BlueTooth.h
** Latest Version:             V1.0.1
** Latest modified Date:       2023/03/08
** Modified by:                Wang Jinge
** Descriptions:               添加注释信息
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Wang Jinge
** Created date:               2022/11/10
** Descriptions:               BlueTooth列表类
**
*********************************************************************************************************/
#include "BlueToothdelegate.h"
#include <QTableView>
#include <QAbstractItemModel>
#include <QHeaderView>
#include <QAbstractScrollArea>
#include <QAbstractItemView>
#include <QFile>
#include <QString>
#include <QScrollBar>
#include <QScroller>
#include <QMouseEvent>
#include <QEasingCurve>

class BlueToothView : public QTableView
{
	Q_OBJECT

	Q_PROPERTY(QAbstractItemModel *blueToothModel   WRITE setModel   READ model)
public:
    explicit BlueToothView(QWidget *parent = 0)
		:QTableView(parent)
	{
        setShowGrid(false);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        setSelectionBehavior(QAbstractItemView::SelectRows);
        //setSelectionMode(QAbstractItemView::SingleSelection);
        horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive); // 允许手动调整大小

        int totalWidth = width(); // 获取视图端口的宽度

        int column1Width = totalWidth / 8 * 6;
        int column2Width = totalWidth / 8;
        int column3Width = totalWidth / 8;

        setColumnWidth(0, column1Width);
        setColumnWidth(1, column2Width);
        setColumnWidth(2, column3Width);

        horizontalHeader()->setVisible(false);
        verticalHeader()->setVisible(false);
        setItemDelegate(new BlueToothDelegate);
        verticalHeader()->setDefaultSectionSize(31);
        setSelectionMode(QAbstractItemView::SingleSelection);

        
        QString stylePath = "./styles/wifiStyle.css";
        setStyle(stylePath);

        setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        QScroller* scroller = QScroller::scroller(viewport());
        scroller->grabGesture(viewport(), QScroller::TouchGesture);
        QScrollerProperties prop;
        prop.setScrollMetric(QScrollerProperties::DecelerationFactor, 0.35f);
        prop.setScrollMetric(QScrollerProperties::MaximumVelocity, 0.5f);
        prop.setScrollMetric(QScrollerProperties::ScrollingCurve, QVariant::fromValue<QEasingCurve>(QEasingCurve::OutCubic));
        scroller->setScrollerProperties(prop);
	}

    void setStyle(const QString &style)
    {
        QFile qss(style);
        bool ret = qss.open(QFile::ReadOnly);
        if (ret)
        {
            this->setStyleSheet(qss.readAll());
            qss.close();
        }
    }

//    void paintEvent(QPaintEvent * event)
//    {
//        for (int i = 0; i < model()->rowCount(); i++)
//        {
//            if ((model()->data(model()->index(i, 0)).isNull()) || (model()->data(model()->index(i, 1)).isNull()))
//            {
//                setRowHidden(i, true);
//            }
//            else
//            {
//                QTableView::paintEvent(event);
//            }
//        }
//    }

    Q_SLOT void refreshDisplay()
    {
        horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive); // 允许手动调整大小

        int totalWidth = width(); // 获取视图端口的宽度

        int column1Width = totalWidth / 8 * 6;
        int column2Width = totalWidth / 8;
        int column3Width = totalWidth / 8;

        setColumnWidth(0, column1Width);
        setColumnWidth(1, column2Width);
        setColumnWidth(2, column3Width);
    }
protected:
    void mouseMoveEvent(QMouseEvent *e)
    {
        clearSelection();
        Q_UNUSED(e)
    }
};
/*********************************************************************************************************
** End of BlueTooth.h
*********************************************************************************************************/