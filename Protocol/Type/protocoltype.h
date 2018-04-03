#ifndef PROTOCOLTYPE_H
#define PROTOCOLTYPE_H

namespace Cuma
{
namespace Protocol
{

enum Type
{
    None,
    Ping,
    Disconnect,
    Connect,
    Search,
    Download,
    Spread,
    Bypass
};

}
}
#endif // PROTOCOLTYPE_H
