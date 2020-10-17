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
#ifndef EWOMS_PARAMETERMAPITEM_H
#define EWOMS_PARAMETERMAPITEM_H

#include <string>

namespace Ewoms {
	/// The parameter handlig system is structured as a tree,
	/// where each node inhertis from ParameterMapItem.
	///
	/// The abstract virtual function getTag() is  used to determine
	/// which derived class the node actually is.
	struct ParameterMapItem {
	    /// Default constructor
	    ParameterMapItem() : used_(false) {}

	    /// Destructor
	    virtual ~ParameterMapItem() {}

	    /// \brief This function returns a string describing
	    ///        the ParameterMapItem.
	    virtual std::string getTag() const = 0;
	    void setUsed() const { used_ = true; }
	    bool used() const { return used_; }
	private:
	    mutable bool used_;
	};

	template<typename T>
	struct ParameterMapItemTrait {
	    static T convert(const ParameterMapItem&,
                             std::string& conversion_error);
	    static std::string type();
	};
} // namespace Ewoms

#endif // EWOMS_PARAMETERMAPITEM_H
