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
#ifndef ECLIPSEPRTLOG_H
#define ECLIPSEPRTLOG_H

#include <map>
#include <string>
#include <ewoms/eclio/opmlog/streamlog.hh>

namespace Ewoms {

class EclipsePRTLog : public StreamLog {

public:
    using StreamLog::StreamLog;

    size_t numMessages(int64_t messageType) const;

    ~EclipsePRTLog();

    /// \brief Construct a logger to the <MODLE>.PRT file
    /// \param logFile The name of the logfile to use.
    /// \param messageMask ????
    /// \param append If true then we append messages to the file.
    ///               Otherwise a new file is created.
    /// \param print_summary If true print a summary to the PRT file.
    EclipsePRTLog(const std::string& logFile , int64_t messageMask,
                  bool append, bool print_summary);

    /// \brief Construct a logger to the <MODLE>.PRT file
    /// \param logFile The name of the logfile to use.
    /// \param messageMask ????
    /// \param append If true then we append messages to the file.
    ///               Otherwise a new file is created.
    /// \param print_summary If true print a summary to the PRT file.
    EclipsePRTLog(std::ostream& os , int64_t messageMask,
                  bool print_summary);

protected:
    void addMessageUnconditionally(int64_t messageType, const std::string& message) override;

private:
    std::map<int64_t, size_t> m_count;
    /// \brief Whether to print a summary to the log file.
    bool print_summary_ = true;
};
}
#endif // ECLIPSEPRTLOG_H
