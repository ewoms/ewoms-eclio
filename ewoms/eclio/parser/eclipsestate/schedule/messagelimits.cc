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

#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/parserkeywords/m.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/messagelimits.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>

namespace Ewoms {

    MessageLimits::MessageLimits( const TimeMap& timemap ) :
        limits( timemap , MLimits())
    { }

    MessageLimits::MessageLimits(const DynamicState<MLimits>& limits_) :
        limits(limits_)
    { }

    int MessageLimits::getMessagePrintLimit(size_t timestep) const
    {
        const auto& mlimit = limits.get( timestep );
        return mlimit.message_print_limit;
    }

    int MessageLimits::getCommentPrintLimit(size_t timestep) const
    {
        const auto& mlimit = limits.get( timestep );
        return mlimit.comment_print_limit;
    }

    int MessageLimits::getWarningPrintLimit(size_t timestep) const
    {
        const auto& mlimit = limits.get( timestep );
        return mlimit.warning_print_limit;
    }

    int MessageLimits::getProblemPrintLimit(size_t timestep) const
    {
        const auto& mlimit = limits.get( timestep );
        return mlimit.problem_print_limit;
    }

    int MessageLimits::getErrorPrintLimit(size_t timestep) const
    {
        const auto& mlimit = limits.get( timestep );
        return mlimit.error_print_limit;
    }

    int MessageLimits::getBugPrintLimit(size_t timestep) const
    {
        const auto& mlimit = limits.get( timestep );
        return mlimit.bug_print_limit;
    }

    /*-----------------------------------------------------------------*/

    int MessageLimits::getMessageStopLimit(size_t timestep) const
    {
        const auto& mlimit = limits.get( timestep );
        return mlimit.message_stop_limit;
    }

    int MessageLimits::getCommentStopLimit(size_t timestep) const
    {
        const auto& mlimit = limits.get( timestep );
        return mlimit.comment_stop_limit;
    }

    int MessageLimits::getWarningStopLimit(size_t timestep) const
    {
        const auto& mlimit = limits.get( timestep );
        return mlimit.warning_stop_limit;
    }

    int MessageLimits::getProblemStopLimit(size_t timestep) const
    {
        const auto& mlimit = limits.get( timestep );
        return mlimit.problem_stop_limit;
    }

    int MessageLimits::getErrorStopLimit(size_t timestep) const
    {
        const auto& mlimit = limits.get( timestep );
        return mlimit.error_stop_limit;
    }

    int MessageLimits::getBugStopLimit(size_t timestep) const
    {
        const auto& mlimit = limits.get( timestep );
        return mlimit.bug_stop_limit;
    }

    /*-----------------------------------------------------------------*/

    void MessageLimits::update(size_t timestep, const MLimits& value) {
        if (timestep == 0)
            limits.updateInitial( value );
        else
            limits.update( timestep , value );
    }

    void MessageLimits::setMessagePrintLimit(size_t timestep, int value)
    {
        auto mlimit = limits.get( timestep );
        mlimit.message_print_limit = value;
        this->update( timestep , mlimit );
    }

    void MessageLimits::setCommentPrintLimit(size_t timestep, int value)
    {
        auto mlimit = limits.get( timestep );
        mlimit.comment_print_limit = value;
        this->update( timestep , mlimit );
    }

    void MessageLimits::setWarningPrintLimit(size_t timestep, int value)
    {
        auto mlimit = limits.get( timestep );
        mlimit.warning_print_limit = value;
        this->update( timestep , mlimit );
    }

    void MessageLimits::setProblemPrintLimit(size_t timestep, int value)
    {
        auto mlimit = limits.get( timestep );
        mlimit.problem_print_limit = value;
        this->update( timestep , mlimit );
    }

    void MessageLimits::setErrorPrintLimit(size_t timestep, int value)
    {
        auto mlimit = limits.get( timestep );
        mlimit.error_print_limit = value;
        this->update( timestep , mlimit );
    }

    void MessageLimits::setBugPrintLimit(size_t timestep, int value)
    {
        auto mlimit = limits.get( timestep );
        mlimit.bug_print_limit = value;
        this->update( timestep , mlimit );
    }

    /*-----------------------------------------------------------------*/

    void MessageLimits::setMessageStopLimit(size_t timestep, int value)
    {
        auto mlimit = limits.get( timestep );
        mlimit.message_stop_limit = value;
        this->update( timestep , mlimit );
    }

    void MessageLimits::setCommentStopLimit(size_t timestep, int value)
    {
        auto mlimit = limits.get( timestep );
        mlimit.comment_stop_limit = value;
        this->update( timestep , mlimit );
    }

    void MessageLimits::setWarningStopLimit(size_t timestep, int value)
    {
        auto mlimit = limits.get( timestep );
        mlimit.warning_stop_limit = value;
        this->update( timestep , mlimit );
    }

    void MessageLimits::setProblemStopLimit(size_t timestep, int value)
    {
        auto mlimit = limits.get( timestep );
        mlimit.problem_stop_limit = value;
        this->update( timestep , mlimit );
    }

    void MessageLimits::setErrorStopLimit(size_t timestep, int value)
    {
        auto mlimit = limits.get( timestep );
        mlimit.error_stop_limit = value;
        this->update( timestep , mlimit );
    }

    void MessageLimits::setBugStopLimit(size_t timestep, int value)
    {
        auto mlimit = limits.get( timestep );
        mlimit.bug_stop_limit = value;
        this->update( timestep , mlimit );
    }

    bool MessageLimits::operator==(const MessageLimits& data) const
    {
        return limits == data.limits;
    }

}

