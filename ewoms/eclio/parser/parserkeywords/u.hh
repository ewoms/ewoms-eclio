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
#ifndef PARSER_KEYWORDS_U_HH
#define PARSER_KEYWORDS_U_HH
#include <ewoms/eclio/parser/parserkeyword.hh>
namespace Ewoms {
namespace ParserKeywords {

   class UDADIMS : public ParserKeyword {
   public:
       UDADIMS();
       static const std::string keywordName;

       class NUM_UDQ_REPLACE {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class IGNORED {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class TOTAL_UDQ_UNIQUE {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };
   };



   class UDQ : public ParserKeyword {
   public:
       UDQ();
       static const std::string keywordName;

       class ACTION {
       public:
           static const std::string itemName;
       };

       class QUANTITY {
       public:
           static const std::string itemName;
       };

       class DATA {
       public:
           static const std::string itemName;
       };
   };



   class UDQDIMS : public ParserKeyword {
   public:
       UDQDIMS();
       static const std::string keywordName;

       class MAX_FUNCTIONS {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MAX_ITEMS {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MAX_CONNECTIONS {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MAX_FIELDS {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MAX_GROUP {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MAX_REGION {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MAX_SEGMENT {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MAX_WELL {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MAX_AQUIFER {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MAX_BLOCK {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class RESTART_NEW_SEED {
       public:
           static const std::string itemName;
           static const std::string defaultValue;
       };
   };



   class UDQPARAM : public ParserKeyword {
   public:
       UDQPARAM();
       static const std::string keywordName;

       class RANDOM_SEED {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class RANGE {
       public:
           static const std::string itemName;
           static const double defaultValue;
       };

       class UNDEFINED_VALUE {
       public:
           static const std::string itemName;
           static const double defaultValue;
       };

       class CMP_EPSILON {
       public:
           static const std::string itemName;
           static const double defaultValue;
       };
   };



   class UDT : public ParserKeyword {
   public:
       UDT();
       static const std::string keywordName;
   };



   class UDTDIMS : public ParserKeyword {
   public:
       UDTDIMS();
       static const std::string keywordName;

       class MAX_TABLES {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MAX_ROWS {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MAX_INTERPOLATION_POINTS {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MAX_DIMENSIONS {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };
   };



   class UNCODHMD : public ParserKeyword {
   public:
       UNCODHMD();
       static const std::string keywordName;
   };



   class UNIFIN : public ParserKeyword {
   public:
       UNIFIN();
       static const std::string keywordName;
   };



   class UNIFOUT : public ParserKeyword {
   public:
       UNIFOUT();
       static const std::string keywordName;
   };



   class UNIFOUTS : public ParserKeyword {
   public:
       UNIFOUTS();
       static const std::string keywordName;
   };



   class UNIFSAVE : public ParserKeyword {
   public:
       UNIFSAVE();
       static const std::string keywordName;
   };



   class USECUPL : public ParserKeyword {
   public:
       USECUPL();
       static const std::string keywordName;

       class BASE {
       public:
           static const std::string itemName;
       };

       class FMT {
       public:
           static const std::string itemName;
       };
   };



   class USEFLUX : public ParserKeyword {
   public:
       USEFLUX();
       static const std::string keywordName;
   };



   class USENOFLO : public ParserKeyword {
   public:
       USENOFLO();
       static const std::string keywordName;
   };



}
}
#endif
