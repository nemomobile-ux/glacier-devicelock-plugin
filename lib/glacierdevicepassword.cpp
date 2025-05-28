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

#include "glacierdevicepassword.h"

#include <QCryptographicHash>
#include <QDir>

#ifndef UNIT_TEST
const QString deviceLockDirPath = "/etc/glacier";
const QString deviceLockFile = "/etc/glacier/deviceLock";
#else
const QString deviceLockDirPath = ".";
const QString deviceLockFile = "deviceLock";
#endif

GlacierDevicePassword::GlacierDevicePassword(QObject* parent)
    : QObject { parent }
{
    QDir deviceLockDir(deviceLockDirPath);
    if (!deviceLockDir.exists()) {
        deviceLockDir.mkpath(deviceLockDirPath);
    }
}

bool GlacierDevicePassword::isSet()
{
    QFile keyFile(deviceLockFile);

    if (keyFile.exists() && keyFile.size() > 0) {
        return true;
    }
    return false;
}

void GlacierDevicePassword::wipe()
{
    /*
     * TODO add correct wipe. Now wipe just remove key code file
     */
    QFile::remove(deviceLockFile);
}

bool GlacierDevicePassword::checkCode(QByteArray code)
{
    if(!isSet()) {
        return true;
    }

    QFile keyFile(deviceLockFile);
    keyFile.open(QIODevice::ReadOnly);
    QByteArray keyBase64 = keyFile.readLine();
    keyFile.close();
    QByteArray key = QByteArray::fromBase64(keyBase64);

    return key == QCryptographicHash::hash(code, QCryptographicHash::Sha256);
}

bool GlacierDevicePassword::setCode(QByteArray oldCode, QByteArray newCode)
{
    if (!checkCode(oldCode)) {
        return false;
    }

    QFile keyFile(deviceLockFile);
    QByteArray key = QCryptographicHash::hash(newCode, QCryptographicHash::Sha256);

    if (keyFile.open(QIODevice::WriteOnly)) {
        keyFile.write(key.toBase64());
        keyFile.close();
        return true;
    } else {
        return false;
    }
}
