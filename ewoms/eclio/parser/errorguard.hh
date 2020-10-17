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
#ifndef ERROR_GUARD_H
#define ERROR_GUARD_H

#include <string>
#include <vector>

namespace Ewoms {

class ErrorGuard {
public:
    void addError(const std::string& errorKey, const std::string& msg);
    void addWarning(const std::string& errorKey, const std::string &msg);
    void clear();

    explicit operator bool() const { return !this->error_list.empty(); }

    /*
      Observe that this desctructor has a somewhat special semantics. If there
      are errors in the error list it will print all warnings and errors on
      stderr and throw std::runtime_error.
    */
    ~ErrorGuard();
    void terminate() const;
    void dump() const;

private:

    std::vector<std::pair<std::string, std::string>> error_list;
    std::vector<std::pair<std::string, std::string>> warning_list;
};

}

#endif
