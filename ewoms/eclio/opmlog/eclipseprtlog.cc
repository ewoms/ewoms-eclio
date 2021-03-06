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
#include "config.h"

#include <ewoms/eclio/opmlog/eclipseprtlog.hh>
#include <ewoms/eclio/opmlog/logutil.hh>

namespace Ewoms {

    void EclipsePRTLog::addMessageUnconditionally(int64_t messageType, const std::string& message)
    {
        StreamLog::addMessageUnconditionally(messageType, message);
        m_count[messageType]++;
    }

    size_t EclipsePRTLog::numMessages(int64_t messageType) const
    {
        if (Log::isPower2( messageType )) {
            auto iter = m_count.find( messageType );
            if (iter == m_count.end())
                return 0;
            else
                return (*iter).second;
        } else
            throw std::invalid_argument("The messageType ID must be 2^n");
    }

    EclipsePRTLog::~EclipsePRTLog()
    {
        if( ! print_summary_ )
        {
            return;
        }

        //output summary.
        const std::string summary_msg = "\n\nError summary:" +
            std::string("\nWarnings          " + std::to_string(numMessages(Log::MessageType::Warning))) +
            std::string("\nInfo              " + std::to_string(numMessages(Log::MessageType::Info))) +
            std::string("\nErrors            " + std::to_string(numMessages(Log::MessageType::Error))) +
            std::string("\nBugs              " + std::to_string(numMessages(Log::MessageType::Bug))) +
            std::string("\nDebug             " + std::to_string(numMessages(Log::MessageType::Debug))) +
            std::string("\nProblems          " + std::to_string(numMessages(Log::MessageType::Problem))) +"\n";
        StreamLog::addTaggedMessage(Log::MessageType::Info, "", summary_msg);
    }

    EclipsePRTLog::EclipsePRTLog(const std::string& logFile,
                                 int64_t messageMask,
                                 bool append,
                                 bool print_summary)
        : StreamLog(logFile, messageMask, append),
          print_summary_(print_summary)
    {}

    EclipsePRTLog::EclipsePRTLog(std::ostream& os,
                                 int64_t messageMask,
                                 bool print_summary)
        : StreamLog(os, messageMask), print_summary_(print_summary)
    {}
}
