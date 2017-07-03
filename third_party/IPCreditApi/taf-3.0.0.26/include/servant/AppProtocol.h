#ifndef __TAF_PROTOCOL_H_
#define __TAF_PROTOCOL_H_

#include <string>
#include <memory>
#include <map>
#include <vector>
#include "jce/RequestF.h"
#include "jce/wup.h"
#include "servant/BaseF.h"
#include "util/tc_epoll_server.h"

using namespace std;
using namespace wup;

namespace taf
{

template<typename T>
T net2host(T len)
{
    switch(sizeof(T))
    {
    case sizeof(uint8_t): return len;
    case sizeof(uint16_t): return ntohs(len);
    case sizeof(uint32_t): return ntohl(len);
    }
    assert(true);
    return 0;
}
//////////////////////////////////////////////////////////////////////
// Э�����
class AppProtocol
{
public:
    /**
     * ����Э��
     * @param in, Ŀǰ��buffer
     * @param out, һ�������İ�
     *
     * @return int, 0��ʾû�н�����ȫ, 1��ʾ�յ�һ��������
     */
    static int parse(string &in, string &out)
    {
        return parseLen<10000000>(in,out);
    }

    template<taf::Int32 iMaxLength>
    static int parseLen(string &in, string &out)
    {
        if(in.length() < sizeof(taf::Int32))
        {
            return TC_EpollServer::PACKET_LESS;
        }

        taf::Int32 iHeaderLen;

        memcpy(&iHeaderLen, in.c_str(), sizeof(taf::Int32));

        iHeaderLen = ntohl(iHeaderLen);

        if(iHeaderLen < taf::Int32(sizeof(taf::Int32))|| iHeaderLen > iMaxLength)
        {
            return TC_EpollServer::PACKET_ERR;
        }

        if((int)in.length() < iHeaderLen)
        {
            return TC_EpollServer::PACKET_LESS;
        }

        out = in.substr(sizeof(taf::Int32), iHeaderLen - sizeof(taf::Int32));

        in  = in.substr(iHeaderLen);

        return TC_EpollServer::PACKET_FULL;
    }

    /**
     * ����Э��
     * @param in, Ŀǰ��buffer
     * @param out, һ�������İ�
     *
     * @return int, 0��ʾû�н�����ȫ, 1��ʾ�յ�һ��������
     */
    static int parseAdmin(string &in, string &out);

    /**
     *
     * @param T
     * @param offset
     * @param netorder
     * @param in
     * @param out
     * @return int
     */
    template<size_t offset, typename T, bool netorder>
    static int parseStream(string& in, string& out)
    {
        if(in.length() < offset + sizeof(T))
        {
            return TC_EpollServer::PACKET_LESS;
        }

        T iHeaderLen = 0;

        ::memcpy(&iHeaderLen, in.c_str() + offset, sizeof(T));

        if (netorder)
        {
            iHeaderLen = net2host<T>(iHeaderLen);
        }

        if (iHeaderLen < (T)(offset + sizeof(T)) || (uint32_t)iHeaderLen > 100000000)
        {
            return TC_EpollServer::PACKET_ERR;
        }

        if (in.length() < (uint32_t)iHeaderLen)
        {
            return TC_EpollServer::PACKET_LESS;
        }

        out = in.substr(0, iHeaderLen);

        in  = in.substr(iHeaderLen);

        return TC_EpollServer::PACKET_FULL;
    }
};
//////////////////////////////////////////////////////////////////////
typedef TC_Functor<void, TL::TLMaker<const RequestPacket&, string&>::Result> request_protocol;
/**
 * ����Э�鴦��, ����ֵ��ʾ�����˶����ֽ�
 * ��ܲ���Զ��Դ����˰�������
 */
typedef TC_Functor<size_t, TL::TLMaker<const char *, size_t, list<ResponsePacket>&>::Result> response_protocol;

/**
 * �ͻ����Զ���Э������
 */
class ProxyProtocol
{
public:
    /**
     * ����
     */
    ProxyProtocol() : requestFunc(streamRequest) {}

    /**
     * ��ͨ�����������
     * @param request
     * @param buff
     */
    static void streamRequest(const RequestPacket& request, string& buff)
    {
        buff.assign((const char*)(&request.sBuffer[0]), request.sBuffer.size());
    }

