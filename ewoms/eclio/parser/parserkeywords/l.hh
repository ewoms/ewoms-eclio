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
#ifndef PARSER_KEYWORDS_L_HH
#define PARSER_KEYWORDS_L_HH
#include <ewoms/eclio/parser/parserkeyword.hh>
namespace Ewoms {
namespace ParserKeywords {

   class LAB : public ParserKeyword {
   public:
       LAB();
       static const std::string keywordName;
   };



   class LANGMPL : public ParserKeyword {
   public:
       LANGMPL();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LANGMUIR : public ParserKeyword {
   public:
       LANGMUIR();
       static const std::string keywordName;

       class DATA {
       public:
           static const std::string itemName;
       };
   };



   class LANGSOLV : public ParserKeyword {
   public:
       LANGSOLV();
       static const std::string keywordName;

       class DATA {
       public:
           static const std::string itemName;
       };
   };



   class LCUNIT : public ParserKeyword {
   public:
       LCUNIT();
       static const std::string keywordName;

       class UNIT {
       public:
           static const std::string itemName;
           static const std::string defaultValue;
       };
   };



   class LGR : public ParserKeyword {
   public:
       LGR();
       static const std::string keywordName;

       class MAXLGR {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MAXCLS {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MCOARS {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MAMALG {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class MXLALG {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class LSTACK {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };

       class INTERP {
       public:
           static const std::string itemName;
           static const std::string defaultValue;
       };

       class NCHCOR {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };
   };



   class LGRCOPY : public ParserKeyword {
   public:
       LGRCOPY();
       static const std::string keywordName;
   };



   class LGRFREE : public ParserKeyword {
   public:
       LGRFREE();
       static const std::string keywordName;

       class LOCAL_GRID_REFINMENT {
       public:
           static const std::string itemName;
       };
   };



   class LGRLOCK : public ParserKeyword {
   public:
       LGRLOCK();
       static const std::string keywordName;

       class LOCAL_GRID_REFINMENT {
       public:
           static const std::string itemName;
       };
   };



   class LGROFF : public ParserKeyword {
   public:
       LGROFF();
       static const std::string keywordName;

       class LOCAL_GRID_REFINMENT {
       public:
           static const std::string itemName;
       };

       class ACTIVE_WELLS {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };
   };



   class LGRON : public ParserKeyword {
   public:
       LGRON();
       static const std::string keywordName;

       class LOCAL_GRID_REFINMENT {
       public:
           static const std::string itemName;
       };

       class ACTIVE_WELLS {
       public:
           static const std::string itemName;
           static const int defaultValue;
       };
   };



   class LICENSE : public ParserKeyword {
   public:
       LICENSE();
       static const std::string keywordName;

       class FEATURE {
       public:
           static const std::string itemName;
       };
   };



   class LIFTOPT : public ParserKeyword {
   public:
       LIFTOPT();
       static const std::string keywordName;

       class INCREMENT_SIZE {
       public:
           static const std::string itemName;
       };

       class MIN_ECONOMIC_GRADIENT {
       public:
           static const std::string itemName;
       };

       class MIN_INTERVAL_BETWEEN_GAS_LIFT_OPTIMIZATIONS {
       public:
           static const std::string itemName;
           static const double defaultValue;
       };

       class OPTIMISE_GAS_LIFT {
       public:
           static const std::string itemName;
           static const std::string defaultValue;
       };
   };



   class LINCOM : public ParserKeyword {
   public:
       LINCOM();
       static const std::string keywordName;

       class ALPHA {
       public:
           static const std::string itemName;
           static const UDAValue defaultValue;
       };

       class BETA {
       public:
           static const std::string itemName;
           static const UDAValue defaultValue;
       };

       class GAMMA {
       public:
           static const std::string itemName;
           static const UDAValue defaultValue;
       };
   };



   class LINKPERM : public ParserKeyword {
   public:
       LINKPERM();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LIVEOIL : public ParserKeyword {
   public:
       LIVEOIL();
       static const std::string keywordName;
   };



