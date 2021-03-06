/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#if defined(Q_WS_QWS)

//TESTED_CLASS=
//TESTED_FILES=

#include <qdesktopwidget.h>
#include <qwindowsystem_qws.h>
#include <qscreen_qws.h>
#include <qscreendriverfactory_qws.h>
#include <qwsdisplay_qws.h>

#ifdef QT_NO_QWS_TRANSFORMED
# undef QT_NO_QWS_TRANSFORMED
#endif

#include <qscreentransformed_qws.h>

class tst_QTransformedScreen : public QObject
{
    Q_OBJECT

public:
    tst_QTransformedScreen() {}
    ~tst_QTransformedScreen() { }

private slots:
    void initTestCase();
    void cleanupTestCase();
    void setTransformation_data();
    void setTransformation();
    void qwsDisplay_setTransformation();

private:
    QTransformedScreen *screen;
    QScreen *oldScreen;
    int id;
};

Q_DECLARE_METATYPE(QTransformedScreen::Transformation);

void tst_QTransformedScreen::initTestCase()
{
    oldScreen = qt_screen;

    QVERIFY(QScreenDriverFactory::keys().contains(QLatin1String("Transformed")));
    QVERIFY(QScreenDriverFactory::keys().contains(QLatin1String("VNC")));

    id = 10;
    screen = static_cast<QTransformedScreen*>(QScreenDriverFactory::create("Transformed", id));
    QVERIFY(screen);
    QVERIFY(screen->connect(QString("Transformed:Rot90:VNC:%1").arg(id)));
    QVERIFY(screen->initDevice());
}

void tst_QTransformedScreen::cleanupTestCase()
{
    screen->shutdownDevice();
    screen->disconnect();
    delete screen;
    screen = 0;

    qt_screen = oldScreen;
}

void tst_QTransformedScreen::setTransformation_data()
{
    QTest::addColumn<QTransformedScreen::Transformation>("transformation");
    QTest::addColumn<bool>("swap");

    QTest::newRow("Rot0") << QTransformedScreen::None << false;
    QTest::newRow("Rot90") << QTransformedScreen::Rot90 << true;
    QTest::newRow("Rot180") << QTransformedScreen::Rot180 << false;
    QTest::newRow("Rot270") << QTransformedScreen::Rot270 << true;
}

void tst_QTransformedScreen::setTransformation()
{
    // Not really failures but equal height and width makes this test useless
    QVERIFY(screen->deviceWidth() != screen->deviceHeight());

    screen->setTransformation(QTransformedScreen::None);
    int dw = screen->deviceWidth();
    int dh = screen->deviceHeight();
    int mmw = screen->physicalWidth();
    int mmh = screen->physicalHeight();

    QFETCH(QTransformedScreen::Transformation, transformation);
    QFETCH(bool, swap);

    screen->setTransformation(transformation);
    QCOMPARE(screen->deviceWidth(), dw);
    QCOMPARE(screen->deviceHeight(), dh);

    if (swap) {
        QCOMPARE(screen->width(), dh);
        QCOMPARE(screen->height(), dw);
        QCOMPARE(screen->physicalWidth(), mmh);
        QCOMPARE(screen->physicalHeight(), mmw);
    } else {
        QCOMPARE(screen->width(), dw);
        QCOMPARE(screen->height(), dh);
        QCOMPARE(screen->physicalWidth(), mmw);
        QCOMPARE(screen->physicalHeight(), mmh);
    }
}

void tst_QTransformedScreen::qwsDisplay_setTransformation()
{
    QDesktopWidget *desktop = QApplication::desktop();

    // test maximized windows
    {
        QWidget w;
        w.showMaximized();
        QApplication::processEvents();

        const int screen = desktop->screenNumber(&w);
        QCOMPARE(desktop->availableGeometry(screen), w.frameGeometry());

        for (int i = QTransformedScreen::None; i <= QTransformedScreen::Rot270; ++i) {
            QWSDisplay::instance()->setTransformation(i, screen);
            QApplication::processEvents();
            QCOMPARE(desktop->availableGeometry(screen), w.frameGeometry());
        }
    }

    // test fullscreen windows
    {
        QWidget w;
        w.showFullScreen();
        QApplication::processEvents();

        const int screen = desktop->screenNumber(&w);
        QCOMPARE(desktop->screenGeometry(screen), w.geometry());

        for (int i = QTransformedScreen::None; i <= QTransformedScreen::Rot270; ++i) {
            QWSDisplay::instance()->setTransformation(i, screen);
            QApplication::processEvents();
            QCOMPARE(desktop->screenGeometry(screen), w.geometry());
        }
    }
}

QTEST_MAIN(tst_QTransformedScreen)

#include "tst_qtransformedscreen.moc"

#else // Q_WS_QWS
QTEST_NOOP_MAIN
#endif
