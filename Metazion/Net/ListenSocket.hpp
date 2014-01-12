#ifndef MZ_NET_LISTENSOCKET_HPP
#define MZ_NET_LISTENSOCKET_HPP

#include "Metazion/Net/NetInclude.hpp"

#include <Metazion/Share/Memory/PieceBuffer.hpp>
#include <Metazion/Share/Memory/RingBuffer.hpp>
#include "Metazion/Net/Socket.hpp"

DECL_NAMESPACE_MZ_NET_BEGIN

class ListenSocket : public Socket {
    DISALLOW_COPY_AND_ASSIGN(ListenSocket)

public:
    ListenSocket();

    virtual ~ListenSocket();

public: // @Override
    void Reset();

    void OnStarted();

    bool IsAlive() const;

public:
    void SetLocalHost(const char* ip, int port);

    bool Listen(int backlog);

    IoStrategy& GetIoStrategy() {
        return m_listenStrategy;
    }

protected:
    Host m_localHost;

private:
    ListenStrategy m_listenStrategy;
};

DECL_NAMESPACE_MZ_NET_END

#endif // MZ_NET_LISTENSOCKET_HPP