/*
 * Copyright (C) 2021-2022 Chupligin Sergey <neochapay@gmail.com>
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
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <nemo-devicelock/host/hostauthenticationinput.h>

#include <glib.h>
#include <unistd.h>

DeviceLocking::DeviceLocking(QObject* parent)
    : QObject()
    , m_settingsPath("/usr/share/lipstick/devicelock/devicelock_settings.conf")
    , m_currentUser(currentUser())
{
    if (m_currentUser.isEmpty() || m_currentUser == "root") {
        exit(NemoDeviceLock::HostAuthenticationInput::Failure);
    }
    QString deviceLockDirPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir deviceLockDir(deviceLockDirPath);
    if (!deviceLockDir.exists()) {
        deviceLockDir.mkpath(deviceLockDirPath);
    }

    if (!QFile::exists(m_settingsPath)) {
        initConfig();
    }
}

DeviceLocking::~DeviceLocking()
{
}

bool DeviceLocking::isSet()
{
    QFile keyFile(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/key");
    if (keyFile.exists() && keyFile.size() > 0) {
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
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/key");
}

bool DeviceLocking::checkCode(QByteArray code)
{
    if (!isSet()) {
        return false;
    }

    QFile keyFile(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/key");
    keyFile.open(QIODevice::ReadOnly);
    QString key = keyFile.readLine();
    keyFile.close();

    if (key == QCryptographicHash::hash(code, QCryptographicHash::Sha256)) {
        setConfigKey("/desktop/nemo/devicelock/current_attempts", "0");

        QByteArray changeData;
        changeData.setNum(QDateTime::currentSecsSinceEpoch());
        setConfigKey("/desktop/nemo/devicelock/code_last_changed", changeData);
        return true;
    }

    int ca = getConfigKey("desktop", "nemo\\devicelock\\current_attempts", "0").toUInt();
    QByteArray currentAttempts;
    currentAttempts.setNum(ca + 1);
    setConfigKey("/desktop/nemo/devicelock/current_attempts", currentAttempts);

    return false;
}

bool DeviceLocking::setCode(QByteArray oldCode, QByteArray code)
{
    QByteArray _oldCode;
    QByteArray _code;

    _oldCode.setNum(oldCode.toInt());
    _code.setNum(code.toInt());

    QFile keyFile(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/key");
    if (keyFile.exists()) {
        if (!checkCode(oldCode)) {
            return false;
        }
    }

    int minLength = getConfigKey("desktop", "nemo\\devicelock\\code_min_length", "5").toInt();
    int maxLength = getConfigKey("desktop", "nemo\\devicelock\\code_max_length", "42").toInt();

    if (_code.length() < minLength || _code.length() > maxLength) {
        return false;
    }

    QByteArray key = QCryptographicHash::hash(_code, QCryptographicHash::Sha256);

    keyFile.open(QIODevice::WriteOnly);
    keyFile.write(key);
    keyFile.close();

    QByteArray codeLength;
    codeLength.setNum(_code.length());
    setConfigKey("/desktop/nemo/devicelock/code_current_length", codeLength);

    return true;
}

bool DeviceLocking::setConfigKey(QByteArray key, QByteArray value)
{
    /*Preparate keyvalue*/
    QByteArray groupName = QString(key).split("/").at(1).toUtf8();
    key.remove(0, groupName.length() + 2);
    key.replace("/", "\\");

    /*Write to config*/
    GError* error = nullptr;
    gchar* str;
    GKeyFile* const settings = g_key_file_new();

    g_key_file_load_from_file(settings, m_settingsPath.toUtf8().constData(), G_KEY_FILE_NONE, &error);

    if (error) {
        return false;
    }

    g_key_file_set_string(settings, groupName.constData(), key.constData(), value.constData());

    str = g_key_file_to_data(settings, NULL, NULL);
    g_key_file_free(settings);
    error = nullptr;

    if (!g_file_set_contents("/usr/share/lipstick/devicelock/devicelock_settings.conf", str, -1, &error)) {
        return false;
    }

    return true;
}

const QString DeviceLocking::currentUser()
{
    QString name = qgetenv("USER");
    if (name.isEmpty()) {
        name = qgetenv("USERNAME");
    }
    return name;
}

QByteArray DeviceLocking::getConfigKey(QByteArray group, QByteArray key, QByteArray defaultValue)
{
    GError* error = nullptr;
    gchar* str;
    GKeyFile* const settings = g_key_file_new();

    g_key_file_load_from_file(settings, m_settingsPath.toUtf8().constData(), G_KEY_FILE_NONE, &error);
    if (error) {
        qWarning() << "Error load settings file" << m_settingsPath;
        return QByteArray();
    }

    error = nullptr;
    gchar* string = g_key_file_get_string(settings,
        group.constData(),
        key.constData(),
        &error);

    if (error) {
        qWarning() << "Can't get key" << key << "from" << m_settingsPath;
        setConfigKey("/" + group + "/" + key, defaultValue);
        return defaultValue;
    }

    return QByteArray(string);
}

void DeviceLocking::initConfig()
{
    QDir confDir("/usr/share/lipstick/devicelock/");
    if (!confDir.exists()) {
        confDir.mkpath("/usr/share/lipstick/devicelock/");
    }
    QFile configFile(m_settingsPath);
    configFile.open(QIODevice::WriteOnly | QIODevice::Append);
    configFile.close();

    QByteArray confBase = "/desktop/nemo/devicelock/";

    // Load def value like here https://github.com/sailfishos/nemo-qml-plugin-devicelock/blob/master/src/nemo-devicelock/private/settingswatcher.cpp#L68
    setConfigKey(confBase + "automatic_locking", "0");
    setConfigKey(confBase + "code_current_length", "0");
    setConfigKey(confBase + "code_min_length", "5");
    setConfigKey(confBase + "code_max_length", "42");
    setConfigKey(confBase + "maximum_attempts", "-1");
    setConfigKey(confBase + "current_attempts", "0");
    setConfigKey(confBase + "peeking_allowed", "1");
    setConfigKey(confBase + "sideloading_allowed", "-1");
    setConfigKey(confBase + "show_notification", "1");
    setConfigKey(confBase + "code_input_is_keyboard", "false");
    setConfigKey(confBase + "code_current_is_digit_only", "true");
    setConfigKey(confBase + "encrypt_home", "false");
}
