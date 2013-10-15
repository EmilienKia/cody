/* -*- Mode: C++; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * template-tools.hpp.h
 *
 * Copyright (C) 2013 Émilien KIA <emilien.kia@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _TEMPLATE_TOOLS_HPP_
#define _TEMPLATE_TOOLS_HPP_

template <typename T>
class Optional
{
public:
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T& const_pointer;

    Optional():_set(false) {}
    Optional(const T& elem):_set(true),_elem(elem) {}
    Optional(const Optional<T>& elem):_set(elem._set),_elem(elem._elem) {}

    bool set()const
    {
        return _set;
    }
    void set(const T& elem)
    {
        _elem=elem;
        _set=true;
    }
    void clear()
    {
        _set=false;
        _elem = T();
    }

    T& operator=(const T& elem)
    {
        _elem=elem;
        _set=true;
        return *this;
    }
    Optional<T>& operator=(const Optional<T>& elem)
    {
        _elem=elem._elem;
        _set=elem._set;
        return *this;
    }

    bool operator!()const
    {
        return !_set;
    }
    operator bool()const
    {
        return _set;
    }

    T& operator*()
    {
        return _elem;
    }
    const T& operator*()const
    {
        return _elem;
    }

    T* operator -> ()
    {
        return &_elem;
    }
    const T* operator -> ()const
    {
        return &_elem;
    }

    operator T& ()
    {
        return _elem;
    }
    operator const T& ()const
    {
        return _elem;
    }

private:
    bool _set;
    T    _elem;
};

#endif // _TEMPLATE_TOOLS_HPP_