    /**
     * ��ͨ����ֹ����taf�����
     * @param request
     * @param buff
     */
    template
    <
        size_t offset,
        typename T,
        bool netorder,
        size_t idOffset,
        typename K,
        bool idNetorder,
        size_t packetMaxSize
    >
    static size_t streamResponse(const char* recvBuffer, size_t length, list<ResponsePacket>& done)
    {
        size_t pos = 0;

        while (pos < length)
        {
            uint32_t len = length - pos;

            if (len < offset + sizeof(T))
            {
                break;
            }

            T iHeaderLen = 0;

            ::memcpy(&iHeaderLen, recvBuffer + pos + offset, sizeof(T));

            if (netorder)
            {
                iHeaderLen = net2host<T>(iHeaderLen);
            }

            //��һ�±���,���ȴ���10M
            size_t sizeHeaderLen = static_cast<size_t>(iHeaderLen);
            if (sizeHeaderLen > packetMaxSize || iHeaderLen == 0)
            {
                throw JceDecodeException("packet length too long or zero,len:(" + TC_Common::tostr(packetMaxSize) + ")" + TC_Common::tostr(iHeaderLen));
            }

            //��û�н���ȫ
            if (len < iHeaderLen)
            {
                break;
            }

            ResponsePacket rsp;

            rsp.sBuffer.reserve(iHeaderLen);
            rsp.sBuffer.resize(iHeaderLen);

            ::memcpy(&rsp.sBuffer[0], recvBuffer + pos, iHeaderLen);

            K requestId;

            ::memcpy(&requestId, recvBuffer + pos + idOffset, sizeof(K));

            if (idNetorder)
            {
                requestId = net2host<K>(requestId);
            }

            rsp.iRequestId = static_cast<uint32_t>(requestId);

            done.push_back(rsp);

            pos += iHeaderLen;
        }

        return pos;
    }

    template
    <
        size_t offset,
        typename T,
        bool netorder,
        size_t idOffset,
        typename K,
        bool idNetorder
    >
    static size_t streamResponse(const char* recvBuffer, size_t length, list<ResponsePacket>& done)
    {
        return streamResponse<offset, T, netorder, idOffset, K, idNetorder, 10000000>(recvBuffer, length, done);
    }

    /**
     * wup��Ӧ��(wup����Ӧ�����ResponsePacket��buffer��)
     * @param request
     * @param buff
     */
    static size_t wupResponse(const char* recvBuffer, size_t length, list<ResponsePacket>& done)
    {
        return wupResponseLen<10000000>(recvBuffer,length,done);
    }

    template<uint32_t iMaxLength>
    static size_t wupResponseLen(const char* recvBuffer, size_t length, list<ResponsePacket>& done)
    {
        size_t pos = 0;
        while (pos < length)
        {
            uint32_t len = length - pos;

            if(len < sizeof(taf::Int32))
            {
                break;
            }

            uint32_t iHeaderLen = ntohl(*(uint32_t*)(recvBuffer + pos));

            //��һ�±���,���ȴ���10M
            if (iHeaderLen > iMaxLength || iHeaderLen < sizeof(taf::Int32))
            {
                throw JceDecodeException("packet length too long or too short,len:" + TC_Common::tostr(iHeaderLen));
            }

            //��û�н���ȫ
            if (len < iHeaderLen)
            {
                //������ͷ�Ƿ���ȷ
                static const uint32_t head = 20;

                if(len >= head)
                {
                    JceInputStream<BufferReader> is;

                    is.setBuffer(recvBuffer + pos + sizeof(taf::Int32), head);

                    //wup������requestpackage
                    RequestPacket rsp;

                    is.read(rsp.iVersion, 1, true);

                    if (rsp.iVersion != WUPVERSION && rsp.iVersion != WUPVERSION2)
                    {
                        throw JceDecodeException("version not correct, version:" + TC_Common::tostr(rsp.iVersion));
                    }

                    is.read(rsp.cPacketType, 2, true);

                    if (rsp.cPacketType != JCENORMAL)
                    {
                        throw JceDecodeException("packettype not correct, packettype:" + TC_Common::tostr((int)rsp.cPacketType));
                    }

                    is.read(rsp.iMessageType, 3, true);
                    is.read(rsp.iRequestId, 4, true);
                }
                break;
            }
            else
            {
                JceInputStream<BufferReader> is;

                //buffer����4���ֽڳ���
                vector<char> buffer;

                buffer.reserve(iHeaderLen);
                buffer.resize(iHeaderLen);
                memcpy(&(buffer[0]), recvBuffer + pos, iHeaderLen);

                is.setBuffer(&(buffer[0]) + sizeof(taf::Int32), buffer.size() - sizeof(taf::Int32));

                pos += iHeaderLen;

                //WUP����Ӧ����ʵҲ�Ƿ��ذ�
                RequestPacket req;
                req.readFrom(is);

                if (req.iVersion != WUPVERSION && req.iVersion != WUPVERSION2)
                {
                    throw JceDecodeException("version not correct, version:" + TC_Common::tostr(req.iVersion));
                }

                if (req.cPacketType != JCENORMAL)
                {
                    throw JceDecodeException("packettype not correct, packettype:" + TC_Common::tostr((int)req.cPacketType));
                }

                ResponsePacket rsp;
                rsp.cPacketType     = req.cPacketType;
                rsp.iMessageType    = req.iMessageType;
                rsp.iRequestId      = req.iRequestId;
                rsp.iVersion        = req.iVersion;
                rsp.context         = req.context;
                //wup����Ӧ��ֱ�ӷ��뵽sBuffer����
                rsp.sBuffer         = buffer;

                done.push_back(rsp);
            }
        }

        return pos;
    }


public:
    /**
     * taf�����
     * @param request
     * @param buff
     */
    static void tafRequest(const RequestPacket& request, string& buff);

