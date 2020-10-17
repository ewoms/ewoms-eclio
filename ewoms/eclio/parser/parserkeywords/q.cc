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
// THIS IS AN AUTOMATICALLY GENERATED FILE! DO NOT EDIT IT MANUALLY!
#include "config.h"

#include <ewoms/eclio/parser/deck/udavalue.hh>
#include <ewoms/eclio/parser/parseritem.hh>
#include <ewoms/eclio/parser/parserrecord.hh>
#include <ewoms/eclio/parser/parser.hh>





#include <ewoms/eclio/parser/parserkeywords/q.hh>
namespace Ewoms {
namespace ParserKeywords {
QDRILL::QDRILL( ) : ParserKeyword("QDRILL")
{
  setFixedSize( (size_t) 1);
  addValidSectionName("SCHEDULE");
  clearDeckNames();
  addDeckName("QDRILL");
  {
     ParserRecord record;
     {
        ParserItem item("WELL_NAME", ParserItem::itype::STRING);
        item.setSizeType(ParserItem::item_size::ALL);
        record.addItem(item);
     }
     addRecord( record );
  }
}
const std::string QDRILL::keywordName = "QDRILL";
const std::string QDRILL::WELL_NAME::itemName = "WELL_NAME";


QHRATING::QHRATING( ) : ParserKeyword("QHRATING")
{
  setSizeType(OTHER_KEYWORD_IN_DECK);
  initSizeKeyword("RIVRDIMS","NRATTA",0);
  addValidSectionName("PROPS");
  clearDeckNames();
  addDeckName("QHRATING");
  {
     ParserRecord record;
     {
        ParserItem item("DATA", ParserItem::itype::DOUBLE);
        item.setSizeType(ParserItem::item_size::ALL);
        item.push_backDimension("Length*Length*Length/Time");
        item.push_backDimension("Length");
        record.addItem(item);
     }
     addRecord( record );
  }
}
const std::string QHRATING::keywordName = "QHRATING";
const std::string QHRATING::DATA::itemName = "DATA";


QMOBIL::QMOBIL( ) : ParserKeyword("QMOBIL")
{
  setFixedSize( (size_t) 1);
  addValidSectionName("GRID");
  clearDeckNames();
  addDeckName("QMOBIL");
  {
     ParserRecord record;
     {
        ParserItem item("MOBILE_END_POINT_CORRECTION", ParserItem::itype::STRING);
        record.addItem(item);
     }
     addRecord( record );
  }
}
const std::string QMOBIL::keywordName = "QMOBIL";
const std::string QMOBIL::MOBILE_END_POINT_CORRECTION::itemName = "MOBILE_END_POINT_CORRECTION";


}
}
