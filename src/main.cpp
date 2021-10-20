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

int main(int argc, char *argv[])
{
/*
    --is-set
    --check-code <CODE>
    --set-code <OLD_CODE> <NEW_CODE>
    --is-encryption-supported
    --encrypt-home
    --set-config-key <TOKEN> <KEY> <VALUE>
    --wipe
*/
    QCoreApplication a(argc, argv);

    DeviceLocking *dvl = new DeviceLocking();
    QCommandLineOption isSet(QStringList() << "is-set");
    QCommandLineOption checkCode(QStringList() << "check-code");
    QCommandLineOption setCode(QStringList() << "set-code");
    QCommandLineOption isEncryptionSupported(QStringList() << "is-encryption-supported");
    QCommandLineOption encryptHome(QStringList() << "encrypt-home");
    QCommandLineOption wipe(QStringList() << "wipe");

    QCommandLineParser parser;
    parser.addOption(isSet);
    parser.addOption(checkCode);
    parser.addOption(setCode);
    parser.addOption(isEncryptionSupported);
    parser.addOption(encryptHome);
    parser.addOption(wipe);
    parser.process(a);

    if(parser.isSet(isSet)) {
        if(dvl->isSet()) {
            return NemoDeviceLock::HostAuthenticationInput::Success;
        }
    }

    if(parser.isSet(checkCode)) {
        const QStringList args = parser.positionalArguments();
        if(dvl->checkCode(args[0].toUtf8())) {
            return NemoDeviceLock::HostAuthenticationInput::Success;
        }
    }

    if(parser.isSet(setCode)) {
        const QStringList args = parser.positionalArguments();
        if(dvl->setCode(args[0].toUtf8(), args[1].toUtf8())) {
            return NemoDeviceLock::HostAuthenticationInput::Success;
        }
    }

    if(parser.isSet(isEncryptionSupported)) {
        if(dvl->isEncryptionSupported()) {
            return NemoDeviceLock::HostAuthenticationInput::Success;
        }
    }

    if(parser.isSet(encryptHome)) {
        dvl->encryptHome();
    }

    if(parser.isSet(wipe)) {
        dvl->wipe();
    }

    dvl->~DeviceLocking();

    return NemoDeviceLock::HostAuthenticationInput::Failure;
}
