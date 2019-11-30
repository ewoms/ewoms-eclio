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

#ifndef INJECTION_H
#define INJECTION_H

#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/well.hh>

namespace Ewoms {
namespace injection {

double rateToSI(double rawRate, Well::InjectorType wellType, const Ewoms::UnitSystem &unitSystem);
double rateToSI(double rawRate, Phase wellPhase, const Ewoms::UnitSystem& unitSystem);

}
}

#endif