    /**
     * taf��Ӧ������
     * @param recvBuffer
     * @param done
     */
    static size_t tafResponse(const char* recvBuffer, size_t length, list<ResponsePacket>& done)
    {
        return tafResponseLen<10000000>(recvBuffer,length,done);
    }

    template<uint32_t iMaxLength>
    static size_t tafResponseLen(const char* recvBuffer, size_t length, list<ResponsePacket>& done)
    {
        size_t pos = 0;
        while (pos < length)
        {
            uint32_t len = length - pos;
            if(len < sizeof(taf::Int32))
            {
                break;
            }

            uint32_t iHeaderLen = ntohl(*(uint32_t*)(recvBuffer + pos));

            //��һ�±���,���ȴ���10M
            if (iHeaderLen > iMaxLength || iHeaderLen < sizeof(taf::Int32))
            {
                throw JceDecodeException("packet length too long or too short,len:" + TC_Common::tostr(iHeaderLen));
            }

            //��û�н���ȫ
            if (len < iHeaderLen)
            {
                //������ͷ�Ƿ���ȷ
                static const uint32_t head = 20;

                if(len >= head)
                {
                    JceInputStream<BufferReader> is;
                    is.setBuffer(recvBuffer + pos + sizeof(taf::Int32), head);

                    ResponsePacket rsp;
                    is.read(rsp.iVersion, 1, false);

                    if (rsp.iVersion != JCEVERSION)
                    {
                        throw JceDecodeException("version not correct, version:" + TC_Common::tostr(rsp.iVersion));
                    }

                    is.read(rsp.cPacketType, 2, false);

                    if (rsp.cPacketType != JCENORMAL)
                    {
                        throw JceDecodeException("packettype not correct, packettype:" + TC_Common::tostr((int)rsp.cPacketType));
                    }

                    is.read(rsp.iRequestId, 3, false);
                    is.read(rsp.iMessageType, 4, false);
                    is.read(rsp.iRet, 5, false);

                    if (rsp.iRet < JCESERVERUNKNOWNERR)
                    {
                        throw JceDecodeException("response value not correct, value:" + TC_Common::tostr(rsp.iRet));
                    }
                }
                break;
            }
            else
            {
                JceInputStream<BufferReader> is;
                is.setBuffer(recvBuffer + pos + sizeof(taf::Int32), iHeaderLen - sizeof(taf::Int32));
                pos += iHeaderLen;

                ResponsePacket rsp;
                rsp.readFrom(is);

                if (rsp.iVersion != JCEVERSION)
                {
                    throw JceDecodeException("version not correct, version:" + TC_Common::tostr(rsp.iVersion));
                }

                if (rsp.cPacketType != JCENORMAL)
                {
                    throw JceDecodeException("packettype not correct, packettype:" + TC_Common::tostr((int)rsp.cPacketType));
                }

                if (rsp.iRet < JCESERVERUNKNOWNERR)
                {
                    throw JceDecodeException("response value not correct, value:" + TC_Common::tostr(rsp.iRet));
                }

                done.push_back(rsp);
            }
        }

        return pos;
    }

public:
    request_protocol requestFunc;

    response_protocol responseFunc;
};

//////////////////////////////////////////////////////////////////////
}
#endif
