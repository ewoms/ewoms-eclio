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
#ifndef EWOMS_PARSER_FAULT_FACE_H
#define EWOMS_PARSER_FAULT_FACE_H

#include <cstddef>
#include <vector>

#include <ewoms/eclio/parser/eclipsestate/grid/facedir.hh>

namespace Ewoms {

class FaultFace {
public:
    FaultFace() = default;
    FaultFace(size_t nx , size_t ny , size_t nz,
              size_t I1 , size_t I2,
              size_t J1 , size_t J2,
              size_t K1 , size_t K2,
              FaceDir::DirEnum faceDir);
    FaultFace(const std::vector<size_t>& indices, FaceDir::DirEnum faceDir);

    std::vector<size_t>::const_iterator begin() const;
    std::vector<size_t>::const_iterator end() const;
    FaceDir::DirEnum getDir() const;
    const std::vector<size_t>& getIndices() const;

    bool operator==( const FaultFace& rhs ) const;
    bool operator!=( const FaultFace& rhs ) const;

private:
    static void checkCoord(size_t dim , size_t l1 , size_t l2);
    FaceDir::DirEnum m_faceDir = FaceDir::XPlus;
    std::vector<size_t> m_indexList;
};

}

#endif // EWOMS_PARSER_FAULT_FACE_H
