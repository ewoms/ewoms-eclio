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

#ifndef UDQFUNCTION_H
#define UDQFUNCTION_H

#include <stdexcept>
#include <vector>
#include <functional>
#include <random>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqset.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqenums.hh>

namespace Ewoms {

class UDQFunction {
public:
    UDQFunction(const std::string& name);
    virtual ~UDQFunction() = default;
    const std::string& name() const;
    UDQTokenType type() const;
private:
    std::string m_name;
    UDQTokenType func_type;
};

class UDQScalarFunction : public UDQFunction {
public:
    UDQScalarFunction(const std::string&name, std::function<UDQSet(const UDQSet& arg)> f);
    UDQSet eval(const UDQSet& arg) const;

    static UDQSet SUM(const UDQSet& arg);
    static UDQSet AVEA(const UDQSet& arg);
    static UDQSet AVEG(const UDQSet& arg);
    static UDQSet AVEH(const UDQSet& arg);
    static UDQSet MIN(const UDQSet& arg);
    static UDQSet MAX(const UDQSet& arg);
    static UDQSet NORM1(const UDQSet& arg);
    static UDQSet NORM2(const UDQSet& arg);
    static UDQSet NORMI(const UDQSet& arg);
    static UDQSet PROD(const UDQSet& arg);

private:
    std::function<UDQSet(const UDQSet& arg)> func;
};

class UDQUnaryElementalFunction : public UDQFunction {
public:
    UDQUnaryElementalFunction(const std::string&name, std::function<UDQSet(const UDQSet& arg)> f);
    UDQSet eval(const UDQSet& arg) const;

    static UDQSet ABS(const UDQSet& arg);
    static UDQSet DEF(const UDQSet& arg);
    static UDQSet EXP(const UDQSet& arg);
    static UDQSet IDV(const UDQSet& arg);
    static UDQSet LN(const UDQSet& arg);
    static UDQSet LOG(const UDQSet& arg);
    static UDQSet NINT(const UDQSet& arg);
    static UDQSet SORTA(const UDQSet& arg);
    static UDQSet SORTD(const UDQSet& arg);
    static UDQSet UNDEF(const UDQSet& arg);

    static UDQSet RANDN(std::mt19937& rng, const UDQSet& arg);
    static UDQSet RANDU(std::mt19937& rng, const UDQSet& arg);

private:
    std::function<UDQSet(const UDQSet& arg)> func;
};

class UDQBinaryFunction : public UDQFunction {
public:
    UDQBinaryFunction(const std::string& name, std::function<UDQSet(const UDQSet& lhs, const UDQSet& rhs)> f);
    UDQSet eval(const UDQSet&, const UDQSet& arg) const;

    static UDQSet EQ(double eps, const UDQSet& lhs, const UDQSet& rhs);
    static UDQSet NE(double eps, const UDQSet& lhs, const UDQSet& rhs);
    static UDQSet LE(double eps, const UDQSet& lhs, const UDQSet& rhs);
    static UDQSet GE(double eps, const UDQSet& lhs, const UDQSet& rhs);

    static UDQSet POW(const UDQSet& lhs, const UDQSet& rhs);
    static UDQSet LT(const UDQSet& lhs, const UDQSet& rhs);
    static UDQSet GT(const UDQSet& lhs, const UDQSet& rhs);
    static UDQSet ADD(const UDQSet& lhs, const UDQSet& rhs);
    static UDQSet MUL(const UDQSet& lhs, const UDQSet& rhs);
    static UDQSet SUB(const UDQSet& lhs, const UDQSet& rhs);
    static UDQSet DIV(const UDQSet& lhs, const UDQSet& rhs);

    static UDQSet UADD(const UDQSet& lhs, const UDQSet& rhs);
    static UDQSet UMUL(const UDQSet& lhs, const UDQSet& rhs);
    static UDQSet UMAX(const UDQSet& lhs, const UDQSet& rhs);
    static UDQSet UMIN(const UDQSet& lhs, const UDQSet& rhs);
private:

    std::function<UDQSet(const UDQSet& lhs, const UDQSet& rhs)> func;
};
}
#endif