   class LKRO : public ParserKeyword {
   public:
       LKRO();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LKRORG : public ParserKeyword {
   public:
       LKRORG();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LKRORW : public ParserKeyword {
   public:
       LKRORW();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LKRW : public ParserKeyword {
   public:
       LKRW();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LKRWR : public ParserKeyword {
   public:
       LKRWR();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LOAD : public ParserKeyword {
   public:
       LOAD();
       static const std::string keywordName;

       class FILE {
       public:
           static const std::string itemName;
       };

       class REPORT_STEP {
       public:
           static const std::string itemName;
       };

       class NOSIM {
       public:
           static const std::string itemName;
           static const std::string defaultValue;
       };

       class FORMATTED {
       public:
           static const std::string itemName;
           static const std::string defaultValue;
       };

       class REQUEST_SAVE_OUTPUT {
       public:
           static const std::string itemName;
           static const std::string defaultValue;
       };
   };



   class LOWSALT : public ParserKeyword {
   public:
       LOWSALT();
       static const std::string keywordName;
   };



   class LPCW : public ParserKeyword {
   public:
       LPCW();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LSALTFNC : public ParserKeyword {
   public:
       LSALTFNC();
       static const std::string keywordName;

       class DATA {
       public:
           static const std::string itemName;
       };
   };



   class LSLTWNUM : public ParserKeyword {
   public:
       LSLTWNUM();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LSNUM : public ParserKeyword {
   public:
       LSNUM();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LSOGCR : public ParserKeyword {
   public:
       LSOGCR();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LSOWCR : public ParserKeyword {
   public:
       LSOWCR();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LSWCR : public ParserKeyword {
   public:
       LSWCR();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LSWL : public ParserKeyword {
   public:
       LSWL();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LSWLPC : public ParserKeyword {
   public:
       LSWLPC();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LSWU : public ParserKeyword {
   public:
       LSWU();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LTOSIGMA : public ParserKeyword {
   public:
       LTOSIGMA();
       static const std::string keywordName;

       class FX {
       public:
           static const std::string itemName;
           static const double defaultValue;
       };

       class FY {
       public:
           static const std::string itemName;
           static const double defaultValue;
       };

       class FZ {
       public:
           static const std::string itemName;
           static const double defaultValue;
       };

       class FGD {
       public:
           static const std::string itemName;
           static const double defaultValue;
       };

       class OPTION {
       public:
           static const std::string itemName;
           static const std::string defaultValue;
       };
   };



   class LWKRO : public ParserKeyword {
   public:
       LWKRO();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LWKRORG : public ParserKeyword {
   public:
       LWKRORG();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LWKRORW : public ParserKeyword {
   public:
       LWKRORW();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LWKRW : public ParserKeyword {
   public:
       LWKRW();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LWKRWR : public ParserKeyword {
   public:
       LWKRWR();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LWPCW : public ParserKeyword {
   public:
       LWPCW();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LWSLTNUM : public ParserKeyword {
   public:
       LWSLTNUM();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LWSNUM : public ParserKeyword {
   public:
       LWSNUM();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LWSOGCR : public ParserKeyword {
   public:
       LWSOGCR();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LWSOWCR : public ParserKeyword {
   public:
       LWSOWCR();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LWSWCR : public ParserKeyword {
   public:
       LWSWCR();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LWSWL : public ParserKeyword {
   public:
       LWSWL();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LWSWLPC : public ParserKeyword {
   public:
       LWSWLPC();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LWSWU : public ParserKeyword {
   public:
       LWSWU();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LX : public ParserKeyword {
   public:
       LX();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LXFIN : public ParserKeyword {
   public:
       LXFIN();
       static const std::string keywordName;

       class CELL_THICKNESS {
       public:
           static const std::string itemName;
       };

       class SIZE_OPTION {
       public:
           static const std::string itemName;
       };
   };



   class LY : public ParserKeyword {
   public:
       LY();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LYFIN : public ParserKeyword {
   public:
       LYFIN();
       static const std::string keywordName;

       class CELL_THICKNESS {
       public:
           static const std::string itemName;
       };

       class SIZE_OPTION {
       public:
           static const std::string itemName;
       };
   };



   class LZ : public ParserKeyword {
   public:
       LZ();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class LZFIN : public ParserKeyword {
   public:
       LZFIN();
       static const std::string keywordName;

       class CELL_THICKNESS {
       public:
           static const std::string itemName;
       };

       class SIZE_OPTION {
       public:
           static const std::string itemName;
       };
   };



}
}
#endif
