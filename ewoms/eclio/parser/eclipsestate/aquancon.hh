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

#ifndef EWOMS_AQUANCON_H
#define EWOMS_AQUANCON_H

/*
  Aquancon is a data container object meant to hold the data from the AQUANCON keyword.
  This also includes the logic for parsing and connections to grid cells. It is meant to be used by ewoms-eclgrids and ewoms-eclsimulators in order to
  implement the analytical aquifer models in eflow.
*/

#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/parserkeywords/a.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>

namespace Ewoms {
    namespace{
        struct AquanconRecord;
        }

    class Aquancon {
        public:

            struct AquanconOutput{
                int aquiferID;
                std::vector<size_t> global_index;
                std::vector<std::shared_ptr<double>> influx_coeff; // Size = size(global_index)
                std::vector<double> influx_multiplier; // Size = size(global_index)
                std::vector<int> reservoir_face_dir; // Size = size(global_index)
                std::vector<int> record_index;
            };

            Aquancon(const EclipseGrid& grid, const Deck& deck);

            const std::vector<Aquancon::AquanconOutput>& getAquOutput() const;

        private:

            static std::vector<Aquancon::AquanconOutput> logic_application(const std::vector<Aquancon::AquanconOutput>& original_vector);

            static void collate_function(std::vector<Aquancon::AquanconOutput>& output_vector,
                                  std::vector<Ewoms::AquanconRecord>& m_aqurecord,
                                  const std::vector<int>& m_aquiferID_per_record, int m_maxAquID);

            static void convert_record_id_to_aquifer_id(std::vector<int>& record_indices_matching_id, int i,
                                                        const std::vector<int>& m_aquiferID_per_record);

            // for a cell to be inside reservoir, its indices need to be within the reservoir grid dimension range,
            // and it needs to be active
            static bool cellInsideReservoirAndActive(const EclipseGrid& grid, int i, int j, int k);

            static bool neighborCellInsideReservoirAndActive(const EclipseGrid& grid, int i, int j, int k, FaceDir::DirEnum faceDir);

            std::vector<Aquancon::AquanconOutput> m_aquoutput;
    };
}

#endif
