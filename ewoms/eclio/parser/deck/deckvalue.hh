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
#ifndef DECK_VALUE_H
#define DECK_VALUE_H

#include <string>

#include <ewoms/eclio/parser/utility/typetools.hh>

namespace Ewoms {

class DeckValue {

    public:
        DeckValue();
        explicit DeckValue(int);
        explicit DeckValue(double);
        explicit DeckValue(const std::string&);

        bool is_default() const;

        template<typename T>
        T get() const;

        template<typename T>
        bool is_compatible() const;

    private:

        bool default_value;
        type_tag value_enum;
        int int_value;
        double double_value;
        std::string string_value;

};

}

#endif
