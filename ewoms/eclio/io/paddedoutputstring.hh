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
#ifndef EWOMS_PADDEDOUTPUTSTRING_H_H
#define EWOMS_PADDEDOUTPUTSTRING_H_H

#include <algorithm>
#include <array>
#include <cstring>
#include <cstddef>
#include <string>

namespace Ewoms { namespace EclIO {

    /// Null-terminated, left adjusted, space padded array of N characters.
    ///
    /// Simple container of character data.  Exists solely for purpose of
    /// outputting std::string (or types convertible to std::string) as
    /// Fortran-style \code character (len=N) \endcode values.
    ///
    /// \tparam N Number of characters.
    template <std::size_t N>
    class PaddedOutputString
    {
    public:
        PaddedOutputString()
        {
            this->clear();
        }

        explicit PaddedOutputString(const std::string& s)
            : PaddedOutputString()
        {
            this->copy_in(s.c_str(), s.size());
        }

        ~PaddedOutputString() = default;

        PaddedOutputString(const PaddedOutputString& rhs) = default;
        PaddedOutputString(PaddedOutputString&& rhs) = default;

        PaddedOutputString& operator=(const PaddedOutputString& rhs) = default;
        PaddedOutputString& operator=(PaddedOutputString&& rhs) = default;

        /// Assign from \code std::string \endcode.
        PaddedOutputString& operator=(const std::string& s)
        {
            this->clear();
            this->copy_in(s.data(), s.size());

            return *this;
        }

        const char* c_str() const
        {
            return this->s_.data();
        }

    private:
        enum : typename std::array<char, N + 1>::size_type { NChar = N };

        std::array<char, NChar + 1> s_;

        /// Clear contents of internal array (fill with ' ').
        void clear()
        {
            this->s_.fill(' ');
            this->s_[NChar] = '\0';
        }

        /// Assign new value to internal array (left adjusted, space padded
        /// and null-terminated).
        void copy_in(const char*                                           s,
                     const typename std::array<char, NChar + 1>::size_type len)
        {
            const auto ncpy = std::min(len, static_cast<decltype(len)>(NChar));

            // Note: Not strcpy() or strncpy() here.  The former has no bounds
            // checking, the latter writes a null-terminator at position 'ncpy'
            // (s_[ncpy]) which violates the post condition if ncpy < NChar.
            std::memcpy(this->s_.data(), s,
                        ncpy * sizeof *this->s_.data());
        }
    };

}} // Ewoms::EclIO
#endif // EWOMS_PADDEDOUTPUTSTRING_H_H
