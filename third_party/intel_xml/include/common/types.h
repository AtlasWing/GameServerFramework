/*
 
 *---------------------------------------------------------------------------
 *
 *     Copyright 2007~2008 Intel Corporation All Rights Reserved.
 *     The source code contained or described herein and all documents 
 *     related to the source code ("Material") are owned by Intel 
 *     Corporation or its suppliers or licensors. Title to the Material
 *     remains with Intel Corporation or its suppliers and licensors. 
 *     The Material may contain trade secrets and proprietary and 
 *     confidential information of Intel Corporation and its suppliers
 *     and licensors, and is protected by worldwide copyright and trade
 *     secret laws and treaty provisions. No part of the Material may 
 *     be used, copied, reproduced, modified, published, uploaded, 
 *     posted, transmitted, distributed, or disclosed in any way without
 *     Intel's prior express written permission. 
 *
 *     No license under any patent, copyright, trade secret or other 
 *     intellectual property right is granted to or conferred upon you 
 *     by disclosure or delivery of the Materials, either expressly, by
 *     implication, inducement, estoppel or otherwise. Any license under
 *     such intellectual property rights must be express and approved by 
 *     Intel in writing.
 *
 *---------------------------------------------------------------------------

*/


#ifndef __COM_INTEL_XML_TYPES_H__
#define __COM_INTEL_XML_TYPES_H__

#include <string>
#include "include/common/defines.h"

INTEL_XML_PARSE_XMLDOC_NAMESPACE_BEGIN
class XMLDocument;
INTEL_XML_PARSE_XMLDOC_NAMESPACE_END

INTEL_XML_NAMESPACE_BEGIN

/** @file types.h
 *  Definition of common types
 */

/**
 * Definition of XMLChar
 */
typedef char XMLChar;

/**
 * Definition of XMLInt64
 * Backward compatibility to the MS compiler not supported long long keyword
 */
#ifdef _WIN32
typedef __int64 XMLInt64;
typedef unsigned __int64 XMLUInt64;
#else 
typedef long long XMLInt64;
typedef unsigned long long XMLUInt64;
#endif

/** 
 * The current encoding enumeration
 *
 * The several types of explicit character encodings that can be set as
 * input and output of parser process. According to XML specification, 
 * other encodings are regarded as an unknown encoding. When setting
 * the output encoding with this, the output is encoded by UTF-8.
 */
enum EncodingStyle {
    UTF_8,         ///< Default 8-bit encoding when no XML declaration is present
    ASCII,         ///< The 8-bit encoding: values between 1 and 127
    ISO_8859_1,  ///< The 8-bit encoding: first 256 Unicode characters
    UTF_16_BE,     ///< The 16-bit big endian encoding: byte mark FE FF 
    UTF_16_LE,     ///< The 16-bit little endian encoding: byte mark FF FE
    UTF_16,      ///< UTF_16 encoding: endianness unknown
    UTF_32_BE,     ///< The 32-bit big endian encoding: byte mark 00 00 FE FF 
    UTF_32_LE,     ///< The 32-bit little endian encoding: byte mark FF FE 00 00
    UTF_32       ///< UTF_32 encoding: endianness unknown
} ;

/**
 * Two validation levels of SAXParser.
 *
 * @e WELL_FORMED XML documents: according to 
 * <a href="http://www.w3.org/TR/xml/#sec-well-formed">
 * XML 1.0: 2.1 Well-formed XML Documents</a>, a textual object is a
 * well-formed XML document if it matches the production labeled document,
 * and meets all well-formedness constraints in the specification,
 * and all parsed entities within the document are well-formed.
 *
 * @e NON_VALIDATING parsing process: according to 
 * <a href="http://www.w3.org/TR/xml/#proc-types">
 * XML 1.0: 5.1 Validating and non-Validating Processors</a>,
 * a non-validating parsing process must normalize the attribute values, 
 * replace text of internal entities and supply default values.
 */
enum ValidationLevel {
    WELL_FORMED = 0,        ///< Validation level only supports well-formedness checking 
    DTD_NON_VALIDATING      ///< Validation level supports well-formedness and non-validating checking
};

/**
 * The Intel(R) XML Solution Suite version information
 *
 */
typedef struct {
    int          MajorVersion;    ///< The major number of the current library version
    int          MinorVersion;    ///< The minor number of the current library version
    int          PatchVersion;    ///< The patch number of the current library version
    int          BuildNumber;     ///< The internal build number
    XMLChar *    Platform;        ///< The platform which the current library fit for
} XSSVersion;    

INTEL_XML_NAMESPACE_END

#endif


