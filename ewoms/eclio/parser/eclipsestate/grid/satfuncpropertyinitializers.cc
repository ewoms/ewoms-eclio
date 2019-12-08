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

#include <array>
#include <exception>
#include <stdexcept>
#include <string>

#include <ewoms/eclio/parser/eclipsestate/eclipse3dproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/gridproperty.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/satfuncpropertyinitializers.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/sgfntable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/sgoftable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/slgoftable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/sof3table.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/swfntable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/swoftable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tabdims.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablecontainer.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>
#include <ewoms/eclio/parser/utility/functional.hh>

namespace Ewoms {

    /*
     * See the "Saturation Functions" chapter in the Eclipse Technical
     * Description; there are several alternative families of keywords which
     * can be used to enter relperm and capillary pressure tables.
     *
     * If SWOF and SGOF are specified in the deck it return I
     * If SWFN, SGFN and SOF3 are specified in the deck it return II
     * If keywords are missing or mixed, an error is given.
     */
    enum class SatfuncFamily { none = 0, I = 1, II = 2 };

    static SatfuncFamily getSaturationFunctionFamily( const TableManager& tm ) {
        const TableContainer& swofTables = tm.getSwofTables();
        const TableContainer& sgofTables = tm.getSgofTables();
        const TableContainer& slgofTables = tm.getSlgofTables();
        const TableContainer& sof3Tables = tm.getSof3Tables();
        const TableContainer& swfnTables = tm.getSwfnTables();
        const TableContainer& sgfnTables = tm.getSgfnTables();

        bool family1 = !sgofTables.empty() || !swofTables.empty() || !slgofTables.empty();
        bool family2 = !swfnTables.empty() || !sgfnTables.empty() || !sof3Tables.empty();

        if (!sgofTables.empty() && !slgofTables.empty()) {
            throw std::invalid_argument("Both, the SGOF and SLGOF have been specified but they are mutually exclusive!");
        }

        if (family1 && family2) {
            throw std::invalid_argument("Saturation families should not be mixed \n"
                                        "Use either SGOF (or SLGOF) and SWOF or SGFN, SWFN and SOF3");
        }

        if (!family1 && !family2) {
            throw std::invalid_argument("Saturations function must be specified using either "
                                        "family 1 or family 2 keywords \n"
                                        "Use either SGOF (or SLGOF) and SWOF or SGFN, SWFN and SOF3" );
        }

        if( family1 ) return SatfuncFamily::I;
        if( family2 ) return SatfuncFamily::II;
        return SatfuncFamily::none;
    }

    enum class limit { min, max };

    static std::vector< double > findMinWaterSaturation( const TableManager& tm ) {
        const auto num_tables = tm.getTabdims().getNumSatTables();
        const auto& swofTables = tm.getSwofTables();
        const auto& swfnTables = tm.getSwfnTables();

        const auto famI = [&swofTables]( int i ) {
            return swofTables.getTable< SwofTable >( i ).getSwColumn().front();
        };

        const auto famII = [&swfnTables]( int i ) {
            return swfnTables.getTable< SwfnTable >( i ).getSwColumn().front();
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I: return map( famI, fun::iota( num_tables ) );
            case SatfuncFamily::II: return map( famII, fun::iota( num_tables ) );
            default:
                throw std::domain_error("No valid saturation keyword family specified");
        }
    }

    static std::vector< double > findMaxWaterSaturation( const TableManager& tm ) {
        const auto num_tables = tm.getTabdims().getNumSatTables();
        const auto& swofTables = tm.getSwofTables();
        const auto& swfnTables = tm.getSwfnTables();

        const auto famI = [&swofTables]( int i ) {
            return swofTables.getTable< SwofTable >( i ).getSwColumn().back();
        };

        const auto famII = [&swfnTables]( int i ) {
            return swfnTables.getTable< SwfnTable >( i ).getSwColumn().back();
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I: return map( famI, fun::iota( num_tables ) );
            case SatfuncFamily::II: return map( famII, fun::iota( num_tables ) );
            default:
                throw std::domain_error("No valid saturation keyword family specified");
        }
    }

    static std::vector< double > findMinGasSaturation( const TableManager& tm ) {
        const auto num_tables = tm.getTabdims().getNumSatTables();
        const auto& sgofTables  = tm.getSgofTables();
        const auto& slgofTables = tm.getSlgofTables();
        const auto& sgfnTables = tm.getSgfnTables();

        const auto famI_sgof = [&sgofTables]( int i ) {
            return sgofTables.getTable< SgofTable >( i ).getSgColumn().front();
        };

        const auto famI_slgof = [&slgofTables]( int i ) {
            return 1.0 - slgofTables.getTable< SlgofTable >( i ).getSlColumn().back();
        };

        const auto famII = [&sgfnTables]( int i ) {
            return sgfnTables.getTable< SgfnTable >( i ).getSgColumn().front();
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I:

                if( sgofTables.empty() && slgofTables.empty() )
                    throw std::runtime_error( "Saturation keyword family I requires either sgof or slgof non-empty" );

                if( !sgofTables.empty() )
                    return fun::map( famI_sgof, fun::iota( num_tables ) );
                else
                    return fun::map( famI_slgof, fun::iota( num_tables ) );

            case SatfuncFamily::II:
                return fun::map( famII, fun::iota( num_tables ) );

            default:
                throw std::domain_error("No valid saturation keyword family specified");

        }

    }

