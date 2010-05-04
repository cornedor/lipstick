/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of mhome.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <QtTest/QtTest>
#include <MTheme>
#include "ut_plaindesktopbackgroundpixmap.h"
#include "plaindesktopbackgroundpixmap.h"

QString QPixmapLoadFileName;
bool QPixmapLoadReturnValue;
bool QPixmap::load(const QString & fileName, const char *, Qt::ImageConversionFlags)
{
    QPixmapLoadFileName = fileName;
    return QPixmapLoadReturnValue;
}

QString MThemePixmapId;
const QPixmap *MThemePixmapDefaultValue;
const QPixmap *MThemePixmapReturnValue;
const QPixmap *MTheme::pixmap(const QString &id, const QSize &)
{
    MThemePixmapId = id;
    return MThemePixmapReturnValue;
}

void MTheme::releasePixmap(const QPixmap *)
{
}

void Ut_PlainDesktopBackgroundPixmap::initTestCase()
{
    MThemePixmapDefaultValue = new QPixmap;
}

void Ut_PlainDesktopBackgroundPixmap::cleanupTestCase()
{
    delete MThemePixmapDefaultValue;
}

void Ut_PlainDesktopBackgroundPixmap::init()
{
    MThemePixmapReturnValue = MThemePixmapDefaultValue;
}

void Ut_PlainDesktopBackgroundPixmap::cleanup()
{
}

void checkBlurredPixmap(const PlainDesktopBackgroundPixmap &pixmap)
{
    QVERIFY(pixmap.blurredPixmap() != NULL);
    QCOMPARE(pixmap.pixmap()->size(), pixmap.blurredPixmap()->size());
}

void Ut_PlainDesktopBackgroundPixmap::testConstructingFromFile()
{
    QPixmapLoadReturnValue = true;
    QString expectedFileName("/tmp/file.png");
    PlainDesktopBackgroundPixmap pixmap(expectedFileName, "test", 0);
    QCOMPARE(QPixmapLoadFileName, expectedFileName);
    QVERIFY(pixmap.pixmapFromFile_ != NULL);
    QCOMPARE(pixmap.pixmap(), pixmap.pixmapFromFile_.data());

    checkBlurredPixmap(pixmap);
}

void Ut_PlainDesktopBackgroundPixmap::testConstructingFromFileFails()
{
    QString expectedName("test");
    QPixmap expectedPixmap(50, 50);
    MThemePixmapReturnValue = &expectedPixmap;
    QPixmapLoadReturnValue = false;

    PlainDesktopBackgroundPixmap pixmap("/tmp/file.png", expectedName, 0);
    QCOMPARE(pixmap.pixmapFromFile_.isNull(), true);
    QCOMPARE(pixmap.pixmapFromTheme_, &expectedPixmap);
    QCOMPARE(MThemePixmapId, expectedName);
    QCOMPARE(pixmap.pixmap(), pixmap.pixmapFromTheme_);

    checkBlurredPixmap(pixmap);
}

void Ut_PlainDesktopBackgroundPixmap::testConstructingFromTheme()
{
    QString expectedName("test");
    QPixmap expectedPixmap(50, 50);
    MThemePixmapReturnValue = &expectedPixmap;

    PlainDesktopBackgroundPixmap pixmap(expectedName, "default", 0);
    QCOMPARE(pixmap.pixmapFromFile_.isNull(), true);
    QCOMPARE(pixmap.pixmapFromTheme_, &expectedPixmap);
    QCOMPARE(MThemePixmapId, expectedName);
    QCOMPARE(pixmap.pixmap(), pixmap.pixmapFromTheme_);

    // The blurred pixmap creation should have succeeded so pixmapRequestsFinished() signals should not be listened
    QCOMPARE(disconnect(MTheme::instance(), SIGNAL(pixmapRequestsFinished()), &pixmap, SLOT(createBlurredPixmap())), false);
    checkBlurredPixmap(pixmap);
}

void Ut_PlainDesktopBackgroundPixmap::testConstructingFromThemeIsDelayed()
{
    QPixmap brokenPixmap(-1, -1);
    MThemePixmapReturnValue = &brokenPixmap;
    PlainDesktopBackgroundPixmap pixmap("test", "default", 0);
    QSignalSpy spy(&pixmap, SIGNAL(pixmapUpdated()));

    // The blurred pixmap creation should have failed so pixmapRequestsFinished() signals should be listened
    QCOMPARE(disconnect(MTheme::instance(), SIGNAL(pixmapRequestsFinished()), &pixmap, SLOT(createBlurredPixmap())), true);
    QCOMPARE(pixmap.blurredPixmap(), (QPixmap *)NULL);
    QCOMPARE(spy.count(), 0);

    // If the pixmap becomes available the blur should be executed
    QPixmap validPixmap(50, 50);
    pixmap.pixmapFromTheme_ = &validPixmap;
    connect(this, SIGNAL(pixmapRequestsFinished()), &pixmap, SLOT(createBlurredPixmap()));
    emit pixmapRequestsFinished();
    QCOMPARE(spy.count(), 1);
    checkBlurredPixmap(pixmap);
}

void Ut_PlainDesktopBackgroundPixmap::testConstructingFromEmptyName()
{
    QString expectedName("test");
    QPixmap expectedPixmap(50, 50);
    MThemePixmapReturnValue = &expectedPixmap;

    PlainDesktopBackgroundPixmap pixmap("", expectedName, 0);
    QCOMPARE(pixmap.pixmapFromFile_.isNull(), true);
    QCOMPARE(pixmap.pixmapFromTheme_, &expectedPixmap);
    QCOMPARE(MThemePixmapId, expectedName);
    QCOMPARE(pixmap.pixmap(), pixmap.pixmapFromTheme_);

    checkBlurredPixmap(pixmap);
}

QTEST_MAIN(Ut_PlainDesktopBackgroundPixmap)