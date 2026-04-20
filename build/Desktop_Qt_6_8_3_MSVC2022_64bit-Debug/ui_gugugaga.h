/********************************************************************************
** Form generated from reading UI file 'gugugaga.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUGUGAGA_H
#define UI_GUGUGAGA_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_gugugaga
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *gugugaga)
    {
        if (gugugaga->objectName().isEmpty())
            gugugaga->setObjectName("gugugaga");
        gugugaga->resize(800, 600);
        centralwidget = new QWidget(gugugaga);
        centralwidget->setObjectName("centralwidget");
        gugugaga->setCentralWidget(centralwidget);
        menubar = new QMenuBar(gugugaga);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 25));
        gugugaga->setMenuBar(menubar);
        statusbar = new QStatusBar(gugugaga);
        statusbar->setObjectName("statusbar");
        gugugaga->setStatusBar(statusbar);

        retranslateUi(gugugaga);

        QMetaObject::connectSlotsByName(gugugaga);
    } // setupUi

    void retranslateUi(QMainWindow *gugugaga)
    {
        gugugaga->setWindowTitle(QCoreApplication::translate("gugugaga", "gugugaga", nullptr));
    } // retranslateUi

};

namespace Ui {
    class gugugaga: public Ui_gugugaga {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUGUGAGA_H
