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
#ifndef EWOMS_COUNTERLOG_H
#define EWOMS_COUNTERLOG_H

#include <string>
#include <memory>
#include <map>

#include <ewoms/eclio/opmlog/logbackend.hh>

namespace Ewoms {
/*!
 * \brief Provides a simple sytem for log message which are found by the
 *        Parser/Deck/EclipseState classes during processing the deck.
 */
    class CounterLog : public LogBackend
    {
    public:
        explicit CounterLog(int64_t messageMask);
        CounterLog();

        size_t numMessages(int64_t messageType) const;

        void clear();

    protected:
        void addMessageUnconditionally(int64_t messageFlag,
                                       const std::string& message) override;
    private:
        std::map<int64_t , size_t> m_count;
    };

} // namespace Ewoms

#endif
