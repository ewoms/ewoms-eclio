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

#include <ewoms/eclio/parser/eclipsestate/endpointscaling.hh>

#include <ewoms/eclio/parser/deck/deck.hh>

#include <ewoms/eclio/parser/parserkeywords/e.hh>
#include <ewoms/eclio/parser/parserkeywords/s.hh>

#include <ewoms/common/string.hh>

#include <initializer_list>
#include <stdexcept>
#include <string>

namespace {
    bool hasScaling(const Ewoms::Deck&                          deck,
                    const char*                               suffix,
                    const std::initializer_list<std::string>& base)
    {
        for (const auto& kw : base) {
            for (const auto* p : {"", "I"}) {
                if (deck.hasKeyword(p + kw + suffix)) {
                    return true;
                }
            }
        }

        return false;
    }

    bool hasScaling(const Ewoms::Deck&                          deck,
                    const std::initializer_list<std::string>& base)
    {
        const auto direction = std::initializer_list<const char*> {
            "", "X-", "X", "Y-", "Y", "Z-", "Z"
        };

        for (const auto* suffix : direction) {
            if (hasScaling(deck, suffix, base)) {
                return true;
            }
        }

        return false;
    }

    bool hasHorzScaling(const Ewoms::Deck& deck)
    {
        return hasScaling(deck, {
            Ewoms::ParserKeywords::SGL  ::keywordName,
            Ewoms::ParserKeywords::SGCR ::keywordName,
            Ewoms::ParserKeywords::SGU  ::keywordName,
            Ewoms::ParserKeywords::SOGCR::keywordName,
            Ewoms::ParserKeywords::SOWCR::keywordName,
            Ewoms::ParserKeywords::SWL  ::keywordName,
            Ewoms::ParserKeywords::SWCR ::keywordName,
            Ewoms::ParserKeywords::SWU  ::keywordName,
        })
        || hasScaling(deck, "", {
            Ewoms::ParserKeywords::SGLPC::keywordName,
            Ewoms::ParserKeywords::SWLPC::keywordName,
        });
    }

    bool hasVertScaling(const Ewoms::Deck& deck)
    {
        return hasScaling(deck, {
            std::string { "KRG"   },
            std::string { "KRGR"  },
            std::string { "KRORG" },
            std::string { "KRORW" },
            std::string { "KRW"   },
            std::string { "KRWR"  },
        });
    }
}

namespace Ewoms {

EndpointScaling EndpointScaling::serializeObject()
{
    EndpointScaling result;
    result.options = std::bitset<4>{13};

    return result;
}

EndpointScaling::operator bool() const noexcept {
    return this->options[ static_cast< ue >( option::any ) ];
}

bool EndpointScaling::directional() const noexcept {
    return this->options[ static_cast< ue >( option::directional ) ];
}

bool EndpointScaling::nondirectional() const noexcept {
    return bool( *this ) && !this->directional();
}

bool EndpointScaling::reversible() const noexcept {
    return this->options[ static_cast< ue >( option::reversible ) ];
}

bool EndpointScaling::irreversible() const noexcept {
    return bool( *this ) && !this->reversible();
}

bool EndpointScaling::twopoint() const noexcept {
    return bool( *this ) && !this->threepoint();
}

bool EndpointScaling::threepoint() const noexcept {
    return this->options[ static_cast< ue >( option::threepoint ) ];
}

bool EndpointScaling::operator==(const EndpointScaling& data) const {
    return options == data.options;
}

namespace {

bool threepoint_scaling( const Deck& deck ) {
    using ScaleCRS = ParserKeywords::SCALECRS;

    if (! deck.hasKeyword<ScaleCRS>())
        return false;

    /*
     * the manual says that Y and N are acceptable values for "YES" and "NO", so
     * it's *VERY* likely that only the first character is checked. We preserve
     * this behaviour
     */
    const auto value = std::toupper(
        deck.getKeyword<ScaleCRS>()
            .getRecord(0)
            .getItem<ScaleCRS::VALUE>()
            .get<std::string>(0).front());

    if (value != 'Y' && value != 'N')
        throw std::invalid_argument {
            ScaleCRS::keywordName + " takes 'YES' or 'NO'"
        };

    return value == 'Y';
}

bool endscale_nodir( const DeckKeyword& kw ) {
    if( kw.getRecord( 0 ).getItem( 0 ).defaultApplied( 0 ) )
        return true;

    const auto& value = uppercase( kw.getRecord( 0 )
                                     .getItem( 0 )
                                     .get< std::string >( 0 ) );

    if( value != "DIRECT" && value != "NODIR" )
        throw std::invalid_argument(
            "ENDSCALE argument 1 must be defaulted, 'DIRECT' or 'NODIR', was "
            + value
        );

    return value == "NODIR";
}

bool endscale_revers( const DeckKeyword& kw ) {
    if( kw.getRecord( 0 ).getItem( 1 ).defaultApplied( 0 ) )
        return true;

    const auto& value = uppercase( kw.getRecord( 0 )
                                     .getItem( 1 )
                                     .get< std::string >( 0 ) );

    if( value != "IRREVERS" && value != "REVERS" )
        throw std::invalid_argument(
            "ENDSCALE argument 2 must be defaulted, 'REVERS' or 'IRREVERS', was "
            + value
        );

    const auto& item0 = kw.getRecord( 0 ).getItem( 0 );
    if( value == "IRREVERS"
     && uppercase( item0.get< std::string >( 0 ) ) != "DIRECT" )
        throw std::invalid_argument( "'IRREVERS' requires 'DIRECT'" );

    return value == "REVERS";
}

}

EndpointScaling::EndpointScaling( const Deck& deck ) {
    const auto has_horz_scaling = hasHorzScaling(deck);
    const auto has_vert_scaling = hasVertScaling(deck);
    const auto has_endscale     = deck.hasKeyword<ParserKeywords::ENDSCALE>();

    if (has_horz_scaling || has_vert_scaling || has_endscale ||
        deck.hasKeyword<ParserKeywords::SWATINIT>())
    {
        const bool threep_ = threepoint_scaling( deck );
        bool direct_ = false;
        bool reversible_ = true;

        if (has_endscale) {
            const auto& endscale = deck.getKeyword<ParserKeywords::ENDSCALE>();
            direct_ = !endscale_nodir( endscale );
            reversible_ = endscale_revers( endscale );
        }

        this->options.set( static_cast< ue >( option::any ), true );
        this->options.set( static_cast< ue >( option::directional ), direct_ );
        this->options.set( static_cast< ue >( option::reversible ), reversible_ );
        this->options.set( static_cast< ue >( option::threepoint ), threep_ );
    }
}

}
