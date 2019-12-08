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

#ifndef FIELDPROPS_MANAGER_H
#define FIELDPROPS_MANAGER_H

#include <memory>

namespace Ewoms {

class EclipseGrid;
class Deck;
class FieldProps;
class TableManager;

class FieldPropsManager {
public:
    // The default constructor should be removed when the FieldPropsManager is mandatory
    FieldPropsManager() = default;
    FieldPropsManager(const Deck& deck, const EclipseGrid& grid, const TableManager& tables);
    void reset_grid(const EclipseGrid& grid);
    const std::string& default_region() const;

    /*
      Because the FieldProps class can autocreate properties the semantics of
      get() and has() is slightly non intuitve:

      - The has<T>("KW") method will check if the current FieldProps container
        has an installed "KW" keyword; if the container has the keyword in
        question it will check if all elements have been assigned a value - only
        in that case will it return true. The has<T>("KW") method will *not* try
        to create a new keyword.

      - The has<T>("KW") method will *not* consult the supported<T>("KW")
        method; i.e. if you ask has<T>("UNKNOWN_KEYWORD") you will just get a
        false.

      - The get<T>("KW") method will try to create a new keyword if it does not
        already have the keyword you are asking for. This implies that you can
        get the following non intuitive sequence of events:

            FieldPropsManager fpm(deck, grid);

            fpm.has<int>("SATNUM");                => false
            auto satnum = fpm.get<int>("SATNUM");  => SATNUM is autocreated
            fpm.has<int>("SATNUM");                => true

      - When checking whether the container has the keyword you should rephrase
        the question slightly:

        * Does the container have the keyword *right now* => has<T>("KW")
        * Can the container provide the keyword => ptr = try_get<T>("KW")

      - It is quite simple to create a deck where the keywords are only partly
        initialized, all the methods in the FieldPropsManager only consider
        fully initialized keywords.
     */

    template <typename T>
    const std::vector<T>& get(const std::string& keyword) const;

    template <typename T>
    const std::vector<T>* try_get(const std::string& keyword) const;

    template <typename T>
    bool has(const std::string& keyword) const;

    template <typename T>
    std::vector<std::string> keys() const;

    template <typename T>
    std::vector<T> get_global(const std::string& keyword) const;

    template <typename T>
    std::vector<bool> defaulted(const std::string& keyword) const;

    template <typename T>
    static bool supported(const std::string& keyword);

private:
    std::shared_ptr<FieldProps> fp;
};

}

#endif
