// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*****************************************************************************
 *                                                                           *
 *   This program is free software: you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation, either version 3 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 *****************************************************************************/
/*!
 * \file
 * \brief Provides the eWoms specific exception classes.
 */
#ifndef EWOMS_EXCEPTIONS_H
#define EWOMS_EXCEPTIONS_H

#include <stdexcept>

// the eWoms specific exception classes
namespace Ewoms {
class NotImplemented : public std::logic_error
{
public:
    explicit NotImplemented(const std::string &message)
        : std::logic_error(message)
    {}
};

class NumericalProblem : public std::runtime_error
{
public:
    explicit NumericalProblem(const std::string &message)
        : std::runtime_error(message)
    {}
};

class MaterialLawProblem : public NumericalProblem
{
public:
    explicit MaterialLawProblem(const std::string &message)
        : NumericalProblem(message)
    {}
};

class LinearSolverProblem : public NumericalProblem
{
public:
    explicit LinearSolverProblem(const std::string &message)
        : NumericalProblem(message)
    {}
};
class TooManyIterations : public NumericalProblem
{
public:
    explicit TooManyIterations(const std::string &message)
        : NumericalProblem(message)
    {}
};
}

#endif // EWOMS_EXCEPTIONS_H
