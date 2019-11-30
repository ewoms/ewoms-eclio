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

#include <stdexcept>

#include <ewoms/eclio/opmlog/opmlog.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipse3dproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fault.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/faultface.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/faultcollection.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/gridproperty.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/transmult.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/griddims.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/multregtscanner.hh>

namespace Ewoms {

    TransMult::TransMult(const GridDims& dims, const Deck& deck, const Eclipse3DProperties& props) :
        m_nx( dims.getNX()),
        m_ny( dims.getNY()),
        m_nz( dims.getNZ()),
        m_names( { { FaceDir::XPlus,  "MULTX"  },
                   { FaceDir::YPlus,  "MULTY"  },
                   { FaceDir::ZPlus,  "MULTZ"  },
                   { FaceDir::XMinus, "MULTX-" },
                   { FaceDir::YMinus, "MULTY-" },
                   { FaceDir::ZMinus, "MULTZ-" }}),
        m_multregtScanner( props, deck.getKeywordList( "MULTREGT" ))
    {
        EDITSection edit_section(deck);
        if (edit_section.hasKeyword("MULTREGT")) {
            std::string msg =
R"(This deck has the MULTREGT keyword located in the EDIT section. Note that:
      1) The MULTREGT keyword from EDIT section will be applied.
      2) It is recommended to place MULTREGT in the GRID section.)";

            OpmLog::warning(msg);
        }
    }

    void TransMult::assertIJK(size_t i , size_t j , size_t k) const {
        if ((i >= m_nx) || (j >= m_ny) || (k >= m_nz))
            throw std::invalid_argument("Invalid ijk");
    }

    size_t TransMult::getGlobalIndex(size_t i , size_t j , size_t k) const {
        assertIJK(i,j,k);
        return i + j*m_nx + k*m_nx*m_ny;
    }

    double TransMult::getMultiplier(size_t globalIndex,  FaceDir::DirEnum faceDir) const {
        if (globalIndex < m_nx * m_ny * m_nz)
            return getMultiplier__(globalIndex , faceDir);
        else
            throw std::invalid_argument("Invalid global index");
    }

    double TransMult::getMultiplier__(size_t globalIndex,  FaceDir::DirEnum faceDir) const {
        if (hasDirectionProperty( faceDir )) {
            const auto& data = m_trans.at(faceDir).getData();
            return data[globalIndex];
        } else
            return 1.0;
    }

    double TransMult::getMultiplier(size_t i , size_t j , size_t k, FaceDir::DirEnum faceDir) const {
        size_t globalIndex = getGlobalIndex(i,j,k);
        return getMultiplier__( globalIndex , faceDir );
    }

    double TransMult::getRegionMultiplier(size_t globalCellIndex1,  size_t globalCellIndex2, FaceDir::DirEnum faceDir) const {
        return m_multregtScanner.getRegionMultiplier(globalCellIndex1, globalCellIndex2, faceDir);
    }

    bool TransMult::hasDirectionProperty(FaceDir::DirEnum faceDir) const {
        return m_trans.count(faceDir) == 1;
    }

    void TransMult::insertNewProperty(FaceDir::DirEnum faceDir) {
        GridPropertySupportedKeywordInfo<double> kwInfo(m_names[faceDir] , 1.0 , "1");
        GridProperty< double > prop( m_nx, m_ny, m_nz, kwInfo );
        m_trans.emplace( faceDir, std::move( prop ) );
    }

    GridProperty<double>& TransMult::getDirectionProperty(FaceDir::DirEnum faceDir) {
        if (m_trans.count(faceDir) == 0)
            insertNewProperty(faceDir);

        return m_trans.at( faceDir );
    }

    void TransMult::applyMULT(const GridProperty<double>& srcProp, FaceDir::DirEnum faceDir)
    {
        auto& dstProp = getDirectionProperty(faceDir);

        const std::vector<double> &srcData = srcProp.getData();
        for (size_t i = 0; i < srcData.size(); ++i)
            dstProp.multiplyValueAtIndex(i, srcData[i]);
    }

    void TransMult::applyMULTFLT(const Fault& fault) {
        double transMult = fault.getTransMult();

        for( const auto& face : fault ) {
            FaceDir::DirEnum faceDir = face.getDir();
            auto& multProperty = getDirectionProperty(faceDir);

            for( auto globalIndex : face ) {
                multProperty.multiplyValueAtIndex( globalIndex , transMult);
            }
        }
    }

    void TransMult::applyMULTFLT(const FaultCollection& faults) {
        for (size_t faultIndex = 0; faultIndex < faults.size(); faultIndex++) {
            auto& fault = faults.getFault(faultIndex);
            applyMULTFLT(fault);
        }
    }
    }
