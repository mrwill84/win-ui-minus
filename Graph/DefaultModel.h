/*
 * Copyright (c) 2006-2007, Andry Korolyuk
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *   * Neither the name of the Andry Korolyuk nor the names of its contributors 
 *     may be used to endorse or promote products derived from this software 
 *     without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include "IModel.h"
#include <vector>

namespace Gui { namespace Graph {

class DefaultModel : public IModel
{
protected:
    std::vector<Node> _nodes;
    std::vector<Link> _links;
public:
    virtual size_t getNodesCount(            ) const { return _nodes.size(); }
    virtual Node   getNode(const size_t index) const { return _nodes.at(index); }
    virtual void   setNode(const size_t index, 
                           const Node & node )
    {
        _nodes[index] = node;
    }

    virtual void   addNode(const Node & node)
    {
        _nodes.push_back(node);
    }
    virtual size_t getLinksCount(void) const { return _links.size();}
    virtual Link   getLink(const size_t index) const { return _links[index];}
    virtual void   selectNode(const size_t index)
    {
    }
    virtual void   shiftSelectedNodeUp   (void    ) {};
    virtual void   shiftSelectedNodeDown (void    ) {};
    virtual void   shiftSelectedNodeLeft (void    ) {};
    virtual void   shiftSelectedNodeRight(void    ) {};

};
}};//namespace