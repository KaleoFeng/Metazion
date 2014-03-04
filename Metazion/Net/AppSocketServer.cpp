#include "Metazion/Net/AppSocketServer.hpp"

#include <Metazion/Share/Thread/ThreadLocalStorage.hpp>

DECL_NAMESPACE_MZ_NET_BEGIN

AppSocketServer::AppSocketServer()
    : m_encodeBufferKey(0)
    , m_decodeBufferKey(0) {}

AppSocketServer::~AppSocketServer() {}

bool AppSocketServer::Initialize(int socketCapacity, int ioThreadNumber) {
    m_encodeBufferKey = NS_SHARE::ThreadLocalStorage::Alloc();
    if (-1 == m_encodeBufferKey) {
        return false;
    }

    m_decodeBufferKey = NS_SHARE::ThreadLocalStorage::Alloc();
    if (-1 == m_decodeBufferKey) {
        return false;
    }

    return NormalSocketServer::Initialize(socketCapacity, ioThreadNumber);
}

void AppSocketServer::Finalize() {
    NormalSocketServer::Finalize();

    if (-1 != m_decodeBufferKey) {
        NS_SHARE::ThreadLocalStorage::Free(m_decodeBufferKey);
    }
    
    if (-1 != m_encodeBufferKey) {
        NS_SHARE::ThreadLocalStorage::Free(m_encodeBufferKey);
    }
}

int AppSocketServer::LockSockets(SocketFilter& filter, SocketArray_t& socketArray) {
    socketArray.Clear();

    Lock();
    const int socketCapacity = GetSocketCapacity();
    for (int index = 0; index < socketCapacity; ++index) {
        Socket* socket = GetSocket(index);
        if (IsNull(socket)) {
            continue;
        }
        if (!filter.Filter(socket)) {
            continue;
        }

        socket->Retain();
        socketArray.Add(socket);
    }
    Unlock();

    return socketArray.GetSize();
}

void AppSocketServer::UnlockSockets(SocketArray_t& socketArray) {
    const int size = socketArray.GetSize();
    for (int index = 0; index < size; ++index) {
        Socket* socket = socketArray[index];
        ASSERT_TRUE(!IsNull(socket));
        socket->Release();
    }
}

EncodeBuffer& AppSocketServer::GetEncodeBuffer() {
    void* buffer = NS_SHARE::ThreadLocalStorage::GetValue(m_encodeBufferKey);
    if (IsNull(buffer)) {
        buffer = new ThreadEncodeBuffer();
        bool ret = NS_SHARE::ThreadLocalStorage::SetValue(m_encodeBufferKey, buffer);
        ASSERT_TRUE(ret);
    }

    ThreadEncodeBuffer* threadBuffer = static_cast<ThreadEncodeBuffer*>(buffer);
    return threadBuffer->m_buffer;
}

DecodeBuffer& AppSocketServer::GetDecodeBuffer() {
    void* buffer = NS_SHARE::ThreadLocalStorage::GetValue(m_decodeBufferKey);
    if (IsNull(buffer)) {
        buffer = new ThreadDecodeBuffer();
        bool ret = NS_SHARE::ThreadLocalStorage::SetValue(m_decodeBufferKey, buffer);
        ASSERT_TRUE(ret);
    }

    ThreadDecodeBuffer* threadBuffer = static_cast<ThreadDecodeBuffer*>(buffer);
    return threadBuffer->m_buffer;
}

DECL_NAMESPACE_MZ_NET_END
