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

#include <iostream>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/deck/decksection.hh>
#include <ewoms/eclio/parser/eclipsestate/initconfig/initconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/initconfig/equil.hh>

#include <ewoms/eclio/parser/parserkeywords/e.hh>
#include <ewoms/eclio/parser/parserkeywords/r.hh>
#include <ewoms/eclio/parser/parserkeywords/s.hh>

namespace Ewoms {

    static inline Equil equils( const Deck& deck ) {
        if( !deck.hasKeyword<ParserKeywords::EQUIL>( ) ) return {};
        return Equil( deck.getKeyword<ParserKeywords::EQUIL>(  ) );
    }

    InitConfig::InitConfig()
        : m_filleps(false)
    {
    }

    InitConfig::InitConfig(const Deck& deck)
        : equil(equils(deck))
        , foamconfig(deck)
        , m_filleps(PROPSSection{deck}.hasKeyword("FILLEPS"))
    {
        if( !deck.hasKeyword( "RESTART" ) ) {
            if( deck.hasKeyword( "SKIPREST" ) ) {
                std::cout << "Deck has SKIPREST, but no RESTART. "
                          << "Ignoring SKIPREST." << std::endl;
            }

            return;
        }

        const auto& record = deck.getKeyword( "RESTART" ).getRecord(0);
        const auto& save_item = record.getItem(2);

        if( save_item.hasValue( 0 ) ) {
            throw std::runtime_error(
                    "eWoms does not support RESTART from a SAVE file, "
                    "only from RESTART files");
        }

        int step = record.getItem( 1 ).get< int >(0);
        const std::string& root = record.getItem( 0 ).get< std::string >( 0 );
        const std::string& input_path = deck.getInputPath();

        if (root[0] == '/' || input_path.empty())
            this->setRestart(root, step);
        else
            this->setRestart( input_path + "/" + root, step );
    }

    InitConfig::InitConfig(const Equil& equils, const FoamConfig& foam,
                           bool filleps, bool restartReq, int restartStep,
                           const std::string& restartRootName)
        : equil(equils)
        , foamconfig(foam)
        , m_filleps(filleps)
        , m_restartRequested(restartReq)
        , m_restartStep(restartStep)
        , m_restartRootName(restartRootName)
    {
    }

    void InitConfig::setRestart( const std::string& root, int step) {
        m_restartRequested = true;
        m_restartStep = step;
        m_restartRootName = root;
    }

    bool InitConfig::restartRequested() const {
        return m_restartRequested;
    }

    int InitConfig::getRestartStep() const {
        return m_restartStep;
    }

    const std::string& InitConfig::getRestartRootName() const {
        return m_restartRootName;
    }

    bool InitConfig::hasEquil() const {
        return !this->equil.empty();
    }

    const Equil& InitConfig::getEquil() const {
        if( !this->hasEquil() )
            throw std::runtime_error( "Error: No 'EQUIL' present" );

        return this->equil;
    }

    bool InitConfig::hasFoamConfig() const {
        // return !this->foamconfig.empty();
        return true;
    }

    const FoamConfig& InitConfig::getFoamConfig() const {
        if( !this->hasFoamConfig() )
            throw std::runtime_error( "Error: No foam model configuration keywords present" );

        return this->foamconfig;
    }

    bool InitConfig::operator==(const InitConfig& data) const {
        return equil == data.equil &&
               foamconfig == data.foamconfig &&
               m_filleps == data.m_filleps &&
               m_restartRequested == data.m_restartRequested &&
               m_restartStep == data.m_restartStep &&
               m_restartRootName == data.m_restartRootName;
    }

} //namespace Ewoms
