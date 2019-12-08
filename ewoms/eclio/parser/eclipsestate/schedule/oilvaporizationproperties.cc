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
#include <ewoms/eclio/parser/eclipsestate/schedule/oilvaporizationproperties.hh>

namespace Ewoms {

    OilVaporizationProperties::OilVaporizationProperties(const size_t numPvtRegionIdx):
         m_vap1(numPvtRegionIdx, -1.0),
         m_vap2(numPvtRegionIdx, -1.0),
         m_maxDRSDT(numPvtRegionIdx, -1.0),
         m_maxDRSDT_allCells(numPvtRegionIdx),
         m_maxDRVDT(numPvtRegionIdx, -1.0)
    {  }

    double OilVaporizationProperties::getMaxDRVDT(const size_t pvtRegionIdx) const{
        if (drvdtActive()){
            return m_maxDRVDT[pvtRegionIdx];
        }else{
            throw std::logic_error("Only valid if DRVDT is active");
        }
    }

    double OilVaporizationProperties::getMaxDRSDT(const size_t pvtRegionIdx) const{
        if (drsdtActive()){
            return m_maxDRSDT[pvtRegionIdx];
        }else{
            throw std::logic_error("Only valid if DRSDT is active");
        }
    }

    bool OilVaporizationProperties::getOption(const size_t pvtRegionIdx) const{
        if (drsdtActive()){
            return m_maxDRSDT_allCells[pvtRegionIdx];
        }else{
            throw std::logic_error("Only valid if DRSDT is active");
        }
    }

    OilVaporizationProperties::OilVaporization OilVaporizationProperties::getType() const{
        return m_type;
    }

    double OilVaporizationProperties::getVap1(const size_t pvtRegionIdx) const{
        if (m_type == OilVaporization::VAPPARS){
            return m_vap1[pvtRegionIdx];
        }else{
            throw std::logic_error("Only valid if type is VAPPARS");
        }
    }

    double OilVaporizationProperties::getVap2(const size_t pvtRegionIdx) const{
        if (m_type == OilVaporization::VAPPARS){
            return m_vap2[pvtRegionIdx];
        }else{
            throw std::logic_error("Only valid if type is VAPPARS");
        }
    }

    void OilVaporizationProperties::updateDRSDT(OilVaporizationProperties& ovp, const std::vector<double>& maximums, const std::vector<std::string>& options){
        ovp.m_type = OilVaporization::DRDT;
        ovp.m_maxDRSDT = maximums;
        for (size_t pvtRegionIdx = 0; pvtRegionIdx < options.size(); ++pvtRegionIdx) {
            if (options[pvtRegionIdx] == "ALL"){
                ovp.m_maxDRSDT_allCells[pvtRegionIdx] = true;
            } else if (options[pvtRegionIdx] == "FREE") {
                ovp.m_maxDRSDT_allCells[pvtRegionIdx] = false;
            } else {
                throw std::invalid_argument("Only ALL or FREE is allowed as option string");
            }
        }
    }

    void OilVaporizationProperties::updateDRVDT(OilVaporizationProperties& ovp, const std::vector<double>& maximums){
        ovp.m_type = OilVaporization::DRDT;
        ovp.m_maxDRVDT = maximums;
    }

    void OilVaporizationProperties::updateVAPPARS(OilVaporizationProperties& ovp, const std::vector<double>& vap1, const std::vector<double>& vap2){
        ovp.m_type = OilVaporization::VAPPARS;
        ovp.m_vap1 = vap1;
        ovp.m_vap2 = vap2;
    }

    bool OilVaporizationProperties::defined() const {
        return this->m_type != OilVaporization::UNDEF;
    }

    bool OilVaporizationProperties::drsdtActive() const {
        return (m_maxDRSDT[0] >= 0 && m_type == OilVaporization::DRDT);
    }

    bool OilVaporizationProperties::drvdtActive() const {
        return (m_maxDRVDT[0] >= 0 && m_type == OilVaporization::DRDT);
    }

    bool OilVaporizationProperties::operator==( const OilVaporizationProperties& rhs ) const {
        if( this->m_type == OilVaporization::UNDEF
         || rhs.m_type   == OilVaporization::UNDEF
         || this->m_type != rhs.m_type ) return false;

        switch( this->m_type ) {
            case OilVaporization::DRDT:
                return this->m_maxDRSDT == rhs.m_maxDRSDT
                    && this->m_maxDRSDT_allCells == rhs.m_maxDRSDT_allCells
                        && this->m_maxDRVDT == rhs.m_maxDRVDT;

            case OilVaporization::VAPPARS:
                return this->m_vap1 == rhs.m_vap1
                    && this->m_vap2 == rhs.m_vap2;

            default:
                throw std::logic_error( "UNDEF Oil vaporization property; this should never happen" );
        }
    }

    bool OilVaporizationProperties::operator!=( const OilVaporizationProperties& rhs ) const {
        return !(*this == rhs);
    }
}
