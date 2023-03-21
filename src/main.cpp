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

#include <QCommandLineParser>
#include <QCoreApplication>
#include <nemo-devicelock/host/hostauthenticationinput.h>

#include "devicelocking.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    DeviceLocking* dvl = new DeviceLocking();
    QCommandLineOption isSet(QStringList() << "is-set");
    QCommandLineOption unlock(QStringList() << "unlock", "<code>");
    QCommandLineOption checkCode(QStringList() << "check-code", "<code>");
    QCommandLineOption setCode(QStringList() << "set-code", "<old_code> <new_code>");
    QCommandLineOption isEncryptionSupported(QStringList() << "is-encryption-supported");
    QCommandLineOption encryptHome(QStringList() << "encrypt-home");
    QCommandLineOption wipe(QStringList() << "wipe");
    QCommandLineOption clearCode(QStringList() << "clear-code");
    QCommandLineOption setConfigKey(QStringList() << "set-config-key", "<code> <key> <value>");

    QCommandLineParser parser;
    parser.addOption(isSet);
    parser.addOption(unlock);
    parser.addOption(checkCode);
    parser.addOption(setCode);
    parser.addOption(isEncryptionSupported);
    parser.addOption(encryptHome);
    parser.addOption(wipe);
    parser.addOption(clearCode);
    parser.addOption(setConfigKey);
    parser.addHelpOption();
    parser.process(a);

    const QStringList args = parser.positionalArguments();

    if (parser.isSet(isSet)) {
        if (dvl->isSet()) {
            return NemoDeviceLock::HostAuthenticationInput::Success;
        }
    }

    if (parser.isSet(checkCode) || parser.isSet(unlock)) {
        if (args.length() != 1) {
            qWarning() << "--check-code/--unlock requires <CODE> argument";
            return NemoDeviceLock::HostAuthenticationInput::Failure;
        }

        if (dvl->checkCode(args[0].toUtf8())) {
            return NemoDeviceLock::HostAuthenticationInput::Success;
        }
    }

    if (parser.isSet(setCode)) {
        if (args.length() != 2) {
            qWarning() << "--set-code requires <old_code> <new_code>";
            return NemoDeviceLock::HostAuthenticationInput::Failure;
        }

        if (dvl->setCode(args[0].toUtf8(), args[1].toUtf8())) {
            return NemoDeviceLock::HostAuthenticationInput::Success;
        }
    }

    if (parser.isSet(isEncryptionSupported)) {
        if (dvl->isEncryptionSupported()) {
            return NemoDeviceLock::HostAuthenticationInput::Success;
        }
    }

    if (parser.isSet(encryptHome)) {
        dvl->encryptHome();
        return NemoDeviceLock::HostAuthenticationInput::Success;
    }

    if (parser.isSet(wipe) || parser.isSet(clearCode)) {
        dvl->wipe();
        return NemoDeviceLock::HostAuthenticationInput::Success;
    }

    if (parser.isSet(setConfigKey)) {
        if (args.length() != 3) {
            qWarning() << "--set-config-key incorrect number of arguments";
            return NemoDeviceLock::HostAuthenticationInput::Failure;
        }

        if (!dvl->checkCode(args[0].toUtf8())) {
            qWarning() << "Entered code is not valid" << args[0].toUtf8();
            return NemoDeviceLock::HostAuthenticationInput::Failure;
        }

        if (dvl->setConfigKey(args[1].toUtf8(), args[2].toUtf8())) {
            return NemoDeviceLock::HostAuthenticationInput::Success;
        }
    }

    dvl->~DeviceLocking();

    return NemoDeviceLock::HostAuthenticationInput::Failure;
}
