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
#include <cstddef>

#include <ewoms/eclio/parser/eclipsestate/schedule/events.hh>

namespace Ewoms {

    Events::Events(const TimeMap& timeMap) :
        m_events( DynamicVector<uint64_t>( timeMap , 0 ) )
    { }

    Events::Events(const DynamicVector<uint64_t>& events) :
        m_events(events)
    { }

    bool Events::hasEvent(uint64_t eventMask , size_t reportStep) const {
        uint64_t eventSum = m_events[reportStep];
        if (eventSum & eventMask)
            return true;
        else
            return false;
    }

    void Events::addEvent(ScheduleEvents::Events event, size_t reportStep) {
        m_events[reportStep] |= event;
    }

    const DynamicVector<uint64_t>& Events::events() const {
        return m_events;
    }

    bool Events::operator==(const Events& data) const {
        return this->events().data() == data.events().data();
    }

}