    static std::vector< double > findMaxGasSaturation( const TableManager& tm ) {
        const auto num_tables = tm.getTabdims().getNumSatTables();
        const auto& sgofTables  = tm.getSgofTables();
        const auto& slgofTables = tm.getSlgofTables();
        const auto& sgfnTables = tm.getSgfnTables();

        const auto famI_sgof = [&sgofTables]( int i ) {
            return sgofTables.getTable< SgofTable >( i ).getSgColumn().back();
        };

        const auto famI_slgof = [&slgofTables]( int i ) {
            return 1.0 - slgofTables.getTable< SlgofTable >( i ).getSlColumn().front();
        };

        const auto famII = [&sgfnTables]( int i ) {
            return sgfnTables.getTable< SgfnTable >( i ).getSgColumn().back();
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I:
                if( sgofTables.empty() && slgofTables.empty() )
                    throw std::runtime_error( "Saturation keyword family I requires either sgof or slgof non-empty" );

                if( !sgofTables.empty() )
                    return fun::map( famI_sgof, fun::iota( num_tables ) );
                else
                    return fun::map( famI_slgof, fun::iota( num_tables ) );

            case SatfuncFamily::II:
                return fun::map( famII, fun::iota( num_tables ) );

            default:
                throw std::domain_error("No valid saturation keyword family specified");
        }
    }

    /*
     * These functions have been ported from an older implementation to instead
     * use std::upper_bound and more from <algorithm> to make code -intent-
     * clearer. This also made some (maybe intentional) details easier to spot.
     * A short discussion:
     *
     * I don't know if not finding any element larger than 0.0 in the tables
     * was ever supposed to happen (or even possible), in which case the vector
     * elements remained at their initial value of 0.0. This behaviour has been
     * preserved, but is now explicit. The original code was also not clear if
     * it was possible to look up columns at index -1 (see critical_water for
     * an example), but the new version is explicit about this. Unfortuately
     * I'm not familiar enough with the maths or internal structure to make
     * more than a guess here, but most of this behaviour should be preserved.
     *
     */

    template< typename T >
    static inline double critical_water( const T& table ) {

        const auto& col = table.getKrwColumn();
        const auto end = col.begin() + table.numRows();
        const auto critical = std::upper_bound( col.begin(), end, 0.0 );
        const auto index = std::distance( col.begin(), critical );

        if( index == 0 || critical == end ) return 0.0;

        return table.getSwColumn()[ index - 1 ];
    }

    static std::vector< double > findCriticalWater( const TableManager& tm ) {

        const auto num_tables = tm.getTabdims().getNumSatTables();
        const auto& swofTables = tm.getSwofTables();
        const auto& swfnTables = tm.getSwfnTables();

        const auto famI = [&swofTables]( int i ) {
            return critical_water( swofTables.getTable< SwofTable >( i ) );
        };

        const auto famII = [&swfnTables]( int i ) {
            return critical_water( swfnTables.getTable< SwfnTable >( i ) );
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I: return fun::map( famI, fun::iota( num_tables ) );
            case SatfuncFamily::II: return fun::map( famII, fun::iota( num_tables ) );
            default: throw std::domain_error("No valid saturation keyword family specified");
        }
    }

    template< typename T >
    static inline double critical_gas( const T& table ) {
        const auto& col = table.getKrgColumn();
        const auto end = col.begin() + table.numRows();
        const auto critical = std::upper_bound( col.begin(), end, 0.0 );
        const auto index = std::distance( col.begin(), critical );

        if( index == 0 || critical == end ) return 0.0;

        return table.getSgColumn()[ index - 1 ];
    }

    static inline double critical_gas( const SlgofTable& slgofTable ) {
        const auto& col = slgofTable.getKrgColumn();
        const auto critical = std::upper_bound( col.begin(), col.end(), 0.0 );
        const auto index = std::distance( col.begin(), critical );

        if( index == 0 || critical == col.end() ) return 0.0;

        return slgofTable.getSlColumn()[ index - 1 ];
    }

    static std::vector< double > findCriticalGas( const TableManager& tm ) {

        const auto num_tables = tm.getTabdims().getNumSatTables();
        const auto& sgfnTables = tm.getSgfnTables();
        const auto& sgofTables = tm.getSgofTables();
        const auto& slgofTables = tm.getSlgofTables();

        const auto famI_sgof = [&sgofTables]( int i ) {
            return critical_gas( sgofTables.getTable< SgofTable >( i ) );
        };

        const auto famI_slgof = [&slgofTables]( int i ) {
            return critical_gas( slgofTables.getTable< SlgofTable >( i ) );
        };

        const auto famII = [&sgfnTables]( int i ) {
            return critical_gas( sgfnTables.getTable< SgfnTable >( i ) );
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I:
                if( sgofTables.empty() && slgofTables.empty() )
                    throw std::runtime_error( "Saturation keyword family I requires either sgof or slgof non-empty" );

                if( !sgofTables.empty() )
                    return fun::map( famI_sgof, fun::iota( num_tables ) );
                else
                    return fun::map( famI_slgof, fun::iota( num_tables ) );

            case SatfuncFamily::II:
                return fun::map( famII, fun::iota( num_tables ) );

            default:
                throw std::domain_error("No valid saturation keyword family specified");
        }
    }

    static inline double critical_oil_water( const SwofTable& swofTable ) {
        const auto& col = swofTable.getKrowColumn();

        using reverse = std::reverse_iterator< decltype( col.begin() ) >;
        auto rbegin = reverse( col.begin() + swofTable.numRows() );
        auto rend = reverse( col.begin() );
        const auto critical = std::upper_bound( rbegin, rend, 0.0 );
        const auto index = std::distance( col.begin(), critical.base() - 1 );

        if( critical == rend ) return 0.0;

        return 1 - swofTable.getSwColumn()[ index + 1 ];
    }

