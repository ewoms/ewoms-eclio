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
#ifndef DYNAMICVECTOR_HH_
#define DYNAMICVECTOR_HH_

#include <stdexcept>
#include <ewoms/eclio/parser/eclipsestate/schedule/timemap.hh>

namespace Ewoms {

    /*
      The DynamicVector<T> class is a thin wrapper around
      std::vector<T> with the following twists:

        - Default-sized to the size of a time map, pre-populated by the default
          value.
    */

    template <class T>
    class DynamicVector {
    public:
        DynamicVector() = default;

        DynamicVector(const TimeMap& timeMap, T defaultValue) :
            m_data( timeMap.size(), defaultValue )
        {}

        explicit DynamicVector(const std::vector<T>& data) :
            m_data(data)
        {}

        const T& operator[](size_t index) const {
            return this->m_data.at( index );
        }

        const T& iget(size_t index) const {
            return (*this)[index];
        }

        T& operator[](size_t index) {
            return this->m_data.at( index );
        }

        void iset(size_t index, T value) {
            (*this)[index] = std::move( value );
        }

        bool operator==(const DynamicVector<T>& data) const {
            return this->m_data == data.m_data;
        }

        template<class Serializer, bool complexType = true>
        void serializeOp(Serializer& serializer)
        {
            serializer.vector(m_data);
        }

    private:
        std::vector<T> m_data;
    };
}

#endif
