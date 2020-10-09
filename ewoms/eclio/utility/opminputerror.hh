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
#ifndef EWOMS_ERROR_H
#define EWOMS_ERROR_H

#include <stdexcept>
#include <string>

#include <ewoms/common/optional.hh>

#include <ewoms/eclio/opmlog/keywordlocation.hh>

namespace Ewoms {

/*
  The OpmInputError is a custom exception class which can be used to signal
  errors in input handling. The importance of the OpmInputError exception is
  *not* the tecnical functionality it provides, but rather the convention
  surrounding it, when and how it should be used.

  The OpmInputError should be used in situations which are "close to user
  input", the root cause can either be incorrect user input or a bug/limitation
  in opm. OpmInputError should only be used in situations where we have a good
  understanding of the underlying issue, and can provide a good error message.

  The local error handling should be complete when the OpmInputError is
  instantiated, it should not be caught and rethrown in order to e.g. add
  additional context or log messages. In order to avoid inadvertendly catching
  this exception in a catch all block.
*/

class OpmInputError : public std::exception {
public:
    /*
      The message string will be used as format string in the fmt::format()
      function as, and optional {} markers can be used to inject keyword,
      filename and linenumber into the final what() message. The placeholders
      must use named arguments

        {keyword} -> loc.keyword
        {file} -> loc.filename
        {line} -> loc.lineno

      additionally, the message can contain any number of positional
      arguments to add further context to the message.

      KeywordLocation loc("KW", "file.inc", 100);
      OpmInputError("Error at line {line} in file {file} - keyword: {keyword} ignored", location);
      OpmInputError("Error at line {line} in file {file} - keyword: {keyword} has invalid argument {}", invalid_argument);
    */

    OpmInputError(const std::string& msg_fmt, const KeywordLocation& loc) :
        m_what   { OpmInputError::format(msg_fmt, loc) },
        location { loc }
    {}

    /*
      Allows for the initialisation of an OpmInputError from another exception.

      Usage:

      try {
          .
          .
          .
      } catch (const Ewoms::OpmInputError&) {
          throw;
      } catch (const std::exception& e) {
          std::throw_with_nested(Ewoms::OpmInputError(location, e));
      }
    */
    OpmInputError(const KeywordLocation& loc, const std::exception& e) :
        m_what   { OpmInputError::formatException(loc, e) },
        location { loc }
    {}

    OpmInputError(const std::string& msg) :
        m_what(msg)
    {}

    const char * what() const throw()
    {
        return this->m_what.c_str();
    }

    static std::string format(const std::string& msg_format, const KeywordLocation& loc);
    static std::string formatException(const KeywordLocation& loc, const std::exception& e);

private:
    std::string m_what;

    // The location member is here for debugging; depending on the msg_fmt
    // passed in the constructor we might not have captured all the information
    // in the location argument passed to the constructor.
    Ewoms::optional<KeywordLocation> location;
};

}
#endif
