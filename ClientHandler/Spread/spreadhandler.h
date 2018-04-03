#ifndef SPREADHANDLER_H
#define SPREADHANDLER_H

#include <QObject>
#include "FireWall/accessclientfirewall.h"
#include "Block/AddressBlock/addressblock.h"
#include "Block/FileBlock/blockstruct.h"
#include "DbAddressPath/dbaddresspathbyfile.h"

class SpreadHandler : public QObject
{
    Q_OBJECT
public:
    explicit SpreadHandler(Cuma::);

signals:

public slots:
};

#endif // SPREADHANDLER_H
