#ifndef __TAF_CONNECTOR_H_
#define __TAF_CONNECTOR_H_

namespace taf
{

class Transceiver;
class ObjectProxy;
class EndpointInfo;

/**
 * 连接器，返回一个transceiver
 */
class Connector
{
public:

    /**
     * 建立连接
     * @param timeout
     *
     * @return TransceiverPtr
     */
    static Transceiver* create(ObjectProxy *op, const EndpointInfo &ep);

    /**
     * 异步发起连接
     * 
     * @param trans 
     */
    static void connect(Transceiver* trans);
};
///////////////////////////////////////////////////
}

#endif
