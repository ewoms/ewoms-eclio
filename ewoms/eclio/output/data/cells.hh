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
#ifndef EWOMS_OUTPUT_CELLS_H
#define EWOMS_OUTPUT_CELLS_H

#include <map>
#include <vector>

#include <ewoms/eclio/parser/units/unitsystem.hh>

namespace Ewoms {

namespace data {

    /*
      The 3D data which are saved to file are assembled in one large
      container. In the container the data is tagged with an element
      from the TargetType enum which indicates why they they have been
      added to the container - and where they are headed.

     RESTART_SOLUTION : Cell-based quantities that are output to the
       SOLUTION section of the restart file.  ECLIPSE-compatible names.
       Many, but not necessarily all, of these quantities are required
       for restarting the simulator.

     RESTART_AUXILIARY : Fields with extra information, not required
       for restart. Examples of this include fluid in place values or
       evaluations of relative permeability. Will end up in the
       restart file.

     SUMMARY : Fields which are added only to serve as input data for
       calculations of summary results. The Summary implementation can
       use data with any tag value, but if it is tagged as SUMMARY it
       will not be output anywhere else.

     INIT : Fields which should go to the INIT file.

     RESTART_EWOMS_EXTENDED: Cell-based quantities that are specific to
       EFlow.  Output only to extended OPM restart files.  Specifically
       not output to ECLIPSE-compatible restart files.
    */

    enum class TargetType {
        RESTART_SOLUTION,
        RESTART_AUXILIARY,
        SUMMARY,
        INIT,
        RESTART_EWOMS_EXTENDED,
    };

    /**
     * Small struct that keeps track of data for output to restart/summary files.
     */
    struct CellData {
        UnitSystem::measure dim;   //< Dimension of the data to write
        std::vector<double> data;  //< The actual data itself
        TargetType target;

        bool operator==(const CellData& cell2) const
        {
            return dim == cell2.dim &&
                   data == cell2.data &&
                   target == cell2.target;
        }
    };

}
}

#endif //EWOMS_OUTPUT_CELLS_H
