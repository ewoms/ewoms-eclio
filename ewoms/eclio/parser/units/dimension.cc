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

#include <ewoms/eclio/parser/units/dimension.hh>

#include <string>
#include <stdexcept>
#include <cmath>

namespace Ewoms {

    Dimension::Dimension() :
        m_name("Dimensionless")
    {
        this->m_SIfactor = 1.0;
        this->m_SIoffset = 0.0;

    }

    Dimension::Dimension(const std::string& name, double SIfactor,
                         double SIoffset, bool sanityCheck)
    {
        for (auto iter = name.begin(); iter != name.end() && sanityCheck; ++iter) {
            if (!isalpha(*iter) && (*iter) != '1')
                throw std::invalid_argument("Invalid dimension name");
        }
        m_name = name;
        m_SIfactor = SIfactor;
        m_SIoffset = SIoffset;
    }

    double Dimension::getSIScaling() const {
        if (!std::isfinite(m_SIfactor))
            throw std::logic_error("The DeckItem contains a field with a context dependent unit. "
                                   "Use getData< double >() and convert the returned value manually!");
        return m_SIfactor;
    }

    double Dimension::getSIScalingRaw() const {
        return m_SIfactor;
    }

    double Dimension::getSIOffset() const {
        return m_SIoffset;
    }

    double Dimension::convertRawToSi(double rawValue) const {
        if (!std::isfinite(m_SIfactor))
            throw std::logic_error("The DeckItem contains a field with a context dependent unit. "
                                   "Use getData< double >() and convert the returned value manually!");

        return rawValue*m_SIfactor + m_SIoffset;
    }

    double Dimension::convertSiToRaw(double siValue) const {
        if (!std::isfinite(m_SIfactor))
            throw std::logic_error("The DeckItem contains a field with a context dependent unit. "
                                   "Use getData< double >() and convert the returned value manually!");

        return (siValue - m_SIoffset)/m_SIfactor;
    }

    const std::string& Dimension::getName() const {
        return m_name;
    }

    // only dimensions with zero offset are compositable...
    bool Dimension::isCompositable() const
    { return m_SIoffset == 0.0; }

    Dimension Dimension::newComposite(const std::string& dim , double SIfactor, double SIoffset) {
        Dimension dimension;
        dimension.m_name = dim;
        dimension.m_SIfactor = SIfactor;
        dimension.m_SIoffset = SIoffset;
        return dimension;
    }

    bool Dimension::equal(const Dimension& other) const {
        return *this == other;
    }

    bool Dimension::operator==( const Dimension& rhs ) const {
        if( this->m_name != rhs.m_name ) return false;
        if( this->m_SIfactor == rhs.m_SIfactor
         && this->m_SIoffset == rhs.m_SIoffset ) return true;

        return std::isnan( this->m_SIfactor ) && std::isnan( rhs.m_SIfactor );
    }

    bool Dimension::operator!=( const Dimension& rhs ) const {
        return !(*this == rhs );
    }
}

