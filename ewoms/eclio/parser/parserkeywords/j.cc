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





#include <ewoms/eclio/parser/parserkeywords/j.hh>
namespace Ewoms {
namespace ParserKeywords {
JFUNC::JFUNC( ) : ParserKeyword("JFUNC")
{
  setFixedSize( (size_t) 1);
  addValidSectionName("GRID");
  clearDeckNames();
  addDeckName("JFUNC");
  {
     ParserRecord record;
     {
        ParserItem item("FLAG", ParserItem::itype::STRING);
        item.setDefault( std::string("BOTH") );
        record.addItem(item);
     }
     {
        ParserItem item("OW_SURFACE_TENSION", ParserItem::itype::DOUBLE);
        item.setDefault( double(-1.000000) );
        item.push_backDimension("SurfaceTension");
        record.addItem(item);
     }
     {
        ParserItem item("GO_SURFACE_TENSION", ParserItem::itype::DOUBLE);
        item.setDefault( double(-1.000000) );
        item.push_backDimension("SurfaceTension");
        record.addItem(item);
     }
     {
        ParserItem item("ALPHA_FACTOR", ParserItem::itype::DOUBLE);
        item.setDefault( double(0.500000) );
        record.addItem(item);
     }
     {
        ParserItem item("BETA_FACTOR", ParserItem::itype::DOUBLE);
        item.setDefault( double(0.500000) );
        record.addItem(item);
     }
     {
        ParserItem item("DIRECTION", ParserItem::itype::STRING);
        item.setDefault( std::string("XY") );
        record.addItem(item);
     }
     addRecord( record );
  }
}
const std::string JFUNC::keywordName = "JFUNC";
const std::string JFUNC::FLAG::itemName = "FLAG";
const std::string JFUNC::FLAG::defaultValue = "BOTH";
const std::string JFUNC::OW_SURFACE_TENSION::itemName = "OW_SURFACE_TENSION";
const double JFUNC::OW_SURFACE_TENSION::defaultValue = -1.000000;
const std::string JFUNC::GO_SURFACE_TENSION::itemName = "GO_SURFACE_TENSION";
const double JFUNC::GO_SURFACE_TENSION::defaultValue = -1.000000;
const std::string JFUNC::ALPHA_FACTOR::itemName = "ALPHA_FACTOR";
const double JFUNC::ALPHA_FACTOR::defaultValue = 0.500000;
const std::string JFUNC::BETA_FACTOR::itemName = "BETA_FACTOR";
const double JFUNC::BETA_FACTOR::defaultValue = 0.500000;
const std::string JFUNC::DIRECTION::itemName = "DIRECTION";
const std::string JFUNC::DIRECTION::defaultValue = "XY";


JFUNCR::JFUNCR( ) : ParserKeyword("JFUNCR")
{
  setSizeType(OTHER_KEYWORD_IN_DECK);
  initSizeKeyword("TABDIMS","NTSFUN",0);
  addValidSectionName("GRID");
  clearDeckNames();
  addDeckName("JFUNCR");
  {
     ParserRecord record;
     {
        ParserItem item("J_FUNCTION", ParserItem::itype::STRING);
        item.setDefault( std::string("BOTH") );
        record.addItem(item);
     }
     {
        ParserItem item("OIL_WAT_SURF_TENSTION", ParserItem::itype::DOUBLE);
        record.addItem(item);
     }
     {
        ParserItem item("OIL_GAS_SURF_TENSTION", ParserItem::itype::DOUBLE);
        record.addItem(item);
     }
     {
        ParserItem item("POROSITY_POWER", ParserItem::itype::DOUBLE);
        item.setDefault( double(0.500000) );
        item.push_backDimension("1");
        record.addItem(item);
     }
     {
        ParserItem item("PERMEABILITY_POWER", ParserItem::itype::DOUBLE);
        item.setDefault( double(0.500000) );
        item.push_backDimension("1");
        record.addItem(item);
     }
     {
        ParserItem item("PERM_DIRECTION", ParserItem::itype::STRING);
        item.setDefault( std::string("XY") );
        record.addItem(item);
     }
     addRecord( record );
  }
}
const std::string JFUNCR::keywordName = "JFUNCR";
const std::string JFUNCR::J_FUNCTION::itemName = "J_FUNCTION";
const std::string JFUNCR::J_FUNCTION::defaultValue = "BOTH";
const std::string JFUNCR::OIL_WAT_SURF_TENSTION::itemName = "OIL_WAT_SURF_TENSTION";
const std::string JFUNCR::OIL_GAS_SURF_TENSTION::itemName = "OIL_GAS_SURF_TENSTION";
const std::string JFUNCR::POROSITY_POWER::itemName = "POROSITY_POWER";
const double JFUNCR::POROSITY_POWER::defaultValue = 0.500000;
const std::string JFUNCR::PERMEABILITY_POWER::itemName = "PERMEABILITY_POWER";
const double JFUNCR::PERMEABILITY_POWER::defaultValue = 0.500000;
const std::string JFUNCR::PERM_DIRECTION::itemName = "PERM_DIRECTION";
const std::string JFUNCR::PERM_DIRECTION::defaultValue = "XY";


}
}
