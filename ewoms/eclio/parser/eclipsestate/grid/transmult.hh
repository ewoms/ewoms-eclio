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

/**
   This class implements a small container which holds the
   transmissibility mulitpliers for all the faces in the grid. The
   multipliers in this class are built up from the transmissibility
   modifier keywords:

      {MULTX , MULTX- , MULTY , MULTY- , MULTZ , MULTZ-, MULTFLT , MULTREGT}

*/
#ifndef EWOMS_PARSER_TRANSMULT_H
#define EWOMS_PARSER_TRANSMULT_H

#include <cstddef>
#include <map>
#include <memory>

#include <ewoms/eclio/parser/eclipsestate/grid/facedir.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/multregtscanner.hh>

namespace Ewoms {
    template< typename > class GridProperty;
    class Fault;
    class FaultCollection;
    class DeckKeyword;
    class FieldPropsManager;

    class TransMult {

    public:
        TransMult(const GridDims& dims, const Deck& deck, const FieldPropsManager& fp);
        double getMultiplier(size_t globalIndex, FaceDir::DirEnum faceDir) const;
        double getMultiplier(size_t i , size_t j , size_t k, FaceDir::DirEnum faceDir) const;
        double getRegionMultiplier( size_t globalCellIndex1, size_t globalCellIndex2, FaceDir::DirEnum faceDir) const;
        void applyMULT(const std::vector<double>& srcMultProp, FaceDir::DirEnum faceDir);
        void applyMULTFLT(const FaultCollection& faults);
        void applyMULTFLT(const Fault& fault);

    private:
        size_t getGlobalIndex(size_t i , size_t j , size_t k) const;
        void assertIJK(size_t i , size_t j , size_t k) const;
        double getMultiplier__(size_t globalIndex , FaceDir::DirEnum faceDir) const;
        bool hasDirectionProperty(FaceDir::DirEnum faceDir) const;
        std::vector<double>& getDirectionProperty(FaceDir::DirEnum faceDir);

        size_t m_nx , m_ny , m_nz;
        std::map<FaceDir::DirEnum , std::vector<double> > m_trans;
        std::map<FaceDir::DirEnum , std::string> m_names;
        MULTREGTScanner m_multregtScanner;
    };

}

#endif // EWOMS_PARSER_TRANSMULT_H
