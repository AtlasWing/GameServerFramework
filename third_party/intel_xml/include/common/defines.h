/*
 
 *---------------------------------------------------------------------------
 *
 *     Copyright 2007 Intel Corporation All Rights Reserved.
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

/** @file defines.h @brief Defines in the API */


#ifndef __COM_INTEL_XML_DEFINES_H__
#define __COM_INTEL_XML_DEFINES_H__


#define INTEL_XML_NAMESPACE_BEGIN   namespace com{ namespace intel{ namespace xml{
#define INTEL_XML_NAMESPACE_END     } } }
#define INTEL_XML_NAMESPACE_USE     using namespace com::intel::xml;
#define INTEL_XML_NAMESPACE         com::intel::xml
INTEL_XML_NAMESPACE_BEGIN
INTEL_XML_NAMESPACE_END
namespace ixml = INTEL_XML_NAMESPACE;

#define INTEL_XML_UTIL_NAMESPACE_BEGIN  namespace com{ namespace intel{ namespace xml{ namespace util{
#define INTEL_XML_UTIL_NAMESPACE_END    } } } }
#define INTEL_XML_UTIL_NAMESPACE_USE    using namespace com::intel::xml::util;
#define INTEL_XML_UTIL_NAMESPACE        com::intel::xml::util
INTEL_XML_UTIL_NAMESPACE_BEGIN
INTEL_XML_UTIL_NAMESPACE_END
namespace iutil = INTEL_XML_UTIL_NAMESPACE;

#define INTEL_XML_PARSE_SAX_NAMESPACE_BEGIN namespace com{ namespace intel{ namespace xml{ namespace parse{ namespace sax {
#define INTEL_XML_PARSE_SAX_NAMESPACE_END    } } } } }
#define INTEL_XML_PARSE_SAX_NAMESPACE_USE   using namespace com::intel::xml::parse::sax;
#define INTEL_XML_PARSE_SAX_NAMESPACE       com::intel::xml::parse::sax
INTEL_XML_PARSE_SAX_NAMESPACE_BEGIN
INTEL_XML_PARSE_SAX_NAMESPACE_END
namespace isax = INTEL_XML_PARSE_SAX_NAMESPACE;

#define INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN namespace com{ namespace intel{ namespace xml{ namespace parse{ namespace dom {
#define INTEL_XML_PARSE_DOM_NAMESPACE_END    } } } } }
#define INTEL_XML_PARSE_DOM_NAMESPACE_USE   using namespace com::intel::xml::parse::dom;
#define INTEL_XML_PARSE_DOM_NAMESPACE       com::intel::xml::parse::dom
INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN
INTEL_XML_PARSE_DOM_NAMESPACE_END
namespace idom = INTEL_XML_PARSE_DOM_NAMESPACE;

#define INTEL_XML_PARSE_XMLDOC_NAMESPACE_BEGIN  namespace com{ namespace intel{ namespace xml{ namespace parse{ namespace xmldoc {
#define INTEL_XML_PARSE_XMLDOC_NAMESPACE_END    } } } } }
#define INTEL_XML_PARSE_XMLDOC_NAMESPACE_USE    using namespace com::intel::xml::parse::xmldoc;
#define INTEL_XML_PARSE_XMLDOC_NAMESPACE        com::intel::xml::parse::xmldoc
INTEL_XML_PARSE_XMLDOC_NAMESPACE_BEGIN
INTEL_XML_PARSE_XMLDOC_NAMESPACE_END
namespace idoc = INTEL_XML_PARSE_XMLDOC_NAMESPACE;

#define INTEL_XML_VALIDATOR_NAMESPACE_BEGIN namespace com{ namespace intel{ namespace xml{ namespace validator{
#define INTEL_XML_VALIDATOR_NAMESPACE_END    } } } }
#define INTEL_XML_VALIDATOR_NAMESPACE_USE   using namespace com::intel::xml::validator;
#define INTEL_XML_VALIDATOR_NAMESPACE       com::intel::xml::validator
INTEL_XML_VALIDATOR_NAMESPACE_BEGIN
INTEL_XML_VALIDATOR_NAMESPACE_END
namespace ival = INTEL_XML_VALIDATOR_NAMESPACE;

#define INTEL_XML_TRANSFORM_NAMESPACE_BEGIN namespace com{ namespace intel{ namespace xml{ namespace transform{
#define INTEL_XML_TRANSFORM_NAMESPACE_END   } } } }
#define INTEL_XML_TRANSFORM_NAMESPACE_USE   using namespace com::intel::xml::transform;
#define INTEL_XML_TRANSFORM_NAMESPACE       com::intel::xml::transform
INTEL_XML_TRANSFORM_NAMESPACE_BEGIN
INTEL_XML_TRANSFORM_NAMESPACE_END
namespace ixslt = INTEL_XML_TRANSFORM_NAMESPACE;

#define INTEL_XML_XPATH_NAMESPACE_BEGIN namespace com{ namespace intel{ namespace xml{ namespace xpath{
#define INTEL_XML_XPATH_NAMESPACE_END   } } } }
#define INTEL_XML_XPATH_NAMESPACE_USE   using namespace com::intel::xml::xpath;
#define INTEL_XML_XPATH_NAMESPACE       com::intel::xml::xpath
INTEL_XML_XPATH_NAMESPACE_BEGIN
INTEL_XML_XPATH_NAMESPACE_END
namespace ixpath = INTEL_XML_XPATH_NAMESPACE;

#define INTEL_XML_PACKAGE_NAMESPACE_USE_ALL         \
            INTEL_XML_NAMESPACE_USE                 \
            INTEL_XML_UTIL_NAMESPACE_USE            \
            INTEL_XML_PARSE_DOM_NAMESPACE_USE       \
            INTEL_XML_PARSE_SAX_NAMESPACE_USE       \
            INTEL_XML_PARSE_XMLDOC_NAMESPACE_USE    \
            INTEL_XML_VALIDATOR_NAMESPACE_USE       \
            INTEL_XML_TRANSFORM_NAMESPACE_USE       \
            INTEL_XML_XPATH_NAMESPACE_USE   


#if defined (_WIN32)||defined(__WIN32__)||defined(WIN32)||defined(WIN64)
#   if defined(INTEL_XSS_BUILD_DLL)
#       define INTEL_XSS_DECL __declspec(dllexport)
#   elif defined (INTEL_XSS_BUILD_LIB)
#       define INTEL_XSS_DECL
#   elif defined(INTEL_XSS_USE_LIB) 
#       define INTEL_XSS_DECL
#   else
#       define INTEL_XSS_DECL __declspec(dllimport)
#   endif
#else
#   define INTEL_XSS_DECL
#endif


#define COMMON_EXPORT INTEL_XSS_DECL
#define UTIL_EXPORT INTEL_XSS_DECL
#define SAX_EXPORT INTEL_XSS_DECL
#define DOM_EXPORT INTEL_XSS_DECL
#define XMLDOC_EXPORT INTEL_XSS_DECL
#define VALIDATION_EXPORT INTEL_XSS_DECL
#define XSLT_EXPORT INTEL_XSS_DECL
#define XPATH_EXPORT INTEL_XSS_DECL


#endif // __COM_INTEL_XML_DEFINES_H__

