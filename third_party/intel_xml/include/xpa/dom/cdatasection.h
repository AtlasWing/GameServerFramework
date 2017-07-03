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

/** @file cdatasection.h @brief @c CDATA interface description.
 *
 * @c CDATA sections are used to escape blocks of text containing 
 * characters that would otherwise be regarded as markup. The only delimiter
 * that is recognized in a @c CDATA is the @c ]]> string 
 * that ends the section. @c CDATA sections cannot be nested. 
 * Their primary purpose is for including material such as XML fragments, 
 * without needing to escape all the delimiters.
 */


#ifndef __COM_INTEL_XML_PARSE_DOM_CDATASECTION_H__
#define __COM_INTEL_XML_PARSE_DOM_CDATASECTION_H__

#include "include/xpa/dom/node.h"
#include "include/xpa/dom/text.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

/**
 *
 * Inherits the
 * CharacterData interface through the Text interface. Adjacent
 * CDATASection nodes are not merged by using the normalize()
 * method of the Node interface. The text contained in a @c CDATA
 * section is stored in the @c data attribute of the Text node. 
 * See also the <a href='http://www.w3.org/TR/2000/REC-DOM-Level-2-Core-20001113'>
 * Document Object Model (DOM) Level 2 Core Specification</a>.
 *
 * @e Escape characters: 
 * content of a CDATASection object may contain delimiters that need to
 * be escaped outside @c CDATA sections, but because no markup is
 * recognized within a CDATASection, numeric character references
 * cannot be used as an escape mechanism when serializing. 
 * As a result, depending on the character encoding chosen for serialization,
 * some characters outside a @c CDATA section might not get printed,
 * and the parser would fail to produce a well-formed XML document.
 * To resolve this, you can end the @c CDATA section before the
 * character or entity reference that needs to be printed, and open a new @c 
 * CDATA section for the following character data in the Text node. 
 * Please remember that not all code conversion libraries report errors
 * or exceptions when writing in case a character is missing from the
 * encoding. 
 *
 * Since DOM Level 1
 */
class DOM_EXPORT CDATASection : public Text
{
public:
    /** 
     * Destructor 
     */
    virtual ~CDATASection() {}
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif
