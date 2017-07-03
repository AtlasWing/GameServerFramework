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

/** @file documentfragment.h @brief Definition of the Document fragment as
 * a minimal Document object
 *
 * This file describes DocumentFragment objects that can hold
 * portions of a document tree. Storing fragments of a document tree in a
 * Document object can work, though is might be too heavyweight
 * depending on the underlying implementation. Instead, a lighter
 * DocumentFragment is used.
 * Working with fragments of a document tree can also be useful in operations
 * that can take DocumentFragment objects as arguments, such as 
 * inserting nodes as children of other Node objects. This moves 
 * all child nodes of the DocumentFragment to the child
 * list of this node.
 */


#ifndef __COM_INTEL_XML_PARSE_DOM_DOCUMENTFRAGMENT_H__
#define __COM_INTEL_XML_PARSE_DOM_DOCUMENTFRAGMENT_H__

#include "include/xpa/dom/node.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

/**
 * Represents a lightweight or minimal Document object.
 * Children of a DocumentFragment node are zero or more
 * nodes representing the tops of sub-trees defining the structure of the
 * document. 
 *
 * DocumentFragment nodes may not be well-formed XML
 * documents, but they must follow the rules imposed upon well-formed XML
 * parsed entities, which can have multiple top nodes. For example, a 
 * structure of a DocumentFragment node with one child Text node
 * does not represent an HTML document or a well-formed XML document.
 *
 * When a DocumentFragment object is inserted into a Document or
 * another Node that can take children, the child nodes of the 
 * DocumentFragment object and not the object itself are inserted into
 * the Node. This makes the DocumentFragment object very useful when you
 * need to create sibling nodes: the DocumentFragment node acts as
 * the parent of these nodes, and you can use the standard methods from the 
 * Node interface, such as insertBefore() and appendChild().
 *
 * Since DOM Level 1
 */
class DOM_EXPORT DocumentFragment : public Node
{
public:
    /**
     * Destructor
     *
     */
    virtual ~DocumentFragment(){}

};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif
