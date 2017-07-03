#ifndef __TAF_CONNECTOR_H_
#define __TAF_CONNECTOR_H_

namespace taf
{

class Transceiver;
class ObjectProxy;
class EndpointInfo;

/**
 * ������������һ��transceiver
 */
class Connector
{
public:

    /**
     * ��������
     * @param timeout
     *
     * @return TransceiverPtr
     */
    static Transceiver* create(ObjectProxy *op, const EndpointInfo &ep);

    /**
     * �첽��������
     * 
     * @param trans 
     */
    static void connect(Transceiver* trans);
};
///////////////////////////////////////////////////
}

#endif
