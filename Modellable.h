/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
 */

#pragma once

namespace Gui
{
template<typename T_Model>
class Modellable
{
protected:
    T_Model * _model;

public:
    Modellable()
        :_model(0)
    {
    }

    virtual void setModel(T_Model * model)
    {
        _model = model;
    }

    virtual T_Model * getModel()
    {
        return _model;
    }
};
};//namespace