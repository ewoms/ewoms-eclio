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

#ifndef EWOMS_MESSAGES_H
#define EWOMS_MESSAGES_H

#include <ewoms/eclio/parser/eclipsestate/schedule/dynamicstate.hh>
#include <ewoms/eclio/parser/parserkeywords/m.hh>

namespace Ewoms {

    class TimeMap;

    struct MLimits {
        int message_print_limit = ParserKeywords::MESSAGES::MESSAGE_PRINT_LIMIT::defaultValue;
        int comment_print_limit = ParserKeywords::MESSAGES::COMMENT_PRINT_LIMIT::defaultValue;
        int warning_print_limit = ParserKeywords::MESSAGES::WARNING_PRINT_LIMIT::defaultValue;
        int problem_print_limit = ParserKeywords::MESSAGES::PROBLEM_PRINT_LIMIT::defaultValue;
        int error_print_limit   = ParserKeywords::MESSAGES::ERROR_PRINT_LIMIT::defaultValue;
        int bug_print_limit     = ParserKeywords::MESSAGES::BUG_PRINT_LIMIT::defaultValue;
        int message_stop_limit  = ParserKeywords::MESSAGES::MESSAGE_STOP_LIMIT::defaultValue;
        int comment_stop_limit  = ParserKeywords::MESSAGES::COMMENT_STOP_LIMIT::defaultValue;
        int warning_stop_limit  = ParserKeywords::MESSAGES::WARNING_STOP_LIMIT::defaultValue;
        int problem_stop_limit  = ParserKeywords::MESSAGES::PROBLEM_STOP_LIMIT::defaultValue;
        int error_stop_limit    = ParserKeywords::MESSAGES::ERROR_STOP_LIMIT::defaultValue;
        int bug_stop_limit      = ParserKeywords::MESSAGES::BUG_STOP_LIMIT::defaultValue;

        bool operator==(const MLimits& other) const {
            return  ((this->message_print_limit == other.message_print_limit) &&
                     (this->comment_print_limit == other.comment_print_limit) &&
                     (this->warning_print_limit == other.warning_print_limit) &&
                     (this->problem_print_limit == other.problem_print_limit) &&
                     (this->error_print_limit   == other.error_print_limit  ) &&
                     (this->bug_print_limit     == other.bug_print_limit    ) &&
                     (this->message_stop_limit  == other.message_stop_limit ) &&
                     (this->comment_stop_limit  == other.comment_stop_limit ) &&
                     (this->warning_stop_limit  == other.warning_stop_limit ) &&
                     (this->problem_stop_limit  == other.problem_stop_limit ) &&
                     (this->error_stop_limit    == other.error_stop_limit   ) &&
                     (this->bug_stop_limit      == other.bug_stop_limit     ));
        }

        bool operator!=(const MLimits& other) const {
            return !(*this == other);
        }

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer(message_print_limit);
            serializer(comment_print_limit);
            serializer(warning_print_limit);
            serializer(problem_print_limit);
            serializer(error_print_limit);
            serializer(bug_print_limit);
            serializer(message_stop_limit);
            serializer(comment_stop_limit);
            serializer(warning_stop_limit);
            serializer(problem_stop_limit);
            serializer(error_stop_limit);
            serializer(bug_stop_limit);
        }
    };

    class MessageLimits {
    public:

        MessageLimits() = default;
        /*
           This constructor will create a new Messages object which is
           a copy of the input argument, and then all items explicitly
           set in the record are modified.
        */

        explicit MessageLimits( const TimeMap& );
        explicit MessageLimits( const DynamicState<MLimits>& );

        ///Get all the value from MESSAGES keyword.
        int getMessagePrintLimit(size_t timestep) const;
        int getCommentPrintLimit(size_t timestep) const;
        int getWarningPrintLimit(size_t timestep) const;
        int getProblemPrintLimit(size_t timestep) const;
        int getErrorPrintLimit(size_t timestep) const;
        int getBugPrintLimit(size_t timestep) const;
        void setMessagePrintLimit(size_t timestep, int value);
        void setCommentPrintLimit(size_t timestep, int value);
        void setWarningPrintLimit(size_t timestep, int value);
        void setProblemPrintLimit(size_t timestep, int value);
        void setErrorPrintLimit(size_t timestep, int value);
        void setBugPrintLimit(size_t timestep, int value);

        int getMessageStopLimit(size_t timestep) const;
        int getCommentStopLimit(size_t timestep) const;
        int getWarningStopLimit(size_t timestep) const;
        int getProblemStopLimit(size_t timestep) const;
        int getErrorStopLimit(size_t timestep) const;
        int getBugStopLimit(size_t timestep) const;
        void setMessageStopLimit(size_t timestep, int value);
        void setCommentStopLimit(size_t timestep, int value);
        void setWarningStopLimit(size_t timestep, int value);
        void setProblemStopLimit(size_t timestep, int value);
        void setErrorStopLimit(size_t timestep, int value);
        void setBugStopLimit(size_t timestep, int value);

        bool operator==(const MessageLimits& data) const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            limits.serializeOp(serializer);
        }

    private:
        void update(size_t timestep, const MLimits& value);

        DynamicState<MLimits> limits;
    };
}

#endif