    static inline double critical_oil( const Sof3Table& sof3Table, const TableColumn& col ) {
        const auto critical = std::upper_bound( col.begin(), col.end(), 0.0 );
        const auto index = std::distance( col.begin(), critical );

        if( index == 0 || critical == col.end() ) return 0.0;

        return sof3Table.getSoColumn()[ index - 1 ];
    }

    static std::vector< double > findCriticalOilWater( const TableManager& tm ) {
        const auto num_tables = tm.getTabdims().getNumSatTables();
        const auto& swofTables = tm.getSwofTables();
        const auto& sof3Tables= tm.getSof3Tables();

        const auto famI = [&swofTables]( int i ) {
            return critical_oil_water( swofTables.getTable< SwofTable >( i ) );
        };

        const auto famII = [&sof3Tables]( int i ) {
            const auto& tb = sof3Tables.getTable< Sof3Table >( i );
            return critical_oil( tb, tb.getKrowColumn() );
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I: return fun::map( famI, fun::iota( num_tables ) );
            case SatfuncFamily::II: return fun::map( famII, fun::iota( num_tables ) );
            default: throw std::domain_error("No valid saturation keyword family specified");
        }
    }

    static inline double critical_oil_gas( const SgofTable& sgofTable ) {
        const auto& col = sgofTable.getKrogColumn();

        using reverse = std::reverse_iterator< decltype( col.begin() ) >;
        auto rbegin = reverse( col.begin() + sgofTable.numRows() );
        auto rend = reverse( col.begin() );
        const auto critical = std::upper_bound( rbegin, rend, 0.0 );
        if( critical == rend ) {
            return 0.0;
        }
        const auto index = std::distance( col.begin(), critical.base() - 1 );
        return 1.0 - sgofTable.getSgColumn()[ index + 1 ];
    }

    static inline double critical_oil_gas( const SlgofTable& sgofTable ) {

        const auto& col = sgofTable.getKrogColumn();
        const auto critical = std::upper_bound( col.begin(), col.end(), 0.0 );
        if (critical == col.end()) {
            return 0.0;
        }
        const auto index = std::distance( col.begin(), critical - 1);
        return sgofTable.getSlColumn()[ index ];
    }

    static std::vector< double > findCriticalOilGas( const TableManager& tm ) {

        const auto num_tables = tm.getTabdims().getNumSatTables();
        const auto& sgofTables = tm.getSgofTables();
        const auto& slgofTables = tm.getSlgofTables();
        const auto& sof3Tables = tm.getSof3Tables();

        const auto famI_sgof = [&sgofTables]( int i ) {
            return critical_oil_gas( sgofTables.getTable< SgofTable >( i ) );
        };

        const auto famI_slgof = [&slgofTables]( int i ) {
            return critical_oil_gas( slgofTables.getTable< SlgofTable >( i ) );
        };

        const auto famII = [&sof3Tables]( int i ) {
            const auto& tb = sof3Tables.getTable< Sof3Table >( i );
            return critical_oil( tb, tb.getKrogColumn() );
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I:
                if( sgofTables.empty() && slgofTables.empty() )
                    throw std::runtime_error( "Saturation keyword family I requires either sgof or slgof non-empty" );

                if( !sgofTables.empty() )
                    return fun::map( famI_sgof, fun::iota( num_tables ) );
                else
                    return fun::map( famI_slgof, fun::iota( num_tables ) );

            case SatfuncFamily::II:
                return fun::map( famII, fun::iota( num_tables ) );

            default:
                throw std::domain_error("No valid saturation keyword family specified");
        }
    }

    static std::vector< double > findMaxKrg( const TableManager& tm ) {
        const auto num_tables = tm.getTabdims().getNumSatTables();
        const auto& sgofTables = tm.getSgofTables();
        const auto& slgofTables = tm.getSlgofTables();
        const auto& sgfnTables = tm.getSgfnTables();

        const auto& famI_sgof = [&sgofTables]( int i ) {
            return sgofTables.getTable< SgofTable >( i ).getKrgColumn().back();
        };

        const auto& famI_slgof = [&slgofTables]( int i ) {
            return slgofTables.getTable< SlgofTable >( i ).getKrgColumn().front();
        };

        const auto& famII = [&sgfnTables]( int i ) {
            return sgfnTables.getTable< SgfnTable >( i ).getKrgColumn().back();
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I:
                if( sgofTables.empty() && slgofTables.empty() )
                    throw std::runtime_error( "Saturation keyword family I requires either sgof or slgof non-empty" );
                if( !sgofTables.empty() )
                    return fun::map( famI_sgof, fun::iota( num_tables ) );
                else
                    return fun::map( famI_slgof, fun::iota( num_tables ) );
            case SatfuncFamily::II:
                return fun::map( famII, fun::iota( num_tables ) );
            default:
                throw std::domain_error("No valid saturation keyword family specified");
        }
    }

