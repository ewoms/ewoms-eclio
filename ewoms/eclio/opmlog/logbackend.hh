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
#ifndef EWOMS_LOGBACKEND_H
#define EWOMS_LOGBACKEND_H

#include <ewoms/eclio/opmlog/messageformatter.hh>
#include <ewoms/eclio/opmlog/messagelimiter.hh>
#include <cstdint>
#include <string>
#include <memory>

namespace Ewoms
{

    /// Abstract interface class for log backends.
    class LogBackend
    {
    public:
        /// Construct with given message mask.
        explicit LogBackend(int64_t mask);

        /// Virtual destructor to enable inheritance.
        virtual ~LogBackend();

        /// Configure how formatMessage() will modify message strings.
        void setMessageFormatter(std::shared_ptr<MessageFormatterInterface> formatter);

        /// Configure how message tags will be used to limit messages.
        void setMessageLimiter(std::shared_ptr<MessageLimiter> limiter);

        /// Add a message to the backend if accepted by the message limiter.
        void addMessage(int64_t messageFlag, const std::string& message);

        /// Add a tagged message to the backend if accepted by the message limiter.
        void addTaggedMessage(int64_t messageFlag,
                              const std::string& messageTag,
                              const std::string& message);

        /// The message mask types are specified in the
        /// Ewoms::Log::MessageType namespace, in file LogUtils.hpp.
        int64_t getMask() const;

    protected:
        /// This is the method subclasses should override.
        ///
        /// Typically a subclass may filter, change, and output
        /// messages based on configuration and the messageFlag.
        virtual void addMessageUnconditionally(int64_t messageFlag,
                                               const std::string& message) = 0;

        /// Return decorated version of message depending on configureDecoration() arguments.
        std::string formatMessage(int64_t messageFlag, const std::string& message);

    private:
        /// Return true if all bits of messageFlag are also set in our mask,
        /// and the message limiter returns a PrintMessage response.
        bool includeMessage(int64_t messageFlag, const std::string& messageTag);

        int64_t m_mask;
        std::shared_ptr<MessageFormatterInterface> m_formatter;
        std::shared_ptr<MessageLimiter> m_limiter;
    };

} // namespace LogBackend

#endif
