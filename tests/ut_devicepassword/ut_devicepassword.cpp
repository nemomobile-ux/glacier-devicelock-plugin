/*
 * Copyright (C) 2025 Chupligin Sergey <neochapay@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "ut_devicepassword.h"
#include <QtTest/QtTest>
#include <glacierdevicepassword.h>

void Ut_DevicePassword::init()
{
    m_devicePassword = new GlacierDevicePassword();
}

void Ut_DevicePassword::cleanup()
{
    QFile passFile("deviceLock");
    passFile.remove();

    delete m_devicePassword;
}

void Ut_DevicePassword::testNotHavePasswordFile()
{
    QFile passFile("deviceLock");
    QCOMPARE(passFile.exists(), m_devicePassword->isSet());
}

void Ut_DevicePassword::testEmptyPasswordFile()
{
    QFile passFile("deviceLock");
    passFile.open(QIODevice::WriteOnly);
    passFile.close();
    QCOMPARE(false, m_devicePassword->isSet());
}

void Ut_DevicePassword::testSetNewPassword()
{
    QFile passFile("deviceLock");
    passFile.open(QIODevice::WriteOnly);
    passFile.close();
    QCOMPARE(m_devicePassword->setCode("1234", "0000"), true);
}

void Ut_DevicePassword::testWrongPassword()
{
    QFile passFile("deviceLock");
    passFile.open(QIODevice::WriteOnly);
    passFile.close();
    m_devicePassword->setCode("1234", "0000");
    QCOMPARE(m_devicePassword->checkCode("9999"), false);
}

void Ut_DevicePassword::testCorrectPassword()
{
    QFile passFile("deviceLock");
    passFile.open(QIODevice::WriteOnly);
    passFile.close();
    m_devicePassword->setCode("1234", "0000");
    QCOMPARE(m_devicePassword->checkCode("0000"), true);
}

void Ut_DevicePassword::testWrongPasswordChanging()
{
    QFile passFile("deviceLock");
    passFile.open(QIODevice::WriteOnly);
    passFile.close();
    m_devicePassword->setCode("1234", "0000");
    QCOMPARE(m_devicePassword->setCode("1234", "0000"), false);
}

void Ut_DevicePassword::testCorrectPasswordChanging()
{
    QFile passFile("deviceLock");
    passFile.open(QIODevice::WriteOnly);
    passFile.close();
    m_devicePassword->setCode("1234", "0000");
    QCOMPARE(m_devicePassword->setCode("0000", "5555"), true);
    QCOMPARE(m_devicePassword->checkCode("0000"), false);
    QCOMPARE(m_devicePassword->checkCode("5555"), true);
}

QTEST_MAIN(Ut_DevicePassword)
