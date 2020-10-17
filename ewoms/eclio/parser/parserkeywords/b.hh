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
#ifndef PARSER_KEYWORDS_B_HH
#define PARSER_KEYWORDS_B_HH
#include <ewoms/eclio/parser/parserkeyword.hh>
namespace Ewoms {
namespace ParserKeywords {

   class BC : public ParserKeyword {
   public:
       BC();
       static const std::string keywordName;

       class I1 {
       public:
           static const std::string itemName;
       };

       class I2 {
       public:
           static const std::string itemName;
       };

       class J1 {
       public:
           static const std::string itemName;
       };

       class J2 {
       public:
           static const std::string itemName;
       };

       class K1 {
       public:
           static const std::string itemName;
       };

       class K2 {
       public:
           static const std::string itemName;
       };

       class TYPE {
       public:
           static const std::string itemName;
       };

       class DIRECTION {
       public:
           static const std::string itemName;
       };

       class COMPONENT {
       public:
           static const std::string itemName;
           static const std::string defaultValue;
       };

       class RATE {
       public:
           static const std::string itemName;
           static const double defaultValue;
       };
   };



   class BDENSITY : public ParserKeyword {
   public:
       BDENSITY();
       static const std::string keywordName;

       class BRINE_DENSITY {
       public:
           static const std::string itemName;
       };
   };



   class BGGI : public ParserKeyword {
   public:
       BGGI();
       static const std::string keywordName;

       class GAS_PRESSURE {
       public:
           static const std::string itemName;
       };

       class DATA {
       public:
           static const std::string itemName;
       };
   };



   class BIGMODEL : public ParserKeyword {
   public:
       BIGMODEL();
       static const std::string keywordName;
   };



   class BLACKOIL : public ParserKeyword {
   public:
       BLACKOIL();
       static const std::string keywordName;
   };



   class BLOCK_PROBE : public ParserKeyword {
   public:
       BLOCK_PROBE();
       static const std::string keywordName;

       class I {
       public:
           static const std::string itemName;
       };

       class J {
       public:
           static const std::string itemName;
       };

       class K {
       public:
           static const std::string itemName;
       };
   };



   class BLOCK_PROBE300 : public ParserKeyword {
   public:
       BLOCK_PROBE300();
       static const std::string keywordName;

       class I {
       public:
           static const std::string itemName;
       };

       class J {
       public:
           static const std::string itemName;
       };

       class K {
       public:
           static const std::string itemName;
       };
   };



   class BOGI : public ParserKeyword {
   public:
       BOGI();
       static const std::string keywordName;

       class OIL_PRESSURE {
       public:
           static const std::string itemName;
       };

       class DATA {
       public:
           static const std::string itemName;
       };
   };



   class BOUNDARY : public ParserKeyword {
   public:
       BOUNDARY();
       static const std::string keywordName;

       class IX1 {
       public:
           static const std::string itemName;
       };

       class IX2 {
       public:
           static const std::string itemName;
       };

       class JY1 {
       public:
           static const std::string itemName;
       };

       class JY2 {
       public:
           static const std::string itemName;
       };

       class KZ1 {
       public:
           static const std::string itemName;
       };

       class KZ2 {
       public:
           static const std::string itemName;
       };

       class ORIENTATION_INDEX {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class DUAL_PORO_FLAG {
       public:
           static const std::string itemName;
           static const std::string defaultValue;
       };
   };



   class BOX : public ParserKeyword {
   public:
       BOX();
       static const std::string keywordName;

       class I1 {
       public:
           static const std::string itemName;
       };

       class I2 {
       public:
           static const std::string itemName;
       };

       class J1 {
       public:
           static const std::string itemName;
       };

       class J2 {
       public:
           static const std::string itemName;
       };

       class K1 {
       public:
           static const std::string itemName;
       };

       class K2 {
       public:
           static const std::string itemName;
       };
   };



   class BPARA : public ParserKeyword {
   public:
       BPARA();
       static const std::string keywordName;
   };



   class BPIDIMS : public ParserKeyword {
   public:
       BPIDIMS();
       static const std::string keywordName;

       class MXNBIP {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MXNLBI {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };
   };



   class BRANPROP : public ParserKeyword {
   public:
       BRANPROP();
       static const std::string keywordName;

       class DOWNTREE_NODE {
       public:
           static const std::string itemName;
       };

       class UPTREE_NODE {
       public:
           static const std::string itemName;
       };

       class VFP_TABLE {
       public:
           static const std::string itemName;
       };

       class ALQ {
       public:
           static const std::string itemName;
           static const double defaultValue;
       };

       class ALQ_SURFACE_DENSITY {
       public:
           static const std::string itemName;
           static const std::string defaultValue;
       };
   };



   class BRINE : public ParserKeyword {
   public:
       BRINE();
       static const std::string keywordName;
   };



   class BTOBALFA : public ParserKeyword {
   public:
       BTOBALFA();
       static const std::string keywordName;

       class VALUE {
       public:
           static const std::string itemName;
       };
   };



   class BTOBALFV : public ParserKeyword {
   public:
       BTOBALFV();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



}
}
#endif
