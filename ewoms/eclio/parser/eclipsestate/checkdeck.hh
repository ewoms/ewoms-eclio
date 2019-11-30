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

#ifndef EWOMS_CHECK_DECK_H
#define EWOMS_CHECK_DECK_H

#include <memory>

namespace Ewoms {

    class Deck;
    class Parser;
    class ParseContext;
    class ErrorGuard;

enum DeckChecks {
    SectionTopology = 0x0001,

    // KeywordSection check only has an effect of the SectionTopology test is enabled
    KeywordSection = 0x0002,

    UnknownKeywords = 0x0004,

    AllChecks = 0xffff
};

// some semantical correctness checks of the deck. this method adds a warning to
// the deck object if any issue is found ...
bool checkDeck( const Deck& deck, const Parser& parser, const ParseContext& parseContext, ErrorGuard& errorGuard, size_t enabledChecks  = AllChecks);

}

#endif
