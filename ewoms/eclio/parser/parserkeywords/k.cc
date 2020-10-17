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





#include <ewoms/eclio/parser/parserkeywords/k.hh>
namespace Ewoms {
namespace ParserKeywords {
KRNUM::KRNUM( ) : ParserKeyword("KRNUM")
{
  setFixedSize( (size_t) 1);
  addValidSectionName("REGIONS");
  clearDeckNames();
  addDeckName("KRNUM");
  {
     ParserRecord record;
     {
        ParserItem item("data", ParserItem::itype::INT);
        item.setSizeType(ParserItem::item_size::ALL);
        record.addDataItem(item);
     }
     addDataRecord( record );
  }
}
const std::string KRNUM::keywordName = "KRNUM";
const std::string KRNUM::data::itemName = "data";


KRNUMMF::KRNUMMF( ) : ParserKeyword("KRNUMMF")
{
  setFixedSize( (size_t) 1);
  addValidSectionName("REGIONS");
  clearDeckNames();
  addDeckName("KRNUMMF");
  {
     ParserRecord record;
     {
        ParserItem item("data", ParserItem::itype::INT);
        item.setSizeType(ParserItem::item_size::ALL);
        record.addDataItem(item);
     }
     addDataRecord( record );
  }
}
const std::string KRNUMMF::keywordName = "KRNUMMF";
const std::string KRNUMMF::data::itemName = "data";


}
}
