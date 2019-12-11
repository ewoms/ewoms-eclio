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

#ifndef EWOMS_OUTPUT_ECLIPSE_VECTOR_INTEHEAD_H
#define EWOMS_OUTPUT_ECLIPSE_VECTOR_INTEHEAD_H

#include <vector>

namespace Ewoms { namespace RestartIO { namespace Helpers { namespace VectorItems {

    // This is a subset of the items in src/ewoms/eclio/output/InteHEAD.cpp .
    // Promote items from that list to this in order to make them public.
    enum intehead : std::vector<int>::size_type {
        ISNUM   =   0,      //  An encoded integer corresponding to the
                            //  time the file was created.  For files not
                            //  originating from ECLIPSE, this value may
                            //  be set to zero.

        VERSION =   1,      //  Simulator version
        UNIT    =   2,      //  Units convention
                            //     1: METRIC, 2: FIELD, 3: LAB, 4: PVT-M

        NX      =   8,      //  #cells in X direction (Cartesian)
        NY      =   9,      //  #cells in Y direction (Cartesian)
        NZ      =  10,      //  #cells in Z direction (Cartesian)
        NACTIV  =  11,      //  Number of active cells

        PHASE   =  14,      //  Phase indicator:
                            //     1: oil, 2: water, 3: O/W, 4: gas,
                            //     5: G/O, 6: G/W, 7: O/G/W

        NWELLS  =  16,      //  Number of wells
        NCWMAX  =  17,      //  Maximum number of completions per well
        NWGMAX  =  19,      //  Maximum number of wells in any well group
        NGMAXZ  =  20,      //  Maximum number of groups in field

        NIWELZ  =  24,      //  Number of data elements per well in IWEL array
                            //  (default 97 for ECLIPSE, 94 for ECLIPSE 300).
        NSWELZ  =  25,      //  Number of data elements per well in SWEL array
        NXWELZ  =  26,      //  Number of delements per well in XWEL array
        NZWELZ  =  27,      //  Number of 8-character words per well in ZWEL array

        NICONZ  =  32,      //  Number of data elements per completion
                            //  in ICON array (default 19)
        NSCONZ  =  33,      //  Number of data elements per completion in SCON array
        NXCONZ  =  34,      //  Number of data elements per completion in XCON array

        NIGRPZ  =  36,      //  Number of data elements per group in IGRP array
        NSGRPZ  =  37,      //  Number of data elements per group in SGRP array
        NXGRPZ  =  38,      //  Number of data elements per group in XGRP array
        NZGRPZ  =  39,      //  Number of data elements per group in ZGRP array

        NCAMAX  =  41,      //  Maximum number of analytic aquifer connections

        NIAAQZ  =  42,      //  Number of data elements per aquifer in IAAQ array
        NSAAQZ  =  43,      //  Number of data elements per aquifer in SAAQ array
        NXAAQZ  =  44,      //  Number of data elements per aquifer in XAAQ array

        NICAQZ  =  45,      //  Number of data elements per aquifer connection in ICAQ array
        NSCAQZ  =  46,      //  Number of data elements per aquifer connection in SCAQ array
        NACAQZ  =  47,      //  Number of data elements per aquifer connection in ACAQ array

        DAY     =  64,      //  Calendar day of report step (1..31)
        MONTH   =  65,      //  Calendar month of report step (1..12)
        YEAR    =  66,      //  Calendar year of report step

        NOOFACTIONS = 156,  //  The number of actions in the dataset
        MAXNOLINES = 157,   //  Maximum number of lines of schedule data for ACTION keyword - including ENDACTIO
        MAXNOSTRPRLINE = 158,   //  Maximum number of 8-chars strings pr input line of Action data (rounded up from input)

        NWMAXZ  = 163,      //  Maximum number of wells in the model

        NSEGWL  = 174,      //  Number of multisegment wells defined with WELSEG
        NSWLMX  = 175,      //  Maximum number of segmented wells (item 1 ofWSEGDIMS)
        NSEGMX  = 176,      //  Maximum number of segments per well (item 2 of WSEGDIMS)
        NLBRMX  = 177,      //  Maximum number of lateral branches (item 3 of WSEGDIMS)

        NISEGZ  = 178,      //  Number of entries per segment in ISEG array
        NRSEGZ  = 179,      //  Number of entries per segment in RSEG array
        NILBRZ  = 180,      //  Number of entries per segment in ILBR array

        MAX_ACT_COND = 245,      //  Maximum number of conditions pr action

        MAX_AN_AQUIFERS = 252, // Maximum number of analytic aquifers

        NO_UDQS = 266,      //  No of UDQ data (parameters)
        UDQPAR_1 = 267,      //  Integer seed value for the RAND
        RSEED    = 296,
    };
}}}} // Ewoms::RestartIO::Helpers::VectorItems

#endif // EWOMS_OUTPUT_ECLIPSE_VECTOR_INTEHEAD_H
