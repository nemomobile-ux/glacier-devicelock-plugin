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

#include <glib.h>
#include <unistd.h>

DeviceLocking::DeviceLocking(QObject *parent)
    : QObject()
    , m_settingsPath("/usr/share/lipstick/devicelock/devicelock_settings.conf")
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
    if(keyFile.exists()) {
        if(!checkCode(oldCode)) {
            return false;
        }
    }

    int minLength = getConfigKey("desktop", "nemo\\devicelock\\code_min_length").toInt();
    int maxLength = getConfigKey("desktop", "nemo\\devicelock\\code_max_length").toInt();

    if(code.length() < minLength || code.length() > maxLength) {
        return false;
    }

    QByteArray key = QCryptographicHash::hash(code, QCryptographicHash::Sha256);

    keyFile.open(QIODevice::WriteOnly);
    keyFile.write(key);
    keyFile.close();

    return true;
}

bool DeviceLocking::setConfigKey(QByteArray key, QByteArray value)
{
/*Preparate keyvalue*/
    QByteArray groupName = QString(key).split("/").at(1).toUtf8();
    key.remove(0,groupName.length()+2);
    key.replace("/","\\");

/*Write to config*/
    GError *error = nullptr;
    gchar *str;
    GKeyFile * const settings = g_key_file_new();

    g_key_file_load_from_file(settings
                              , m_settingsPath.toUtf8().constData()
                              , G_KEY_FILE_NONE
                              , &error);

    if(error) {
        return false;
    }

    g_key_file_set_string(settings
                          ,groupName.constData()
                          ,key.constData()
                          ,value.constData());


    str = g_key_file_to_data (settings, NULL, NULL);
    g_key_file_free(settings);
    error = nullptr;

    if (!g_file_set_contents("/usr/share/lipstick/devicelock/devicelock_settings.conf", str, -1, &error)) {
        return false;
    }

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

QByteArray DeviceLocking::getConfigKey(QByteArray group, QByteArray key)
{
    GError *error = nullptr;
    gchar *str;
    GKeyFile * const settings = g_key_file_new();

    g_key_file_load_from_file(settings
                              , m_settingsPath.toUtf8().constData()
                              , G_KEY_FILE_NONE
                              , &error);
    if(error) {
        qWarning() << "Error load settings file" << m_settingsPath;
        return QByteArray();
    }

    error = nullptr;
    gchar *string = g_key_file_get_string(settings,
                          group.constData(),
                          key.constData(),
                          &error);

    if(error) {
        qWarning() << "Can't get key" << key << "from" << m_settingsPath;
        return QByteArray();
    }

    return QByteArray(string);
}
