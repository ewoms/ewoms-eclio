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

#ifndef EWOMS_TRESHOLD_PRESSURES_H
#define EWOMS_TRESHOLD_PRESSURES_H

#include <map>
#include <vector>

namespace Ewoms {

    class Deck;
    class Eclipse3DProperties;
    class FieldPropsManager;

    class ThresholdPressure {

    public:
        ThresholdPressure(bool restart,
                          const Deck& deck,
                          const FieldPropsManager& fp,
                          const Eclipse3DProperties& eclipseProperties);

        /*
          The hasRegionBarrier() method checks if a threshold pressure
          has been configured between the equilibration regions r1 and
          r2; i.e. if the deck contains a THPRES record with regions
          r1 and r2.
        */
        bool   hasRegionBarrier(int r1 , int r2) const;

        /*
          Checks if a threshold presssure has been configured between
          the equilibration regions r1 and r2; the function will
          return false either if no THPRES record with r1 and r2 has
          been configured - or if THPRES record with ra and r2 has
          defaulted pressure.
        */
        bool   hasThresholdPressure(int r1 , int r2) const;

        /*
          Will return the threshold pressure between equilibration
          regions r1 and r2; if the pressure has been defaulted the
          function will raise the error
          INTERNAL_ERROR_UNINITIALIZED_THPRES - check with
          hasThresholdPressure(r1,r2) first to be safe.
        */
        double getThresholdPressure(int r1 , int r2) const;
        size_t size() const;
        bool active() const;
        bool restart() const;
    private:
        bool m_active;
        bool m_restart;
        static std::pair<int,int> makeIndex(int r1 , int r2);
        void addPair(int r1 , int r2 , const std::pair<bool , double>& valuePair);
        void addBarrier(int r1 , int r2);
        void addBarrier(int r1 , int r2 , double p);

        std::vector<std::pair<bool,double>> m_thresholdPressureTable;
        std::map<std::pair<int,int> , std::pair<bool , double> > m_pressureTable;
    };
} //namespace Ewoms

#endif