    static std::vector< double > findKrgr( const TableManager& tm ) {
        const auto num_tables = tm.getTabdims().getNumSatTables();
        const auto& sgofTables = tm.getSgofTables();
        const auto& slgofTables = tm.getSlgofTables();
        const auto& sgfnTables = tm.getSgfnTables();

        const auto& famI_sgof = [&sgofTables]( int i ) {
            return sgofTables.getTable< SgofTable >( i ).getKrgColumn().front();
        };

        const auto& famI_slgof = [&slgofTables]( int i ) {
            return slgofTables.getTable< SlgofTable >( i ).getKrgColumn().back();
        };

        const auto& famII = [&sgfnTables]( int i ) {
            return sgfnTables.getTable< SgfnTable >( i ).getKrgColumn().back();
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I:
                if( sgofTables.empty() && slgofTables.empty() )
                    throw std::runtime_error( "Saturation keyword family I requires either sgof or slgof non-empty" );
                if( !sgofTables.empty() )
                    return fun::map( famI_sgof, fun::iota( num_tables ) );
                else
                    return fun::map( famI_slgof, fun::iota( num_tables ) );
            case SatfuncFamily::II:
                return fun::map( famII, fun::iota( num_tables ) );
            default:
                throw std::domain_error("No valid saturation keyword family specified");
        }
    }

    static std::vector< double > findKrwr( const TableManager& tm ) {
        const auto num_tables = tm.getTabdims().getNumSatTables();
        const auto& swofTables = tm.getSwofTables();
        const auto& swfnTables = tm.getSwfnTables();

        const auto& famI = [&swofTables]( int i ) {
            return swofTables.getTable< SwofTable >( i ).getKrwColumn().front();
        };

        const auto& famII = [&swfnTables]( int i ) {
            return swfnTables.getTable< SwfnTable >( i ).getKrwColumn().front();
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I:
                return fun::map( famI, fun::iota( num_tables ) );
            case SatfuncFamily::II:
                return fun::map( famII, fun::iota( num_tables ) );
            default:
                throw std::domain_error("No valid saturation keyword family specified");
        }
    }

    static std::vector< double > findKrorw( const TableManager& tm ) {
        const auto num_tables = tm.getTabdims().getNumSatTables();
        const auto& swofTables = tm.getSwofTables();
        const auto& sof3Tables = tm.getSof3Tables();

        const auto& famI = [&swofTables]( int i ) {
            const auto& swofTable = swofTables.getTable< SwofTable >( i );
            const auto& krwCol = swofTable.getKrwColumn();
            const auto crit = std::upper_bound( krwCol.begin(), krwCol.end(), 0.0 );
            const auto index = std::distance( krwCol.begin(), crit );

            if( crit == krwCol.end() ) return 0.0;

            return swofTable.getKrowColumn()[ index - 1 ];
        };

        const auto crit_water = findCriticalWater( tm );
        const auto min_gas = findMinGasSaturation( tm );
        const auto& famII = [&sof3Tables,&crit_water,&min_gas]( int i ) {
            const double OilSatAtcritialWaterSat = 1.0 - crit_water[ i ] - min_gas[ i ];
            return sof3Tables.getTable< Sof3Table >( i )
                .evaluate("KROW", OilSatAtcritialWaterSat);
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I:
                return fun::map( famI, fun::iota( num_tables ) );
            case SatfuncFamily::II:
                return fun::map( famII, fun::iota( num_tables ) );
            default:
                throw std::domain_error("No valid saturation keyword family specified");
        }
    }

    static std::vector< double > findKrorg( const TableManager& tm ) {
        const auto num_tables = tm.getTabdims().getNumSatTables();
        const auto& sgofTables = tm.getSgofTables();
        const auto& slgofTables = tm.getSlgofTables();
        const auto& sof3Tables = tm.getSof3Tables();

        const auto& famI_sgof = [&sgofTables]( int i ) {
            const auto& sgofTable = sgofTables.getTable< SgofTable >( i );
            const auto& krgCol = sgofTable.getKrgColumn();
            const auto crit = std::upper_bound( krgCol.begin(), krgCol.end(), 0.0 );
            const auto index = std::distance( krgCol.begin(), crit );

            if( crit == krgCol.end() ) return 0.0;

            return sgofTable.getKrogColumn()[ index - 1 ];
        };

        const auto& famI_slgof = [&slgofTables]( int i ) {
            const auto& slgofTable = slgofTables.getTable< SlgofTable >( i );
            const auto& col = slgofTable.getKrgColumn();
            using reverse = std::reverse_iterator< decltype( col.begin() ) >;
            auto rbegin = reverse( col.begin() + slgofTable.numRows() );
            auto rend = reverse( col.begin() );
            const auto crit = std::upper_bound( rbegin, rend, 0.0 );
            // base() points to the next element in the forward order
            const auto index = std::distance( col.begin(), crit.base());

            if( crit == rend ) return 0.0;

            return slgofTable.getKrogColumn()[ index ];
        };

        const auto crit_gas = findCriticalGas( tm );
        const auto min_water = findMinWaterSaturation( tm );
        const auto& famII = [&sof3Tables,&crit_gas,&min_water]( int i ) {
            const double OilSatAtcritialGasSat = 1.0 - crit_gas[ i ] - min_water[ i ];
            return sof3Tables.getTable< Sof3Table >( i )
                .evaluate("KROG", OilSatAtcritialGasSat);
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I:
                if( sgofTables.empty() && slgofTables.empty() )
                    throw std::runtime_error( "Saturation keyword family I requires either sgof or slgof non-empty" );
                if( !sgofTables.empty() )
                    return fun::map( famI_sgof, fun::iota( num_tables ) );
                else
                    return fun::map( famI_slgof, fun::iota( num_tables ) );
            case SatfuncFamily::II:
                return fun::map( famII, fun::iota( num_tables ) );
            default:
                throw std::domain_error("No valid saturation keyword family specified");
        }
    }

