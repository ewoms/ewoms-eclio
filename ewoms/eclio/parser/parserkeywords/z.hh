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
#ifndef PARSER_KEYWORDS_Z_HH
#define PARSER_KEYWORDS_Z_HH
#include <ewoms/eclio/parser/parserkeyword.hh>
namespace Ewoms {
namespace ParserKeywords {

   class ZCORN : public ParserKeyword {
   public:
       ZCORN();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class ZFACT1 : public ParserKeyword {
   public:
       ZFACT1();
       static const std::string keywordName;

       class Z0 {
       public:
           static const std::string itemName;
       };
   };



   class ZFACT1S : public ParserKeyword {
   public:
       ZFACT1S();
       static const std::string keywordName;

       class Z0 {
       public:
           static const std::string itemName;
       };
   };



   class ZFACTOR : public ParserKeyword {
   public:
       ZFACTOR();
       static const std::string keywordName;

       class Z0 {
       public:
           static const std::string itemName;
       };
   };



   class ZFACTORS : public ParserKeyword {
   public:
       ZFACTORS();
       static const std::string keywordName;

       class Z0 {
       public:
           static const std::string itemName;
       };
   };



   class ZIPP2OFF : public ParserKeyword {
   public:
       ZIPP2OFF();
       static const std::string keywordName;
   };



   class ZIPPY2 : public ParserKeyword {
   public:
       ZIPPY2();
       static const std::string keywordName;

       class SETTINGS {
       public:
           static const std::string itemName;
       };
   };



}
}
#endif
