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

#include <ewoms/common/fmt/format.h>

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
      can use named arguments

        {keyword} -> loc.keyword
        {file} -> loc.filename
        {line} -> loc.lineno

      or numbered arguments

        {0} -> loc.keyword
        {1} -> loc.filename
        {2} -> loc.lineno

      If just plain {} placeholders are used the order of the arguments is
      keyword, filename, linenumber.

      KeywordLocation loc("KW", "file.inc", 100);
      OpmInputError("Error at line {line} in file{file} - keyword: {keyword} ignored", location)
    */

    OpmInputError(const std::string& msg_fmt, const KeywordLocation& loc) :
        m_what(OpmInputError::format(msg_fmt, loc)),
        location(loc)
    {}

    const char * what() const throw()
    {
        return this->m_what.c_str();
    }

    static std::string format(const std::string& msg_fmt, const KeywordLocation& loc) {
        return fmt::format(msg_fmt,
                           fmt::arg("keyword", loc.keyword),
                           fmt::arg("file", loc.filename),
                           fmt::arg("line", loc.lineno));
    }

private:
    std::string m_what;

    // The location member is here for debugging; depending on the msg_fmt
    // passed in the constructor we might not have captured all the information
    // in the location argument passed to the constructor.
    KeywordLocation location;
};

}
#endif
