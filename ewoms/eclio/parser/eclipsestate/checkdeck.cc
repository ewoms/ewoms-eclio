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

#include <ewoms/eclio/parser/eclipsestate/checkdeck.hh>

#include <ewoms/eclio/opmlog/opmlog.hh>
#include <ewoms/eclio/opmlog/logutil.hh>

#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/decksection.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/parsecontext.hh>
#include <ewoms/eclio/parser/errorguard.hh>
#include <ewoms/common/string.hh>

namespace Ewoms {
bool checkDeck( const Deck& deck, const Parser& parser, const ParseContext& parseContext, ErrorGuard& errorGuard, size_t enabledChecks) {
    bool deckValid = true;

    // make sure that the deck does not contain unknown keywords
    if (enabledChecks & UnknownKeywords) {
        size_t keywordIdx = 0;
        for (; keywordIdx < deck.size(); keywordIdx++) {
            const auto& keyword = deck.getKeyword(keywordIdx);
            if (!parser.isRecognizedKeyword( keyword.name() ) ) {
                std::string msg("Keyword '" + keyword.name() + "' is unknown.");
                const auto& location = keyword.location();
                OpmLog::warning( Log::fileMessage(location, msg) );
                deckValid = false;
            }
        }
    }

    // make sure all mandatory sections are present and that their order is correct
    if (enabledChecks & SectionTopology) {
        bool ensureKeywordSection = enabledChecks & KeywordSection;
        deckValid = deckValid && DeckSection::checkSectionTopology(deck, parser, ensureKeywordSection);
    }

    const std::string& deckUnitSystem = uppercase(deck.getActiveUnitSystem().getName());
    for (const auto& keyword : deck.getKeywordList("FILEUNIT")) {
        const std::string& fileUnitSystem = uppercase(keyword->getRecord(0).getItem("FILE_UNIT_SYSTEM").getTrimmedString(0));
        if (fileUnitSystem != deckUnitSystem) {
            const auto& location = keyword->location();
            std::string msg_fmt = "Unit system mismatch\n"
                                  "In {file} line {line}";
            parseContext.handleError(ParseContext::UNIT_SYSTEM_MISMATCH, msg_fmt, location, errorGuard);
            deckValid = false;
        }
    }

    return deckValid;
}
}
