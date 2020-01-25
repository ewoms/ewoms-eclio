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
#include <ewoms/eclio/parser/deck/decksection.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fieldpropsmanager.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fault.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/faultface.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/faultcollection.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/transmult.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/griddims.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/multregtscanner.hh>

namespace Ewoms {

   TransMult::TransMult(const GridDims& dims, const Deck& deck, const FieldPropsManager& fp) :
        m_nx( dims.getNX()),
        m_ny( dims.getNY()),
        m_nz( dims.getNZ()),
        m_names( { { FaceDir::XPlus,  "MULTX"  },
                   { FaceDir::YPlus,  "MULTY"  },
                   { FaceDir::ZPlus,  "MULTZ"  },
                   { FaceDir::XMinus, "MULTX-" },
                   { FaceDir::YMinus, "MULTY-" },
                   { FaceDir::ZMinus, "MULTZ-" }}),
        m_multregtScanner( dims, &fp, deck.getKeywordList( "MULTREGT" ))
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

    TransMult::TransMult(const std::array<size_t,3>& size,
                         const std::map<FaceDir::DirEnum, std::vector<double>>& trans,
                         const std::map<FaceDir::DirEnum, std::string>& names,
                         const MULTREGTScanner& scanner) :
        m_nx(size[0]),
        m_ny(size[1]),
        m_nz(size[2]),
        m_trans(trans),
        m_names(names),
        m_multregtScanner(scanner)
    {
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
            return this->getMultiplier__(globalIndex , faceDir);
        else
            throw std::invalid_argument("Invalid global index");
    }

    double TransMult::getMultiplier__(size_t globalIndex,  FaceDir::DirEnum faceDir) const {
        if (hasDirectionProperty( faceDir )) {
            const auto& data = m_trans.at(faceDir);
            return data[globalIndex];
        } else
            return 1.0;
    }

    double TransMult::getMultiplier(size_t i , size_t j , size_t k, FaceDir::DirEnum faceDir) const {
        size_t globalIndex = this->getGlobalIndex(i,j,k);
        return getMultiplier__( globalIndex , faceDir );
    }

    double TransMult::getRegionMultiplier(size_t globalCellIndex1,  size_t globalCellIndex2, FaceDir::DirEnum faceDir) const {
        return m_multregtScanner.getRegionMultiplier(globalCellIndex1, globalCellIndex2, faceDir);
    }

    bool TransMult::hasDirectionProperty(FaceDir::DirEnum faceDir) const {
        return m_trans.count(faceDir) == 1;
    }

    std::vector<double>& TransMult::getDirectionProperty(FaceDir::DirEnum faceDir) {
        if (m_trans.count(faceDir) == 0) {
            std::size_t global_size = this->m_nx * this->m_ny * this->m_nz;
            m_trans[faceDir] = std::vector<double>(global_size, 1);
        }

        return m_trans.at( faceDir );
    }

    void TransMult::applyMULT(const std::vector<double>& srcData, FaceDir::DirEnum faceDir)
    {
        auto& dstProp = this->getDirectionProperty(faceDir);
        for (size_t i = 0; i < srcData.size(); ++i)
            dstProp[i] *= srcData[i];
    }

    void TransMult::applyMULTFLT(const Fault& fault) {
        double transMult = fault.getTransMult();

        for( const auto& face : fault ) {
            FaceDir::DirEnum faceDir = face.getDir();
            auto& multProperty = this->getDirectionProperty(faceDir);

            for( auto globalIndex : face )
                multProperty[globalIndex] *= transMult;
        }
    }

    void TransMult::applyMULTFLT(const FaultCollection& faults) {
        for (size_t faultIndex = 0; faultIndex < faults.size(); faultIndex++) {
            auto& fault = faults.getFault(faultIndex);
            this->applyMULTFLT(fault);
        }
    }

    std::array<size_t,3> TransMult::getSize() const {
        return {m_nx, m_ny, m_nz};
    }

    const std::map<FaceDir::DirEnum, std::vector<double>>& TransMult::getTrans() const {
        return m_trans;
    }

    const std::map<FaceDir::DirEnum, std::string>& TransMult::getNames() const {
        return m_names;
    }

    const MULTREGTScanner& TransMult::getScanner() const {
        return m_multregtScanner;
    }

    bool TransMult::operator==(const TransMult& data) const {
        return this->getSize() == data.getSize() &&
               this->getTrans() == data.getTrans() &&
               this->getNames() == data.getNames() &&
               this->getScanner() == data.getScanner();
    }

}
