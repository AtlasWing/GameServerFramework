// Copyright (c) 2012, Tencent Inc. All rights reserved.
// Author:  Cheng Xi <pattycheng@tencent.com>
// Created: 2012-07-29

#ifndef _IPCM_COMM_HPP_
#define _IPCM_COMM_HPP_

#include <iostream>
#include "IPCredit.h"

// define some common struct used by server and client

const size_t kApiVersion = 20120704;

enum ENUM_IP_TYPE
{
	IP_TYPE_UNKNOWN    =  0,
    IP_TYPE_FOREIGNIP  =  1, 
    IP_TYPE_HKIP       =  2, 
	IP_TYPE_WIFIIP     =  3,
	IP_TYPE_GATEWAYIP  =  4,
	IP_TYPE_PROXYIP    =  5,
};

enum ENUM_IP_LABLE
{
	FROM_WAP = 1 ,
    FROM_WX = 2,     
	FROM_BGP = 3,    
	FROM_IM = 4,     
};

struct VALUE {
    int64_t place;
    uint32_t extend;
    uint32_t oper;
    uint32_t nettype;
    uint8_t apn;
    uint8_t flable;
}; // struct VALUE

#pragma pack(1)
struct GeneralAttribute {
    uint32_t startIP;
    uint32_t endIP;
    uint32_t extend;
    uint32_t oper;
    uint32_t nettype;
    uint32_t apn;
    uint32_t flable;
    uint64_t place;
    GeneralAttribute(const newIPPro::IPAttribute& attr) {
        startIP = attr.startIP;
        endIP = attr.endIP;
        place = attr.place;
        extend = attr.extend;
        oper = attr.oper;
        nettype = attr.nettype;
        apn = attr.apn;
        flable = attr.flable;
    }
}; // GeneralAttribute
#pragma pack()

struct IPHumanMessage {
    std::string m_country;
    std::string m_province;
    std::string m_city;
    std::string m_district;
    std::string m_extend;
    std::string m_oper;
    std::string m_nettype;
    std::string m_apn;
    std::string m_flable;
 
    std::string ToString() const {
        std::ostringstream ostrs;
#define PRINT_MSG(type) ostrs << " "#type << "=" << m_##type
        PRINT_MSG(country);
        PRINT_MSG(province);
        PRINT_MSG(city);
        PRINT_MSG(district);
        PRINT_MSG(extend);
        PRINT_MSG(oper);
        PRINT_MSG(nettype);
        PRINT_MSG(apn);
        PRINT_MSG(flable);
#undef PRINT_MSG
        return ostrs.str();
    }
}; // struct IPHumanMessage

typedef vector<newIPPro::IPProperty> vIPProperty;

#pragma pack(1)
struct CodeAttribute
{
	uint32_t id; 
	string content;
    
	CodeAttribute& operator=(const CodeAttribute& right)
	{
		if(this == &right)
			return *this;
		
		id	 	= right.id;
		content	= right.content;
		
		return *this;	
	}

	bool operator==(const CodeAttribute& right) const
	{
		return id == right.id && content == right.content;
	}

	bool operator<(const CodeAttribute& right) const
	{
		if(id < right.id)
		{
			return true;
		}
		else if(id > right.id)
		{
			return false;
		}
		else
		{
			return content < right.content;
		}
		
	}
};
#pragma pack()
struct CodeGap
{
	uint64_t id;
	string content;
	bool operator<(const CodeGap& right) const
	{
		return (id < right.id) || (content < right.content);
		
	}
	CodeGap& operator=(const CodeGap& right) 
	{
		if(this == &right)
			return *this;

		id = right.id;
		content = right.content;
		return *this;		
	}

	bool operator==(const CodeGap& right) const
	{
			return   id == right.id && content == right.content;  
	}

	friend ostream& operator<<(ostream& os, const CodeGap& data);
	
};//__attribute__((packed));

inline ostream& operator<<(ostream& os, const CodeGap& data)
{
	return os<<"id="<<data.id<<"|content="<<data.content<<endl;
}



#define INT2BYTE(buffer, val) \
	memcpy(buffer, &val, sizeof(val)); \
	buffer += sizeof(val); \
	++buffer;

#define BYTE2INT(buffer, val) \
	val = *((uint64_t*)buffer); \
	buffer += sizeof(val); \
	++buffer;

#define STRING2BYTE(buffer, str) \
	memcpy(buffer, str.data(), str.size()); \
	buffer += str.size(); \
	++buffer;

#define BYTE2STRING(buffer, str) \
	str = string(buffer); \
	buffer += str.size(); \
	++buffer;

struct CodeGapEntry
{
	char buffer[128];

	bool operator < (const CodeGapEntry& entry) const
	{
		uint64_t id1 = *((uint64_t*)buffer);
		uint64_t id2 = *((uint64_t*)entry.buffer);
		return id1 < id2;
	}
	bool operator == (const CodeGapEntry& entry) const
	{
		uint64_t id1 = *((uint64_t*)buffer);
		uint64_t id2 = *((uint64_t*)entry.buffer);
		return id1 == id2;
	}
	CodeGapEntry& operator = (const CodeGapEntry& entry)
	{
		if (this != &entry)
		{
			memcpy(buffer, entry.buffer, sizeof(buffer));
		}
		return *this;
	}
	void Set(const CodeGap& node)
	{
		memset(buffer, 0, sizeof(buffer));
		char* temp = buffer;
		INT2BYTE(temp, node.id);
		STRING2BYTE(temp, node.content);
	}
	void Get(CodeGap* node)
	{
		char* temp = buffer;
		BYTE2INT(temp, node->id);
		BYTE2STRING(temp, node->content);

	}
};

#endif // _IPCM_COMM_HPP_ 
