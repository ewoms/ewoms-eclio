/*

  This file is part of the eWoms project.

  eWoms is free software: you can redistribute it and/or modify it under the terms
  of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  eWoms is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  eWoms.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef OPERATE_H
#define OPERATE_H

#include <functional>

namespace Ewoms {
namespace Operate {

//using operate_fptr = decltype(&MULTA);
using function = std::function<double(double,double)>;

function get(const std::string& func, double alpha, double beta);

}
}
#endif