    /* find the maximum output values of the water-oil system. the maximum oil
     * relperm is possibly wrong because we have two oil relperms in a threephase
     * system. the documentation is very ambiguos here, though: it says that the
     * oil relperm at the maximum oil saturation is scaled according to maximum
     * specified the KRO keyword. the first part of the statement points at
     * scaling the resultant threephase oil relperm, but then the gas saturation
     * is not taken into account which means that some twophase quantity must be
     * scaled.
     */
    static std::vector< double > findMaxPcog( const TableManager& tm ) {
        const auto num_tables = tm.getTabdims().getNumSatTables();
        const auto& sgofTables = tm.getSgofTables();
        const auto& slgofTables = tm.getSlgofTables();
        const auto& sgfnTables = tm.getSgfnTables();

        const auto& famI_sgof = [&sgofTables]( int i ) {
            return sgofTables.getTable< SgofTable >( i ).getPcogColumn().back();
        };

        const auto& famI_slgof = [&slgofTables]( int i ) {
            return slgofTables.getTable< SlgofTable >( i ).getPcogColumn().front();
        };

        const auto& famII = [&sgfnTables]( int i ) {
            return sgfnTables.getTable< SgfnTable >( i ).getPcogColumn().back();
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I:
                if( sgofTables.empty() && slgofTables.empty() )
                    throw std::runtime_error( "Saturation keyword family I requires either sgof or slgof non-empty" );
                if( !sgofTables.empty() )
                    return fun::map( famI_sgof, fun::iota( num_tables ) );
                else
                    return fun::map( famI_slgof, fun::iota( num_tables ) );
            case SatfuncFamily::II:
                return fun::map( famII, fun::iota( num_tables ) );
            default:
                throw std::domain_error("No valid saturation keyword family specified");
        }
    }

    static std::vector< double > findMaxPcow( const TableManager& tm ) {
        const auto num_tables  = tm.getTabdims().getNumSatTables();
        const auto& swofTables = tm.getSwofTables();
        const auto& swfnTables = tm.getSwfnTables();

        const auto& famI = [&swofTables]( int i ) {
            return swofTables.getTable< SwofTable >( i ).getPcowColumn().front();
        };

        const auto& famII = [&swfnTables]( int i ) {
            return swfnTables.getTable< SwfnTable >( i ).getPcowColumn().front();
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I:
                return fun::map( famI, fun::iota( num_tables ) );
            case SatfuncFamily::II:
                return fun::map( famII, fun::iota( num_tables ) );
            default:
                throw std::domain_error("No valid saturation keyword family specified");
        }
    }

    static std::vector< double > findMaxKro( const TableManager& tm ) {
        const auto num_tables = tm.getTabdims().getNumSatTables();
        const auto& swofTables = tm.getSwofTables();
        const auto& sof3Tables = tm.getSof3Tables();

        const auto& famI = [&swofTables]( int i ) {
            return swofTables.getTable< SwofTable >( i ).getKrowColumn().front();
        };

        const auto& famII = [&sof3Tables]( int i ) {
            return sof3Tables.getTable< Sof3Table >( i ).getKrowColumn().back();
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I:
                return fun::map( famI, fun::iota( num_tables ) );
            case SatfuncFamily::II:
                return fun::map( famII, fun::iota( num_tables ) );
            default:
                throw std::domain_error("No valid saturation keyword family specified");
        }
    }

    static std::vector< double > findMaxKrw( const TableManager& tm ) {
        const auto num_tables = tm.getTabdims().getNumSatTables();
        const auto& swofTables = tm.getSwofTables();
        const auto& swfnTables = tm.getSwfnTables();

        const auto& famI = [&swofTables]( int i ) {
            return swofTables.getTable< SwofTable >( i ).getKrwColumn().back();
        };

        const auto& famII = [&swfnTables]( int i ) {
            return swfnTables.getTable< SwfnTable >( i ).getKrwColumn().back();
        };

        switch( getSaturationFunctionFamily( tm ) ) {
            case SatfuncFamily::I:
                return fun::map( famI, fun::iota( num_tables ) );
            case SatfuncFamily::II:
                return fun::map( famII, fun::iota( num_tables ) );
            default:
                throw std::domain_error("No valid saturation keyword family specified");
        }
    }

    static double selectValue( const TableContainer& depthTables,
                               int tableIdx,
                               const std::string& columnName,
                               double cellDepth,
                               double fallbackValue,
                               bool useOneMinusTableValue) {

        if( tableIdx < 0 ) return fallbackValue;

        const auto& table = depthTables.getTable( tableIdx );

        if( tableIdx >= int( depthTables.size() ) )
            throw std::invalid_argument("Not enough tables!");

        // evaluate the table at the cell depth
        const double value = table.evaluate( columnName, cellDepth );

        // a column can be fully defaulted. In this case, eval() returns a NaN
        // and we have to use the data from saturation tables
        if( !std::isfinite( value ) ) return fallbackValue;
        if( useOneMinusTableValue ) return 1 - value;
        return value;
    }

    static double cell_depth(const EclipseGrid& grid, std::size_t cell_index, bool active_only) {
        if (active_only)
            return grid.getCellDepth( grid.getGlobalIndex(cell_index) );
        else
            return grid.getCellDepth(cell_index);
    }

