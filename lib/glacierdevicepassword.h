#ifndef GLACIERDEVICEPASSWORD_H
#define GLACIERDEVICEPASSWORD_H

#include <QObject>

class GlacierDevicePassword : public QObject {
    Q_OBJECT
public:
    explicit GlacierDevicePassword(QObject* parent = nullptr);

    bool isSet();
    void wipe();

    bool checkCode(QByteArray code);
    bool setCode(QByteArray oldCode, QByteArray newCode);
};

#endif // GLACIERDEVICEPASSWORD_H
