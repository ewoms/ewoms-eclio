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
#ifndef PARSER_KEYWORDS_J_HH
#define PARSER_KEYWORDS_J_HH
#include <ewoms/eclio/parser/parserkeyword.hh>
namespace Ewoms {
namespace ParserKeywords {

   class JFUNC : public ParserKeyword {
   public:
       JFUNC();
       static const std::string keywordName;

       class FLAG {
       public:
           static const std::string itemName;
           static const std::string defaultValue;
       };

       class OW_SURFACE_TENSION {
       public:
           static const std::string itemName;
           static const double defaultValue;
       };

       class GO_SURFACE_TENSION {
       public:
           static const std::string itemName;
           static const double defaultValue;
       };

       class ALPHA_FACTOR {
       public:
           static const std::string itemName;
           static const double defaultValue;
       };

       class BETA_FACTOR {
       public:
           static const std::string itemName;
           static const double defaultValue;
       };

       class DIRECTION {
       public:
           static const std::string itemName;
           static const std::string defaultValue;
       };
   };



   class JFUNCR : public ParserKeyword {
   public:
       JFUNCR();
       static const std::string keywordName;

       class J_FUNCTION {
       public:
           static const std::string itemName;
           static const std::string defaultValue;
       };

       class OIL_WAT_SURF_TENSTION {
       public:
           static const std::string itemName;
       };

       class OIL_GAS_SURF_TENSTION {
       public:
           static const std::string itemName;
       };

       class POROSITY_POWER {
       public:
           static const std::string itemName;
           static const double defaultValue;
       };

       class PERMEABILITY_POWER {
       public:
           static const std::string itemName;
           static const double defaultValue;
       };

       class PERM_DIRECTION {
       public:
           static const std::string itemName;
           static const std::string defaultValue;
       };
   };



}
}
#endif