    static std::vector< double > satnumApply( size_t size,
                                              const std::string& columnName,
                                              const std::vector< double >& fallbackValues,
                                              const TableManager& tableManager,
                                              const EclipseGrid& eclipseGrid,
                                              const std::vector<int>& satnum_data,
                                              const std::vector<int>& endnum_data,
                                              bool useOneMinusTableValue ) {

        std::vector< double > values( size, 0 );
        // Actually assign the defaults. If the ENPVD keyword was specified in the deck,
        // this currently cannot be done because we would need the Z-coordinate of the
        // cell and we would need to know how the simulator wants to interpolate between
        // sampling points. Both of these are outside the scope of ewoms-eclio, so we just
        // assign a NaN in this case...
        bool active_only = (size == eclipseGrid.getNumActive());
        const bool useEnptvd = tableManager.useEnptvd();
        const auto& enptvdTables = tableManager.getEnptvdTables();
        for( size_t cellIdx = 0; cellIdx < values.size(); cellIdx++ ) {
            int satTableIdx = satnum_data[cellIdx] - 1;
            int endNum = endnum_data[cellIdx] - 1;

            if (!active_only) {
                if (! eclipseGrid.cellActive(cellIdx)) {
                    // Pick from appropriate saturation region if defined
                    // in this cell, else use region 1 (satTableIdx == 0).
                    values[cellIdx] = (satTableIdx >= 0)
                        ? fallbackValues[satTableIdx] : fallbackValues[0];
                    continue;
                }
            }

            // Active cell better have {SAT,END}NUM > 0.
            if ((satTableIdx < 0) || (endNum < 0)) {
                throw std::invalid_argument {
                    "Region Index Out of Bounds in Active Cell "
                    + std::to_string(cellIdx) + ". SATNUM = "
                    + std::to_string(satTableIdx + 1) + ", ENDNUM = "
                    + std::to_string(endNum + 1)
                };
            }

            values[cellIdx] = selectValue(enptvdTables,
                                          (useEnptvd && endNum >= 0) ? endNum : -1,
                                          columnName,
                                          cell_depth(eclipseGrid, cellIdx, active_only),
                                          fallbackValues[ satTableIdx ],
                                          useOneMinusTableValue);
        }

        return values;
    }

    static std::vector< double > satnumApply( size_t size,
                                              const std::string& columnName,
                                              const std::vector< double >& fallbackValues,
                                              const TableManager& tableManager,
                                              const EclipseGrid& eclipseGrid,
                                              const GridProperties<int>* intGridProperties,
                                              bool useOneMinusTableValue ) {
        auto tabdims = tableManager.getTabdims();
        const auto& satnum = intGridProperties->getKeyword("SATNUM");
        const auto& endnum = intGridProperties->getKeyword("ENDNUM");
        int numSatTables = tabdims.getNumSatTables();

        // SATNUM = 0 *might* occur in deactivated cells
        satnum.checkLimits( 0 , numSatTables );

        return satnumApply(size,
                           columnName,
                           fallbackValues,
                           tableManager,
                           eclipseGrid,
                           satnum.getData(),
                           endnum.getData(),
                           useOneMinusTableValue);
    }

    static std::vector< double > imbnumApply( size_t size,
                                              const std::string& columnName,
                                              const std::vector< double >& fallBackValues,
                                              const TableManager& tableManager,
                                              const EclipseGrid& eclipseGrid,
                                              const std::vector<int>& imbnum_data,
                                              const std::vector<int>& endnum_data,
                                              bool useOneMinusTableValue ) {

        std::vector< double > values( size, 0 );

        // Actually assign the defaults. if the ENPVD keyword was specified in the deck,
        // this currently cannot be done because we would need the Z-coordinate of the
        // cell and we would need to know how the simulator wants to interpolate between
        // sampling points. Both of these are outside the scope of ewoms-eclio, so we just
        // assign a NaN in this case...
        bool active_only = (size == eclipseGrid.getNumActive());
        const bool useImptvd = tableManager.useImptvd();
        const TableContainer& imptvdTables = tableManager.getImptvdTables();
        for( size_t cellIdx = 0; cellIdx < values.size(); cellIdx++ ) {
            int imbTableIdx = imbnum_data[ cellIdx ] - 1;
            int endNum = endnum_data[ cellIdx ] - 1;

            if (!active_only) {
                if (! eclipseGrid.cellActive(cellIdx)) {
                    // Pick from appropriate saturation region if defined
                    // in this cell, else use region 1 (imbTableIdx == 0).
                    values[cellIdx] = (imbTableIdx >= 0)
                        ? fallBackValues[imbTableIdx] : fallBackValues[0];
                    continue;
                }
            }

            // Active cell better have {IMB,END}NUM > 0.
            if ((imbTableIdx < 0) || (endNum < 0)) {
                throw std::invalid_argument {
                    "Region Index Out of Bounds in Active Cell "
                    + std::to_string(cellIdx) + ". IMBNUM = "
                    + std::to_string(imbTableIdx + 1) + ", ENDNUM = "
                    + std::to_string(endNum + 1)
                };
            }

            values[cellIdx] = selectValue(imptvdTables,
                                          (useImptvd && endNum >= 0) ? endNum : -1,
                                          columnName,
                                          cell_depth(eclipseGrid, cellIdx, active_only),
                                          fallBackValues[imbTableIdx],
                                          useOneMinusTableValue);
        }

        return values;
    }

    static std::vector< double > imbnumApply( size_t size,
                                              const std::string& columnName,
                                              const std::vector< double >& fallbackValues,
                                              const TableManager& tableManager,
                                              const EclipseGrid& eclipseGrid,
                                              const GridProperties<int>* intGridProperties,
                                              bool useOneMinusTableValue ) {
        auto tabdims = tableManager.getTabdims();
        const auto& imbnum = intGridProperties->getKeyword("IMBNUM");
        const auto& endnum = intGridProperties->getKeyword("ENDNUM");
        int numSatTables = tabdims.getNumSatTables();

        // IMBNUM = 0 *might* occur in deactivated cells
        imbnum.checkLimits( 0 , numSatTables );

        return imbnumApply(size,
                           columnName,
                           fallbackValues,
                           tableManager,
                           eclipseGrid,
                           imbnum.getData(),
                           endnum.getData(),
                           useOneMinusTableValue);

    }

