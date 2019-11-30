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

#ifndef BOXMANAGER_HH_
#define BOXMANAGER_HH_

#include <vector>
#include <memory>

#include <ewoms/eclio/parser/eclipsestate/grid/box.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>

/*
  This class implements a simple book keeping system for the current
  input box. In general there are three different input boxes which
  are relevant:

   1. The global box give by the complete dimensions of the grid.

   2. The input box given explicitly by the BOX keyword. That BOX will
       apply to all following FIELD properties, and it will continue
       to apply until either:

          - ENDBOX
          - A new BOX
          - End of current section

       is encountered.

   3. Some keywords allow for a Box which applies only to the elements
      of that keyword.

*/

namespace Ewoms {

    class BoxManager {
    public:
        BoxManager(const EclipseGrid& grid);

        void setInputBox( int i1,int i2 , int j1 , int j2 , int k1 , int k2);
        void setKeywordBox( int i1,int i2 , int j1 , int j2 , int k1 , int k2);

        void endSection();
        void endInputBox();
        void endKeyword();

        const Box& getActiveBox() const;
        const std::vector<Box::cell_index>& index_list() const;

    private:
        const EclipseGrid& grid;
        std::unique_ptr<Box> m_globalBox;
        std::unique_ptr<Box> m_inputBox;
        std::unique_ptr<Box> m_keywordBox;
    };
}

#endif
