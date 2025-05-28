#include "glacierdevicepassword.h"

#include <QCryptographicHash>
#include <QDir>

const QString deviceLockDirPath = "/etc/glacier";
const QString deviceLockFile = "/etc/glacier/deviceLock";

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