    std::vector< double > SGLEndpoint( size_t size,
                                       const TableManager * tableManager,
                                       const EclipseGrid* eclipseGrid,
                                       GridProperties<int>* intGridProperties )
    {
        const auto min_gas = findMinGasSaturation( *tableManager );
        return satnumApply( size, "SGCO", min_gas, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > ISGLEndpoint( size_t size,
                                        const TableManager * tableManager,
                                        const EclipseGrid* eclipseGrid,
                                        GridProperties<int>* intGridProperties )
    {
        const auto min_gas = findMinGasSaturation( *tableManager );
        return imbnumApply( size, "SGCO", min_gas, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > SGUEndpoint( size_t size,
                                       const TableManager * tableManager,
                                       const EclipseGrid* eclipseGrid,
                                       GridProperties<int>* intGridProperties )
    {
        const auto max_gas = findMaxGasSaturation( *tableManager );
        return satnumApply( size, "SGMAX", max_gas, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > ISGUEndpoint( size_t size,
                                        const TableManager * tableManager,
                                        const EclipseGrid* eclipseGrid,
                                        GridProperties<int>* intGridProperties )
    {
        const auto max_gas = findMaxGasSaturation( *tableManager );
        return imbnumApply( size, "SGMAX", max_gas, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > SWLEndpoint( size_t size,
                                       const TableManager * tableManager,
                                       const EclipseGrid* eclipseGrid,
                                       GridProperties<int>* intGridProperties )
    {
        const auto min_water = findMinWaterSaturation( *tableManager );
        return satnumApply( size, "SWCO", min_water, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > ISWLEndpoint( size_t size,
                                        const TableManager * tableManager,
                                        const EclipseGrid  * eclipseGrid,
                                        GridProperties<int>* intGridProperties )
    {
        const auto min_water = findMinWaterSaturation( *tableManager );
        return imbnumApply( size, "SWCO", min_water, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > SWUEndpoint( size_t size,
                                       const TableManager * tableManager,
                                       const EclipseGrid  * eclipseGrid,
                                       GridProperties<int>* intGridProperties )
    {
        const auto max_water = findMaxWaterSaturation( *tableManager );
        return satnumApply( size, "SWMAX", max_water, *tableManager, *eclipseGrid,
                            intGridProperties, true );
    }

    std::vector< double > ISWUEndpoint( size_t size,
                                        const TableManager * tableManager,
                                        const EclipseGrid  * eclipseGrid,
                                        GridProperties<int>* intGridProperties )
    {
        const auto max_water = findMaxWaterSaturation( *tableManager );
        return imbnumApply( size, "SWMAX", max_water, *tableManager, *eclipseGrid,
                            intGridProperties, true);
    }

    std::vector< double > SGCREndpoint( size_t size,
                                        const TableManager * tableManager,
                                        const EclipseGrid  * eclipseGrid,
                                        GridProperties<int>* intGridProperties )
    {
        const auto crit_gas = findCriticalGas( *tableManager );
        return satnumApply( size, "SGCRIT", crit_gas, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > ISGCREndpoint( size_t size,
                                         const TableManager * tableManager,
                                         const EclipseGrid  * eclipseGrid,
                                         GridProperties<int>* intGridProperties )
    {
        const auto crit_gas = findCriticalGas( *tableManager );
        return imbnumApply( size, "SGCRIT", crit_gas, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > SOWCREndpoint( size_t size,
                                         const TableManager * tableManager,
                                         const EclipseGrid  * eclipseGrid,
                                         GridProperties<int>* intGridProperties )
    {
        const auto oil_water = findCriticalOilWater( *tableManager );
        return satnumApply( size, "SOWCRIT", oil_water, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > ISOWCREndpoint( size_t size,
                                          const TableManager * tableManager,
                                          const EclipseGrid  * eclipseGrid,
                                          GridProperties<int>* intGridProperties )
    {
        const auto oil_water = findCriticalOilWater( *tableManager );
        return imbnumApply( size, "SOWCRIT", oil_water, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > SOGCREndpoint( size_t size,
                                         const TableManager * tableManager,
                                         const EclipseGrid  * eclipseGrid,
                                         GridProperties<int>* intGridProperties )
    {
        const auto crit_oil_gas = findCriticalOilGas( *tableManager );
        return satnumApply( size, "SOGCRIT", crit_oil_gas, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > ISOGCREndpoint( size_t size,
                                          const TableManager * tableManager,
                                          const EclipseGrid  * eclipseGrid,
                                          GridProperties<int>* intGridProperties )
    {
        const auto crit_oil_gas = findCriticalOilGas( *tableManager );
        return imbnumApply( size, "SOGCRIT", crit_oil_gas, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > SWCREndpoint( size_t size,
                                        const TableManager * tableManager,
                                        const EclipseGrid  * eclipseGrid,
                                        GridProperties<int>* intGridProperties )
    {
        const auto crit_water = findCriticalWater( *tableManager );
        return satnumApply( size, "SWCRIT", crit_water, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > ISWCREndpoint( size_t size,
                                         const TableManager * tableManager,
                                         const EclipseGrid  * eclipseGrid,
                                         GridProperties<int>* intGridProperties )
    {
        const auto crit_water = findCriticalWater( *tableManager );
        return imbnumApply( size, "SWCRIT", crit_water, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > PCWEndpoint( size_t size,
                                       const TableManager * tableManager,
                                       const EclipseGrid  * eclipseGrid,
                                       GridProperties<int>* intGridProperties )
    {
        const auto max_pcow = findMaxPcow( *tableManager );
        return satnumApply( size, "PCW", max_pcow, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > IPCWEndpoint( size_t size,
                                        const TableManager * tableManager,
                                        const EclipseGrid  * eclipseGrid,
                                        GridProperties<int>* intGridProperties )
    {
        const auto max_pcow = findMaxPcow( *tableManager );
        return imbnumApply( size, "IPCW", max_pcow, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > PCGEndpoint( size_t size,
                                       const TableManager * tableManager,
                                       const EclipseGrid  * eclipseGrid,
                                       GridProperties<int>* intGridProperties )
    {
        const auto max_pcog = findMaxPcog( *tableManager );
        return satnumApply( size, "PCG", max_pcog, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > IPCGEndpoint( size_t size,
                                        const TableManager * tableManager,
                                        const EclipseGrid  * eclipseGrid,
                                        GridProperties<int>* intGridProperties )
    {
        const auto max_pcog = findMaxPcog( *tableManager );
        return imbnumApply( size, "IPCG", max_pcog, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > KRWEndpoint( size_t size,
                                       const TableManager * tableManager,
                                       const EclipseGrid  * eclipseGrid,
                                       GridProperties<int>* intGridProperties )
    {
        const auto max_krw = findMaxKrw( *tableManager );
        return satnumApply( size, "KRW", max_krw, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > IKRWEndpoint( size_t size,
                                        const TableManager * tableManager,
                                        const EclipseGrid  * eclipseGrid,
                                        GridProperties<int>* intGridProperties )
    {
        const auto krwr = findKrwr( *tableManager );
        return imbnumApply( size, "IKRW", krwr, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > KRWREndpoint( size_t size,
                                        const TableManager * tableManager,
                                        const EclipseGrid  * eclipseGrid,
                                        GridProperties<int>* intGridProperties )
    {
        const auto krwr = findKrwr( *tableManager );
        return satnumApply( size, "KRWR", krwr, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > IKRWREndpoint( size_t size,
                                         const TableManager * tableManager,
                                         const EclipseGrid  * eclipseGrid,
                                         GridProperties<int>* intGridProperties )
    {
        const auto krwr = findKrwr( *tableManager );
        return imbnumApply( size, "IKRWR", krwr, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > KROEndpoint( size_t size,
                                       const TableManager * tableManager,
                                       const EclipseGrid  * eclipseGrid,
                                       GridProperties<int>* intGridProperties )
    {
        const auto max_kro = findMaxKro( *tableManager );
        return satnumApply( size, "KRO", max_kro, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > IKROEndpoint( size_t size,
                                        const TableManager * tableManager,
                                        const EclipseGrid  * eclipseGrid,
                                        GridProperties<int>* intGridProperties )
    {
        const auto max_kro = findMaxKro( *tableManager );
        return imbnumApply( size, "IKRO", max_kro, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > KRORWEndpoint( size_t size,
                                         const TableManager * tableManager,
                                         const EclipseGrid  * eclipseGrid,
                                         GridProperties<int>* intGridProperties )
    {
        const auto krorw = findKrorw( *tableManager );
        return satnumApply( size, "KRORW", krorw, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > IKRORWEndpoint( size_t size,
                                          const TableManager * tableManager,
                                          const EclipseGrid  * eclipseGrid,
                                          GridProperties<int>* intGridProperties )
    {
        const auto krorw = findKrorw( *tableManager );
        return imbnumApply( size, "IKRORW", krorw, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > KRORGEndpoint( size_t size,
                                         const TableManager * tableManager,
                                         const EclipseGrid  * eclipseGrid,
                                         GridProperties<int>* intGridProperties )
    {
        const auto krorg = findKrorg( *tableManager );
        return satnumApply( size, "KRORG", krorg, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > IKRORGEndpoint( size_t size,
                                          const TableManager * tableManager,
                                          const EclipseGrid  * eclipseGrid,
                                          GridProperties<int>* intGridProperties )
    {
        const auto krorg = findKrorg( *tableManager );
        return imbnumApply( size, "IKRORG", krorg, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > KRGEndpoint( size_t size,
                                       const TableManager * tableManager,
                                       const EclipseGrid  * eclipseGrid,
                                       GridProperties<int>* intGridProperties )
    {
        const auto max_krg = findMaxKrg( *tableManager );
        return satnumApply( size, "KRG", max_krg, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > IKRGEndpoint( size_t size,
                                        const TableManager * tableManager,
                                        const EclipseGrid  * eclipseGrid,
                                        GridProperties<int>* intGridProperties )
    {
        const auto max_krg = findMaxKrg( *tableManager );
        return imbnumApply( size, "IKRG", max_krg, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > KRGREndpoint( size_t size,
                                        const TableManager * tableManager,
                                        const EclipseGrid  * eclipseGrid,
                                        GridProperties<int>* intGridProperties )
    {
        const auto krgr = findKrgr( *tableManager );
        return satnumApply( size, "KRGR", krgr, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

    std::vector< double > IKRGREndpoint( size_t size,
                                        const TableManager * tableManager,
                                         const EclipseGrid* eclipseGrid,
                                         GridProperties<int>* intGridProperties )
    {
        const auto krgr = findKrgr( *tableManager );
        return imbnumApply( size, "IKRGR", krgr, *tableManager, *eclipseGrid,
                            intGridProperties, false );
    }

}
