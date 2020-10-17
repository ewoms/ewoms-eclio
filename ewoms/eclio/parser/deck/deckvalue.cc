// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*
  This file is part of the eWoms project.

  eWoms is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  eWoms is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with eWoms.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "config.h"

#include <stdexcept>
#include <iostream>

#include <ewoms/eclio/parser/deck/deckvalue.hh>

namespace Ewoms {

DeckValue::DeckValue():
    default_value(true),
    value_enum(type_tag::unknown)
{}

DeckValue::DeckValue(int value):
    default_value(false),
    value_enum(type_tag::integer),
    int_value(value)
{}

DeckValue::DeckValue(double value):
    default_value(false),
    value_enum(type_tag::fdouble),
    double_value(value)
{}

DeckValue::DeckValue(const std::string& value):
    default_value(false),
    value_enum(type_tag::string),
    string_value(value)
{}

bool DeckValue::is_default() const {
    return default_value;
}

template<>
int DeckValue::get() const {
    if (value_enum == type_tag::integer)
        return this->int_value;

    throw std::invalid_argument("DeckValue does not hold an integer value");
}

template<>
double DeckValue::get() const {
    if (value_enum == type_tag::fdouble)
        return this->double_value;

    if (value_enum == type_tag::integer)
        return this->int_value;

    throw std::invalid_argument("DeckValue does not hold a double value");
}

template<>
std::string DeckValue::get() const {
    if (value_enum == type_tag::string)
        return this->string_value;

    throw std::invalid_argument("DeckValue does not hold a string value");
}

template<>
bool DeckValue::is_compatible<int>() const {
    return (value_enum == type_tag::integer);
}

template<>
bool DeckValue::is_compatible<double>() const {
    return (value_enum == type_tag::fdouble || value_enum == type_tag::integer);
}

template<>
bool DeckValue::is_compatible<std::string>() const {
    return (value_enum == type_tag::string);
}

}

