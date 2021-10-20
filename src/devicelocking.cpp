/*
 * Copyright (C) 2021 Chupligin Sergey <neochapay@gmail.com>
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

#include "devicelocking.h"
#include <nemo-devicelock/host/hostauthenticationinput.h>
#include <QDir>
#include <QDebug>
#include <QCryptographicHash>

DeviceLocking::DeviceLocking(QObject *parent)
{
    m_currentUser = currentUser();
    if(m_currentUser.isEmpty()) {
        qDebug() << "FUCK!";
        exit(NemoDeviceLock::HostAuthenticationInput::Failure);
    }

    QDir deviceLockDir("/home/"+m_currentUser+"/.config/glacier-devicelock/");
    if(!deviceLockDir.exists()) {
        deviceLockDir.mkpath("/home/"+m_currentUser+"/.config/glacier-devicelock/");
    }
}

DeviceLocking::~DeviceLocking()
{

}

bool DeviceLocking::isSet()
{
    QFile keyFile("/home/"+m_currentUser+"/.config/glacier-devicelock/key");
    if(keyFile.exists() && keyFile.size() > 0) {
        return true;
    }
    return false;
}

bool DeviceLocking::isEncryptionSupported()
{
    /*
     * TODO: Add encryption support
     */
    qDebug() << "Encryption not support";
    return false;
}

void DeviceLocking::encryptHome()
{
    qDebug() << "Call to encrypt home";
}

void DeviceLocking::wipe()
{
    /*
     * TODO add correct wipe. Now wipe just remove key code file
     */
    QFile::remove("/home/"+m_currentUser+"/.config/glacier-devicelock/key");
}

bool DeviceLocking::checkCode(QByteArray code)
{
    if(!isSet()) {
        return false;
    }

    QFile keyFile("/home/"+m_currentUser+"/.config/glacier-devicelock/key");
    keyFile.open(QIODevice::ReadOnly);
    QString key = keyFile.readLine();
    keyFile.close();

    if(key == QCryptographicHash::hash(code, QCryptographicHash::Sha256)) {
        return true;
    }

    return false;
}

bool DeviceLocking::setCode(QByteArray oldCode, QByteArray code)
{
    QFile keyFile("/home/"+m_currentUser+"/.config/glacier-devicelock/key");
    if(!checkCode(oldCode) && keyFile.exists()) {
        return false;
    }

    if(code.length() < 4) {
        return false;
    }

    QByteArray key = QCryptographicHash::hash(code, QCryptographicHash::Sha256);

    keyFile.open(QIODevice::WriteOnly);
    keyFile.write(key);
    keyFile.close();

    return true;
}

QString DeviceLocking::currentUser()
{
    /*
     * TODO: check to logind to current user session.
     * See here: https://www.freedesktop.org/wiki/Software/systemd/logind/
     */

    return "manjaro";
}
