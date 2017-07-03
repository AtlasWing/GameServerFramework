#ifndef __REQUESTF_SYM_H_
#define __REQUESTF_SYM_H_

#include "Jce_mcare.h"


struct RequestPacket : public JceStructBase
{
public:
    static String className();
    String structName() const;
    RequestPacket();
    void reset();
    void writeTo(JceOutputStream& _os) const;
    void readFrom(JceInputStream& _is);
public:
    Short iVersion;
    Char cPacketType;
    Int32 iMessageType;
    Int32 iRequestId;
    String sServantName;
    String sFuncName;
    JArray<Char> sBuffer;
    Int32 iTimeout;
    JMapWrapper<String,String> context;
    JMapWrapper<String,String> status;

    RequestPacket& operator=(const RequestPacket& r);
};
bool operator==(const RequestPacket& l, const RequestPacket& r);
bool operator!=(const RequestPacket&l, const RequestPacket&r);

#endif
