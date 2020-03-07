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

#include <memory>

#include <ewoms/eclio/opmlog/logutil.hh>

#include <ewoms/eclio/parser/parserkeywords/a.hh>
#include <ewoms/eclio/parser/parserkeywords/e.hh>
#include <ewoms/eclio/parser/parserkeywords/g.hh>
#include <ewoms/eclio/parser/parserkeywords/m.hh>
#include <ewoms/eclio/parser/parserkeywords/o.hh>
#include <ewoms/eclio/parser/parserkeywords/p.hh>
#include <ewoms/eclio/parser/parserkeywords/s.hh>
#include <ewoms/eclio/parser/parserkeywords/t.hh>
#include <ewoms/eclio/parser/parserkeywords/v.hh>
#include <ewoms/eclio/parser/parserkeywords/w.hh>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>

#include <ewoms/eclio/parser/eclipsestate/tables/brinedensitytable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/enkrvdtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/enptvdtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/gasviscttable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/imkrvdtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/imptvdtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/misctable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/msfntable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/oilviscttable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/plyadstable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/plydhflftable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/plymaxtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/plyrocktable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/plyshlogtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/plyvisctable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/foamadstable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/foammobtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/pmisctable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tlpmixpatable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/pvdgtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/pvdotable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/pvdstable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/rocktabtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/rockwnodtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/overburdtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/rsvdtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/pbvdtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/pdvdtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/rtempvdtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/rvvdtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/saltvdtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/sgcwmistable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/sgfntable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/sgoftable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/sgwfntable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/slgoftable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/sof2table.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/sof3table.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/solventdensitytable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/sorwmistable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/specheattable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/specrocktable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/ssfntable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/swfntable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/swoftable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablecontainer.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/watviscttable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/aqutabtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/jfunc.hh>

#include <ewoms/eclio/parser/eclipsestate/tables/tabdims.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/eqldims.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/regdims.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/aqudims.hh>

#include <ewoms/eclio/parser/units/units.hh>

namespace Ewoms {

    TableManager::TableManager(const std::map<std::string, TableContainer>& simpleTables,
                               const std::vector<PvtgTable>& pvtgTables,
                               const std::vector<PvtoTable>& pvtoTables,
                               const std::vector<Rock2dTable>& rock2dTables,
                               const std::vector<Rock2dtrTable>& rock2dtrTables,
                               const PvtwTable& pvtwTable,
                               const PvcdoTable& pvcdoTable,
                               const DensityTable& densityTable,
                               const PlyvmhTable& plyvmhTable,
                               const RockTable& rockTable,
                               const PlmixparTable& plmixparTable,
                               const ShrateTable& shrateTable,
                               const Stone1exTable& stone1exTable,
                               const TlmixparTable& tlmixparTable,
                               const ViscrefTable& viscrefTable,
                               const WatdentTable& watdentTable,
                               const std::vector<PvtwsaltTable>& pvtwsaltTables,
                               const std::vector<BrineDensityTable>& bdensityTables,
                               const std::vector<SolventDensityTable>& sdensityTables,
                               const std::map<int, PlymwinjTable>& plymwinjTables,
                               const std::map<int, SkprwatTable>& skprwatTables,
                               const std::map<int, SkprpolyTable>& skprpolyTables,
                               const Tabdims& tabdims,
                               const Regdims& regdims,
                               const Eqldims& eqldims,
                               const Aqudims& aqudims,
                               bool useImptvd,
                               bool useEnptvd,
                               bool useEqlnum,
                               bool useShrate,
                               std::shared_ptr<JFunc> jfunc_param,
                               const DenT& oilDenT_,
                               const DenT& gasDenT_,
                               const DenT& watDenT_,
                               const StandardCond& stcond_,
                               std::size_t gas_comp_index,
                               double rtemp)
        :
        m_simpleTables(simpleTables),
        m_pvtgTables(pvtgTables),
        m_pvtoTables(pvtoTables),
        m_rock2dTables(rock2dTables),
        m_rock2dtrTables(rock2dtrTables),
        m_pvtwTable(pvtwTable),
        m_pvcdoTable(pvcdoTable),
        m_densityTable(densityTable),
        m_plyvmhTable(plyvmhTable),
        m_rockTable(rockTable),
        m_plmixparTable(plmixparTable),
        m_shrateTable(shrateTable),
        m_stone1exTable(stone1exTable),
        m_tlmixparTable(tlmixparTable),
        m_viscrefTable(viscrefTable),
        m_watdentTable(watdentTable),
        m_pvtwsaltTables(pvtwsaltTables),
        m_bdensityTables(bdensityTables),
        m_sdensityTables(sdensityTables),
        m_plymwinjTables(plymwinjTables),
        m_skprwatTables(skprwatTables),
        m_skprpolyTables(skprpolyTables),
        m_tabdims(tabdims),
        m_regdims(std::make_shared<Regdims>(regdims)),
        m_eqldims(std::make_shared<Eqldims>(eqldims)),
        m_aqudims(aqudims),
        hasImptvd(useImptvd),
        hasEnptvd(useEnptvd),
        hasEqlnum(useEqlnum),
        hasShrate(useShrate),
        jfunc(std::move(jfunc_param)),
        oilDenT(oilDenT_),
        gasDenT(gasDenT_),
        watDenT(watDenT_),
        stcond(stcond_),
        m_gas_comp_index(gas_comp_index),
        m_rtemp(rtemp)
    {
    }

    TableManager::TableManager( const Deck& deck )
        :
        m_tabdims( Tabdims(deck)),
        m_aqudims( Aqudims(deck)),
        hasImptvd (deck.hasKeyword("IMPTVD")),
        hasEnptvd (deck.hasKeyword("ENPTVD")),
        hasEqlnum (deck.hasKeyword("EQLNUM"))
    {
        if (deck.hasKeyword("JFUNC"))
            jfunc.reset( new JFunc(deck) );

        // determine the default resevoir temperature in Kelvin
        m_rtemp = ParserKeywords::RTEMP::TEMP::defaultValue;
        m_rtemp += Metric::TemperatureOffset; // <- default values always use METRIC as the unit system!

        initDims( deck );
        initSimpleTables( deck );
        initFullTables(deck, "PVTG", m_pvtgTables);
        initFullTables(deck, "PVTO", m_pvtoTables);

        if( deck.hasKeyword( "PVTW" ) )
            this->m_pvtwTable = PvtwTable( deck.getKeyword( "PVTW" ) );

        if( deck.hasKeyword( "PVCDO" ) )
            this->m_pvcdoTable = PvcdoTable( deck.getKeyword( "PVCDO" ) );

        if( deck.hasKeyword( "DENSITY" ) )
            this->m_densityTable = DensityTable( deck.getKeyword( "DENSITY" ) );

        if( deck.hasKeyword( "ROCK" ) )
            this->m_rockTable = RockTable( deck.getKeyword( "ROCK" ) );

        if( deck.hasKeyword( "VISCREF" ) )
            this->m_viscrefTable = ViscrefTable( deck.getKeyword( "VISCREF" ) );

        if( deck.hasKeyword( "WATDENT" ) )
            this->m_watdentTable = WatdentTable( deck.getKeyword( "WATDENT" ) );

        if( deck.hasKeyword( "RTEMP" ) )
            m_rtemp = deck.getKeyword("RTEMP").getRecord(0).getItem("TEMP").getSIDouble( 0 );
        else if (deck.hasKeyword( "RTEMPA" ) )
            m_rtemp = deck.getKeyword("RTEMPA").getRecord(0).getItem("TEMP").getSIDouble( 0 );

        if ( deck.hasKeyword( "ROCK2D") )
            initRockTables(deck, "ROCK2D", m_rock2dTables );

        if ( deck.hasKeyword( "ROCK2DTR") )
            initRockTables(deck, "ROCK2DTR", m_rock2dtrTables );

        if ( deck.hasKeyword( "PVTWSALT") )
            initPvtwsaltTables(deck, m_pvtwsaltTables );

        if ( deck.hasKeyword( "BDENSITY") )
            initBrineTables(deck, m_bdensityTables );

        if ( deck.hasKeyword( "SDENSITY") )
            initSolventTables(deck, m_sdensityTables );

        if (deck.hasKeyword<ParserKeywords::GASDENT>())
            this->gasDenT = DenT( deck.getKeyword<ParserKeywords::GASDENT>());

        if (deck.hasKeyword<ParserKeywords::OILDENT>())
            this->oilDenT = DenT( deck.getKeyword<ParserKeywords::OILDENT>());

        if (deck.hasKeyword<ParserKeywords::WATDENT>())
            this->watDenT = DenT( deck.getKeyword<ParserKeywords::WATDENT>());

        if (deck.hasKeyword<ParserKeywords::STCOND>()) {
            auto stcondKeyword = deck.getKeyword("STCOND");
            this->stcond.temperature = stcondKeyword.getRecord(0).getItem("TEMPERATURE").getSIDouble(0);
            this->stcond.pressure = stcondKeyword.getRecord(0).getItem("PRESSURE").getSIDouble(0);
        }

        if (deck.hasKeyword<ParserKeywords::PLMIXPAR>()) {
            this->m_plmixparTable = PlmixparTable(deck.getKeyword("PLMIXPAR"));
        }

        if (deck.hasKeyword<ParserKeywords::SHRATE>()) {
            this->m_shrateTable = ShrateTable(deck.getKeyword("SHRATE"));
            hasShrate = true;
        }

        if (deck.hasKeyword<ParserKeywords::STONE1EX>()) {
            this->m_stone1exTable = Stone1exTable(deck.getKeyword("STONE1EX"));
            hasShrate = true;
        }

        if (deck.hasKeyword<ParserKeywords::TLMIXPAR>()) {
            this->m_tlmixparTable = TlmixparTable(deck.getKeyword("TLMIXPAR"));
        }

        if (deck.hasKeyword<ParserKeywords::PLYVMH>()) {
            this->m_plyvmhTable = PlyvmhTable(deck.getKeyword("PLYVMH"));
        }

        using GC = ParserKeywords::GCOMPIDX;
        if (deck.hasKeyword<GC>())
            this->m_gas_comp_index = deck.getKeyword<GC>().getRecord(0).getItem<GC::GAS_COMPONENT_INDEX>().get<int>(0);
    }

    TableManager& TableManager::operator=(const TableManager& data) {
        m_simpleTables = data.m_simpleTables;
        m_pvtgTables = data.m_pvtgTables;
        m_pvtoTables = data.m_pvtoTables;
        m_rock2dTables = data.m_rock2dTables;
        m_rock2dtrTables = data.m_rock2dtrTables;
        m_pvtwTable = data.m_pvtwTable;
        m_pvcdoTable = data.m_pvcdoTable;
        m_plyvmhTable = data.m_plyvmhTable;
        m_densityTable = data.m_densityTable;
        m_plmixparTable = data.m_plmixparTable;
        m_shrateTable = data.m_shrateTable;
        m_stone1exTable = data.m_stone1exTable;
        m_tlmixparTable = data.m_tlmixparTable;
        m_viscrefTable = data.m_viscrefTable;
        m_watdentTable = data.m_watdentTable;
        m_pvtwsaltTables = data.m_pvtwsaltTables;
        m_bdensityTables = data.m_bdensityTables;
        m_sdensityTables = data.m_sdensityTables;
        m_plymwinjTables = data.m_plymwinjTables;
        m_skprwatTables = data.m_skprwatTables;
        m_skprpolyTables = data.m_skprpolyTables;
        m_tabdims = data.m_tabdims;
        m_regdims = std::make_shared<Regdims>(*data.m_regdims);
        m_eqldims = std::make_shared<Eqldims>(*data.m_eqldims);
        m_aqudims = data.m_aqudims;
        hasImptvd = data.hasImptvd;
        hasEnptvd = data.hasEnptvd;
        hasEqlnum = data.hasEqlnum;
        hasShrate = data.hasShrate;
        if (data.jfunc)
          jfunc = std::make_shared<JFunc>(*data.jfunc);
        m_rtemp = data.m_rtemp;
        gasDenT = data.gasDenT;
        oilDenT = data.oilDenT;
        watDenT = data.watDenT;
        stcond = data.stcond;
        m_gas_comp_index = data.m_gas_comp_index;

        return *this;
    }

    void TableManager::initDims(const Deck& deck) {
        using namespace Ewoms::ParserKeywords;

        if (deck.hasKeyword<EQLDIMS>()) {
            const auto& keyword = deck.getKeyword<EQLDIMS>();
            const auto& record = keyword.getRecord(0);
            int ntsequl   = record.getItem<EQLDIMS::NTEQUL>().get< int >(0);
            int nodes_p   = record.getItem<EQLDIMS::DEPTH_NODES_P>().get< int >(0);
            int nodes_tab = record.getItem<EQLDIMS::DEPTH_NODES_TAB>().get< int >(0);
            int nttrvd    = record.getItem<EQLDIMS::NTTRVD>().get< int >(0);
            int ntsrvd    = record.getItem<EQLDIMS::NSTRVD>().get< int >(0);

            m_eqldims = std::make_shared<Eqldims>(ntsequl , nodes_p , nodes_tab , nttrvd , ntsrvd );
        } else
            m_eqldims = std::make_shared<Eqldims>();

        if (deck.hasKeyword<REGDIMS>()) {
            const auto& keyword = deck.getKeyword<REGDIMS>();
            const auto& record = keyword.getRecord(0);
            int ntfip  = record.getItem<REGDIMS::NTFIP>().get< int >(0);
            int nmfipr = record.getItem<REGDIMS::NMFIPR>().get< int >(0);
            int nrfreg = record.getItem<REGDIMS::NRFREG>().get< int >(0);
            int ntfreg = record.getItem<REGDIMS::NTFREG>().get< int >(0);
            int nplmix = record.getItem<REGDIMS::NPLMIX>().get< int >(0);
            m_regdims = std::make_shared<Regdims>( ntfip , nmfipr , nrfreg , ntfreg , nplmix );
        } else
            m_regdims = std::make_shared<Regdims>();
    }

    void TableManager::addTables( const std::string& tableName , size_t numTables) {
        m_simpleTables.emplace(std::make_pair(tableName , TableContainer( numTables )));
    }

    bool TableManager::hasTables( const std::string& tableName ) const {
        auto pair = m_simpleTables.find( tableName );
        if (pair == m_simpleTables.end())
            return false;
        else {
            const auto& tables = pair->second;
            return !tables.empty();
        }
    }

    const TableContainer& TableManager::getTables( const std::string& tableName ) const {
        auto pair = m_simpleTables.find( tableName );
        if (pair == m_simpleTables.end())
            throw std::invalid_argument("No such table collection: " + tableName);
        else
            return pair->second;
    }

    TableContainer& TableManager::forceGetTables( const std::string& tableName , size_t numTables )  {
        auto pair = m_simpleTables.find( tableName );
        if (pair == m_simpleTables.end()) {
            addTables( tableName , numTables );
            pair = m_simpleTables.find( tableName );
        }
        return pair->second;
    }

    const DenT& TableManager::WatDenT() const {
        return this->watDenT;
    }

    const DenT& TableManager::GasDenT() const {
        return this->gasDenT;
    }

    const DenT& TableManager::OilDenT() const {
        return this->oilDenT;
    }

    const StandardCond& TableManager::stCond() const {
        return this->stcond;
    }

    const TableContainer& TableManager::operator[](const std::string& tableName) const {
        return getTables(tableName);
    }

    void TableManager::initSimpleTables(const Deck& deck) {

        addTables( "SWOF" , m_tabdims.getNumSatTables() );
        addTables( "SGWFN", m_tabdims.getNumSatTables() );
        addTables( "SGOF",  m_tabdims.getNumSatTables() );
        addTables( "SLGOF", m_tabdims.getNumSatTables() );
        addTables( "SOF2",  m_tabdims.getNumSatTables() );
        addTables( "SOF3",  m_tabdims.getNumSatTables() );
        addTables( "SWFN",  m_tabdims.getNumSatTables() );
        addTables( "SGFN",  m_tabdims.getNumSatTables() );
        addTables( "SSFN",  m_tabdims.getNumSatTables() );
        addTables( "MSFN",  m_tabdims.getNumSatTables() );

        addTables( "PLYADS", m_tabdims.getNumSatTables() );
        addTables( "PLYROCK", m_tabdims.getNumSatTables());
        addTables( "PLYVISC", m_tabdims.getNumPVTTables());
        addTables( "PLYDHFLF", m_tabdims.getNumPVTTables());

        addTables( "FOAMADS", m_tabdims.getNumSatTables() );
        addTables( "FOAMMOB", m_tabdims.getNumPVTTables() );

        addTables( "PVDG", m_tabdims.getNumPVTTables());
        addTables( "PVDO", m_tabdims.getNumPVTTables());
        addTables( "PVDS", m_tabdims.getNumPVTTables());

        addTables( "SPECHEAT", m_tabdims.getNumPVTTables());
        addTables( "SPECROCK", m_tabdims.getNumSatTables());

        addTables( "OILVISCT", m_tabdims.getNumPVTTables());
        addTables( "WATVISCT", m_tabdims.getNumPVTTables());
        addTables( "GASVISCT", m_tabdims.getNumPVTTables());

        addTables( "PLYMAX", m_regdims->getNPLMIX());
        addTables( "RSVD", m_eqldims->getNumEquilRegions());
        addTables( "RVVD", m_eqldims->getNumEquilRegions());
        addTables( "PBVD", m_eqldims->getNumEquilRegions());
        addTables( "PDVD", m_eqldims->getNumEquilRegions());
        addTables( "SALTVD", m_eqldims->getNumEquilRegions());

        addTables( "AQUTAB", m_aqudims.getNumInfluenceTablesCT());
        {
            size_t numMiscibleTables = ParserKeywords::MISCIBLE::NTMISC::defaultValue;
            if (deck.hasKeyword<ParserKeywords::MISCIBLE>()) {
                const auto& keyword = deck.getKeyword<ParserKeywords::MISCIBLE>();
                const auto& record = keyword.getRecord(0);
                numMiscibleTables =  static_cast<size_t>(record.getItem<ParserKeywords::MISCIBLE::NTMISC>().get< int >(0));
            }
            addTables( "SORWMIS", numMiscibleTables);
            addTables( "SGCWMIS", numMiscibleTables);
            addTables( "MISC",    numMiscibleTables);
            addTables( "PMISC",   numMiscibleTables);
            addTables( "TLPMIXPA",numMiscibleTables);
        }

        {
            size_t numEndScaleTables = ParserKeywords::ENDSCALE::NUM_TABLES::defaultValue;

            if (deck.hasKeyword<ParserKeywords::ENDSCALE>()) {
                const auto& keyword = deck.getKeyword<ParserKeywords::ENDSCALE>();
                const auto& record = keyword.getRecord(0);
                numEndScaleTables = static_cast<size_t>(record.getItem<ParserKeywords::ENDSCALE::NUM_TABLES>().get< int >(0));
            }

            addTables( "ENKRVD", numEndScaleTables);
            addTables( "ENPTVD", numEndScaleTables);
            addTables( "IMKRVD", numEndScaleTables);
            addTables( "IMPTVD", numEndScaleTables);
        }
        {
            size_t numRocktabTables = ParserKeywords::ROCKCOMP::NTROCC::defaultValue;

            if (deck.hasKeyword<ParserKeywords::ROCKCOMP>()) {
                const auto& keyword = deck.getKeyword<ParserKeywords::ROCKCOMP>();
                const auto& record = keyword.getRecord(0);
                numRocktabTables = static_cast<size_t>(record.getItem<ParserKeywords::ROCKCOMP::NTROCC>().get< int >(0));
            }
            addTables( "ROCKTAB", numRocktabTables);
            addTables( "ROCKWNOD", numRocktabTables);
            addTables( "OVERBURD", numRocktabTables);
        }

        initSimpleTableContainer<SgwfnTable>(deck, "SGWFN", m_tabdims.getNumSatTables());
        initSimpleTableContainer<Sof2Table>(deck, "SOF2" , m_tabdims.getNumSatTables());
        initSimpleTableContainer<Sof3Table>(deck, "SOF3" , m_tabdims.getNumSatTables());
        {
            initSimpleTableContainerWithJFunc<SwofTable>(deck, "SWOF", m_tabdims.getNumSatTables());
            initSimpleTableContainerWithJFunc<SgofTable>(deck, "SGOF", m_tabdims.getNumSatTables());
            initSimpleTableContainerWithJFunc<SwfnTable>(deck, "SWFN", m_tabdims.getNumSatTables());
            initSimpleTableContainerWithJFunc<SgfnTable>(deck, "SGFN", m_tabdims.getNumSatTables());
            initSimpleTableContainerWithJFunc<SlgofTable>(deck, "SLGOF", m_tabdims.getNumSatTables());

        }
        initSimpleTableContainer<SsfnTable>(deck, "SSFN" , m_tabdims.getNumSatTables());
        initSimpleTableContainer<MsfnTable>(deck, "MSFN" , m_tabdims.getNumSatTables());

        initSimpleTableContainer<RsvdTable>(deck, "RSVD" , m_eqldims->getNumEquilRegions());
        initSimpleTableContainer<RvvdTable>(deck, "RVVD" , m_eqldims->getNumEquilRegions());
        initSimpleTableContainer<PbvdTable>(deck, "PBVD" , m_eqldims->getNumEquilRegions());
        initSimpleTableContainer<PdvdTable>(deck, "PDVD" , m_eqldims->getNumEquilRegions());
        initSimpleTableContainer<SaltvdTable>(deck, "SALTVD" , m_eqldims->getNumEquilRegions());
        initSimpleTableContainer<AqutabTable>(deck, "AQUTAB" , m_aqudims.getNumInfluenceTablesCT());
        {
            size_t numEndScaleTables = ParserKeywords::ENDSCALE::NUM_TABLES::defaultValue;

            if (deck.hasKeyword<ParserKeywords::ENDSCALE>()) {
                const auto& keyword = deck.getKeyword<ParserKeywords::ENDSCALE>();
                const auto& record = keyword.getRecord(0);
                numEndScaleTables = static_cast<size_t>(record.getItem<ParserKeywords::ENDSCALE::NUM_TABLES>().get< int >(0));
            }

            initSimpleTableContainer<EnkrvdTable>( deck , "ENKRVD", numEndScaleTables);
            initSimpleTableContainer<EnptvdTable>( deck , "ENPTVD", numEndScaleTables);
            initSimpleTableContainer<ImkrvdTable>( deck , "IMKRVD", numEndScaleTables);
            initSimpleTableContainer<ImptvdTable>( deck , "IMPTVD", numEndScaleTables);
        }

        {
            size_t numMiscibleTables = ParserKeywords::MISCIBLE::NTMISC::defaultValue;
            if (deck.hasKeyword<ParserKeywords::MISCIBLE>()) {
                const auto& keyword = deck.getKeyword<ParserKeywords::MISCIBLE>();
                const auto& record = keyword.getRecord(0);
                numMiscibleTables =  static_cast<size_t>(record.getItem<ParserKeywords::MISCIBLE::NTMISC>().get< int >(0));
            }
            initSimpleTableContainer<SorwmisTable>(deck, "SORWMIS", numMiscibleTables);
            initSimpleTableContainer<SgcwmisTable>(deck, "SGCWMIS", numMiscibleTables);
            initSimpleTableContainer<MiscTable>(deck, "MISC", numMiscibleTables);
            initSimpleTableContainer<PmiscTable>(deck, "PMISC", numMiscibleTables);
            initSimpleTableContainer<TlpmixpaTable>(deck, "TLPMIXPA", numMiscibleTables);

        }
        {
            size_t numRocktabTables = ParserKeywords::ROCKCOMP::NTROCC::defaultValue;

            if (deck.hasKeyword<ParserKeywords::ROCKCOMP>()) {
                const auto& keyword = deck.getKeyword<ParserKeywords::ROCKCOMP>();
                const auto& record = keyword.getRecord(0);
                numRocktabTables = static_cast<size_t>(record.getItem<ParserKeywords::ROCKCOMP::NTROCC>().get< int >(0));
            }
            initSimpleTableContainer<RockwnodTable>(deck, "ROCKWNOD", numRocktabTables);
            initSimpleTableContainer<OverburdTable>(deck, "OVERBURD", numRocktabTables);
        }

        initSimpleTableContainer<PvdgTable>(deck, "PVDG", m_tabdims.getNumPVTTables());
        initSimpleTableContainer<PvdoTable>(deck, "PVDO", m_tabdims.getNumPVTTables());
        initSimpleTableContainer<PvdsTable>(deck, "PVDS", m_tabdims.getNumPVTTables());
        initSimpleTableContainer<SpecheatTable>(deck, "SPECHEAT", m_tabdims.getNumPVTTables());
        initSimpleTableContainer<SpecrockTable>(deck, "SPECROCK", m_tabdims.getNumSatTables());
        initSimpleTableContainer<OilvisctTable>(deck, "OILVISCT", m_tabdims.getNumPVTTables());
        initSimpleTableContainer<WatvisctTable>(deck, "WATVISCT", m_tabdims.getNumPVTTables());

        initSimpleTableContainer<PlyadsTable>(deck, "PLYADS", m_tabdims.getNumSatTables());
        initSimpleTableContainer<PlyviscTable>(deck, "PLYVISC", m_tabdims.getNumPVTTables());
        initSimpleTableContainer<PlydhflfTable>(deck, "PLYDHFLF", m_tabdims.getNumPVTTables());

        initSimpleTableContainer<FoamadsTable>(deck, "FOAMADS", m_tabdims.getNumSatTables());
        initSimpleTableContainer<FoammobTable>(deck, "FOAMMOB", m_tabdims.getNumPVTTables());

        initPlyrockTables(deck);
        initPlymaxTables(deck);
        initGasvisctTables(deck);
        initRTempTables(deck);
        initRocktabTables(deck);
        initPlyshlogTables(deck);
        initPlymwinjTables(deck);
        initSkprpolyTables(deck);
        initSkprwatTables(deck);
    }

    void TableManager::initRTempTables(const Deck& deck) {
        // the temperature vs depth table. the problem here is that
        // the TEMPVD (E300) and RTEMPVD (E300 + E100) keywords are
        // synonymous, but we want to provide only a single cannonical
        // API here, so we jump through some small hoops...
        if (deck.hasKeyword("TEMPVD") && deck.hasKeyword("RTEMPVD"))
            throw std::invalid_argument("The TEMPVD and RTEMPVD tables are mutually exclusive!");
        else if (deck.hasKeyword("TEMPVD"))
            initSimpleTableContainer<RtempvdTable>(deck, "TEMPVD", "RTEMPVD", m_eqldims->getNumEquilRegions());
        else if (deck.hasKeyword("RTEMPVD"))
            initSimpleTableContainer<RtempvdTable>(deck, "RTEMPVD", "RTEMPVD" , m_eqldims->getNumEquilRegions());
    }

    void TableManager::initGasvisctTables(const Deck& deck) {

        const std::string keywordName = "GASVISCT";
        size_t numTables = m_tabdims.getNumPVTTables();

        if (!deck.hasKeyword(keywordName))
            return; // the table is not featured by the deck...

        auto& container = forceGetTables(keywordName , numTables);

        if (deck.count(keywordName) > 1) {
            complainAboutAmbiguousKeyword(deck, keywordName);
            return;
        }

        const auto& tableKeyword = deck.getKeyword(keywordName);
        for (size_t tableIdx = 0; tableIdx < tableKeyword.size(); ++tableIdx) {
            const auto& tableRecord = tableKeyword.getRecord( tableIdx );
            const auto& dataItem = tableRecord.getItem( 0 );
            if (dataItem.data_size() > 0) {
                std::shared_ptr<GasvisctTable> table = std::make_shared<GasvisctTable>( deck , dataItem );
                container.addTable( tableIdx , table );
            }
        }
    }

    void TableManager::initPlyshlogTables(const Deck& deck) {
        const std::string keywordName = "PLYSHLOG";

        if (!deck.hasKeyword(keywordName)) {
            return;
        }

        if (!deck.count(keywordName)) {
            complainAboutAmbiguousKeyword(deck, keywordName);
            return;
        }
        size_t numTables = m_tabdims.getNumPVTTables();
        auto& container = forceGetTables(keywordName , numTables);
        const auto& tableKeyword = deck.getKeyword(keywordName);

        if (tableKeyword.size() > 2) {
            std::string msg = "The Parser does currently NOT support the alternating record schema used in PLYSHLOG";
            throw std::invalid_argument( msg );
        }

        for (size_t tableIdx = 0; tableIdx < tableKeyword.size(); tableIdx += 2) {
            const auto& indexRecord = tableKeyword.getRecord( tableIdx );
            const auto& dataRecord = tableKeyword.getRecord( tableIdx + 1);
            const auto& dataItem = dataRecord.getItem( 0 );
            if (dataItem.data_size() > 0) {
                std::shared_ptr<PlyshlogTable> table = std::make_shared<PlyshlogTable>(indexRecord , dataRecord);
                container.addTable( tableIdx , table );
            }
        }
    }

    void TableManager::initPlymwinjTables(const Deck& deck) {
        if (!deck.hasKeyword("PLYMWINJ")) {
            return;
        }

        const size_t num_tables = deck.count("PLYMWINJ");
        const auto& keywords = deck.getKeywordList<ParserKeywords::PLYMWINJ>();
        for (size_t i = 0; i < num_tables; ++i) {
            const DeckKeyword &keyword = *keywords[i];

            // not const for std::move
            PlymwinjTable table(keyword);

            // we need to check the value of the table_number against the allowed ones
            const int table_number = table.getTableNumber();
            // we should check if the table_number is valid
            if (m_plymwinjTables.find(table_number) == m_plymwinjTables.end()) {
                m_plymwinjTables.insert(std::make_pair(table_number, std::move(table)));
            } else {
                throw std::invalid_argument("Duplicated table number "
                                            + std::to_string(table_number)
                                            + " for keyword PLYMWINJ found");
            }
        }
    }

    void TableManager::initSkprwatTables(const Ewoms::Deck &deck) {
        if (!deck.hasKeyword("SKPRWAT")) {
            return;
        }

        const size_t num_tables = deck.count("SKPRWAT");
        const auto& keywords = deck.getKeywordList<ParserKeywords::SKPRWAT>();
        for (size_t i = 0; i < num_tables; ++i) {
            const DeckKeyword &keyword = *keywords[i];

            // not const for std::move
            SkprwatTable table(keyword);

            // we need to check the value of the table_number against the allowed ones
            const int table_number = table.getTableNumber();
            // we should check if the table_number is valid
            if (m_skprwatTables.find(table_number) == m_skprwatTables.end()) {
                m_skprwatTables.insert(std::make_pair(table_number, std::move(table)));
            } else {
                throw std::invalid_argument("Duplicated table number "
                                            + std::to_string(table_number)
                                            + " for keyword SKPRWAT found");
            }
        }
    }

    void TableManager::initSkprpolyTables(const Ewoms::Deck &deck) {
        if (!deck.hasKeyword("SKPRPOLY")) {
            return;
        }

        const size_t num_tables = deck.count("SKPRPOLY");
        const auto& keywords = deck.getKeywordList<ParserKeywords::SKPRPOLY>();
        for (size_t i = 0; i < num_tables; ++i) {
            const DeckKeyword &keyword = *keywords[i];

            // not const for std::move
            SkprpolyTable table(keyword);

            // we need to check the value of the table_number against the allowed ones
            const int table_number = table.getTableNumber();
            // we should check if the table_number is valid
            if (m_skprpolyTables.find(table_number) == m_skprpolyTables.end()) {
                m_skprpolyTables.insert(std::make_pair(table_number, std::move(table)));
            } else {
                throw std::invalid_argument("Duplicated table number "
                                            + std::to_string(table_number)
                                            + " for keyword SKPRPOLY found");
            }
        }
    }

    void TableManager::initPlyrockTables(const Deck& deck) {
        size_t numTables = m_tabdims.getNumSatTables();
        const std::string keywordName = "PLYROCK";
        if (!deck.hasKeyword(keywordName)) {
            return;
        }

        if (!deck.count(keywordName)) {
            complainAboutAmbiguousKeyword(deck, keywordName);
            return;
        }

        const auto& keyword = deck.getKeyword<ParserKeywords::PLYROCK>();
        auto& container = forceGetTables(keywordName , numTables);
        for (size_t tableIdx = 0; tableIdx < keyword.size(); ++tableIdx) {
            const auto& tableRecord = keyword.getRecord( tableIdx );
            std::shared_ptr<PlyrockTable> table = std::make_shared<PlyrockTable>(tableRecord);
            container.addTable( tableIdx , table );
        }
    }

    void TableManager::initPlymaxTables(const Deck& deck) {
        size_t numTables = m_regdims->getNPLMIX();
        const std::string keywordName = "PLYMAX";
        if (!deck.hasKeyword(keywordName)) {
            return;
        }

        if (!deck.count(keywordName)) {
            complainAboutAmbiguousKeyword(deck, keywordName);
            return;
        }

        const auto& keyword = deck.getKeyword<ParserKeywords::PLYMAX>();
        auto& container = forceGetTables(keywordName , numTables);
        for (size_t tableIdx = 0; tableIdx < keyword.size(); ++tableIdx) {
            const auto& tableRecord = keyword.getRecord( tableIdx );
            std::shared_ptr<PlymaxTable> table = std::make_shared<PlymaxTable>( tableRecord );
            container.addTable( tableIdx , table );
        }
    }

    void TableManager::initRocktabTables(const Deck& deck) {
        if (!deck.hasKeyword("ROCKTAB"))
            return; // ROCKTAB is not featured by the deck...

        if (deck.count("ROCKTAB") > 1) {
            complainAboutAmbiguousKeyword(deck, "ROCKTAB");
            return;
        }
        const auto& rockcompKeyword = deck.getKeyword<ParserKeywords::ROCKCOMP>();
        const auto& record = rockcompKeyword.getRecord( 0 );
        size_t numTables = record.getItem<ParserKeywords::ROCKCOMP::NTROCC>().get< int >(0);
        auto& container = forceGetTables("ROCKTAB" , numTables);
        const auto rocktabKeyword = deck.getKeyword("ROCKTAB");

        bool isDirectional = deck.hasKeyword<ParserKeywords::RKTRMDIR>();
        bool useStressOption = false;
        if (deck.hasKeyword<ParserKeywords::ROCKOPTS>()) {
            const auto rockoptsKeyword = deck.getKeyword<ParserKeywords::ROCKOPTS>();
            const auto& rockoptsRecord = rockoptsKeyword.getRecord(0);
            const auto& item = rockoptsRecord.getItem<ParserKeywords::ROCKOPTS::METHOD>();
            useStressOption = (item.getTrimmedString(0) == "STRESS");
        }

        for (size_t tableIdx = 0; tableIdx < rocktabKeyword.size(); ++tableIdx) {
            const auto& tableRecord = rocktabKeyword.getRecord( tableIdx );
            const auto& dataItem = tableRecord.getItem( 0 );
            if (dataItem.data_size() > 0) {
                std::shared_ptr<RocktabTable> table = std::make_shared<RocktabTable>( dataItem , isDirectional, useStressOption );
                container.addTable( tableIdx , table );
            }
        }
    }

        size_t TableManager::numFIPRegions() const {
        size_t ntfip = m_tabdims.getNumFIPRegions();
        if (m_regdims->getNTFIP( ) > ntfip)
            return m_regdims->getNTFIP( );
        else
            return ntfip;
    }

    const Tabdims& TableManager::getTabdims() const {
        return m_tabdims;
    }

    const Eqldims& TableManager::getEqldims() const {
        return *m_eqldims;
    }

    const Aqudims& TableManager::getAqudims() const {
        return m_aqudims;
    }

    const Regdims& TableManager::getRegdims() const {
        return *this->m_regdims;
    }

    /*
      const std::vector<SwofTable>& TableManager::getSwofTables() const {
        return m_swofTables;
        }
    */

    const TableContainer& TableManager::getSwofTables() const {
        return getTables("SWOF");
    }

    const TableContainer& TableManager::getSgwfnTables() const {
        return getTables("SGWFN");
    }

    const TableContainer& TableManager::getSlgofTables() const {
        return getTables("SLGOF");
    }

    const TableContainer& TableManager::getSgofTables() const {
        return getTables("SGOF");
    }

    const TableContainer& TableManager::getSof2Tables() const {
        return getTables("SOF2");
    }

    const TableContainer& TableManager::getSof3Tables() const {
        return getTables("SOF3");
    }

    const TableContainer& TableManager::getSwfnTables() const {
        return getTables("SWFN");
    }

    const TableContainer& TableManager::getSgfnTables() const {
        return getTables("SGFN");
    }

    const TableContainer& TableManager::getSsfnTables() const {
        return getTables("SSFN");
    }

    const TableContainer& TableManager::getRsvdTables() const {
        return getTables("RSVD");
    }

    const TableContainer& TableManager::getRvvdTables() const {
        return getTables("RVVD");
    }

    const TableContainer& TableManager::getPbvdTables() const {
        return getTables("PBVD");
    }

    const TableContainer& TableManager::getPdvdTables() const {
        return getTables("PDVD");
    }

    const TableContainer& TableManager::getSaltvdTables() const {
        return getTables("SALTVD");
    }

    const TableContainer& TableManager::getEnkrvdTables() const {
        return getTables("ENKRVD");
    }

    const TableContainer& TableManager::getEnptvdTables() const {
        return getTables("ENPTVD");
    }

    const TableContainer& TableManager::getImkrvdTables() const {
        return getTables("IMKRVD");
    }

    const TableContainer& TableManager::getImptvdTables() const {
        return getTables("IMPTVD");
    }

    const TableContainer& TableManager::getPvdgTables() const {
        return getTables("PVDG");
    }

    const TableContainer& TableManager::getPvdoTables() const {
        return getTables("PVDO");
    }

    const TableContainer& TableManager::getPvdsTables() const {
        return getTables("PVDS");
    }

    const TableContainer& TableManager::getSpecheatTables() const {
        return getTables("SPECHEAT");
    }

    const TableContainer& TableManager::getSpecrockTables() const {
        return getTables("SPECROCK");
    }

    const TableContainer& TableManager::getOilvisctTables() const {
        return getTables("OILVISCT");
    }

    const TableContainer& TableManager::getWatvisctTables() const {
        return getTables("WATVISCT");
    }

    const TableContainer& TableManager::getGasvisctTables() const {
        return getTables("GASVISCT");
    }

    const TableContainer& TableManager::getRtempvdTables() const {
        return getTables("RTEMPVD");
    }

    const TableContainer& TableManager::getRocktabTables() const {
        return getTables("ROCKTAB");
    }

    const TableContainer& TableManager::getPlyadsTables() const {
        return getTables("PLYADS");
    }

    const TableContainer& TableManager::getPlyviscTables() const {
        return getTables("PLYVISC");
    }

    const TableContainer& TableManager::getPlydhflfTables() const {
        return getTables("PLYDHFL");
    }

    const TableContainer& TableManager::getPlymaxTables() const {
        return getTables("PLYMAX");
    }

    const TableContainer& TableManager::getPlyrockTables() const {
        return getTables("PLYROCK");
    }

    const TableContainer& TableManager::getPlyshlogTables() const {
        return getTables("PLYSHLOG");
    }

    const TableContainer& TableManager::getAqutabTables() const {
        return getTables("AQUTAB");
    }

    const TableContainer& TableManager::getFoamadsTables() const {
        return getTables("FOAMADS");
    }

    const TableContainer& TableManager::getFoammobTables() const {
        return getTables("FOAMMOB");
    }

    const std::vector<PvtgTable>& TableManager::getPvtgTables() const {
        return m_pvtgTables;
    }

    const std::vector<PvtoTable>& TableManager::getPvtoTables() const {
        return m_pvtoTables;
    }

    const std::vector<Rock2dTable>& TableManager::getRock2dTables() const {
        return m_rock2dTables;
    }

    const std::vector<Rock2dtrTable>& TableManager::getRock2dtrTables() const {
        return m_rock2dtrTables;
    }

    const TableContainer& TableManager::getRockwnodTables() const {
        return getTables("ROCKWNOD");
    }

    const TableContainer& TableManager::getOverburdTables() const {
        return getTables("OVERBURD");
    }

    const PvtwTable& TableManager::getPvtwTable() const {
        return this->m_pvtwTable;
    }

    const std::vector<PvtwsaltTable>& TableManager::getPvtwSaltTables() const {
        return this->m_pvtwsaltTables;
    }

    const std::vector<BrineDensityTable>& TableManager::getBrineDensityTables() const {
        return this->m_bdensityTables;
    }

    const std::vector<SolventDensityTable>& TableManager::getSolventDensityTables() const {
        return this->m_sdensityTables;
    }

    const PvcdoTable& TableManager::getPvcdoTable() const {
        return this->m_pvcdoTable;
    }

    const DensityTable& TableManager::getDensityTable() const {
        return this->m_densityTable;
    }

    const RockTable& TableManager::getRockTable() const {
        return this->m_rockTable;
    }

    const ViscrefTable& TableManager::getViscrefTable() const {
        return this->m_viscrefTable;
    }

    const WatdentTable& TableManager::getWatdentTable() const {
        return this->m_watdentTable;
    }

    const TableContainer& TableManager::getMsfnTables() const {
        return getTables("MSFN");
    }
    const TableContainer& TableManager::getPmiscTables() const {
        return getTables("PMISC");
    }
    const TableContainer& TableManager::getMiscTables() const {
        return getTables("MISC");
    }
    const TableContainer& TableManager::getSgcwmisTables() const {
        return getTables("SGCWMIS");
    }
    const TableContainer& TableManager::getSorwmisTables() const {
        return getTables("SORWMIS");
    }
    const TableContainer& TableManager::getTlpmixpaTables() const {
        return getTables("TLPMIXPA");
    }

    const PlmixparTable& TableManager::getPlmixparTable() const {
        return m_plmixparTable;
    }

    const ShrateTable& TableManager::getShrateTable() const {
        return m_shrateTable;
    }

    const Stone1exTable& TableManager::getStone1exTable() const {
        return m_stone1exTable;
    }

    const TlmixparTable& TableManager::getTlmixparTable() const {
        return m_tlmixparTable;
    }

    const JFunc& TableManager::getJFunc() const {
        if (!jfunc)
            throw std::invalid_argument("Cannot get JFUNC table when JFUNC not in deck");
        return *jfunc;
    }

    const PlyvmhTable& TableManager::getPlyvmhTable() const {
        return m_plyvmhTable;
    }

    const std::map<int, PlymwinjTable>& TableManager::getPlymwinjTables() const {
        return m_plymwinjTables;
    }

    const std::map<int, SkprwatTable>& TableManager::getSkprwatTables() const {
        return m_skprwatTables;
    }

    const std::map<int, SkprpolyTable>& TableManager::getSkprpolyTables() const {
        return m_skprpolyTables;
    }

    const std::map<std::string, TableContainer>& TableManager::getSimpleTables() const {
        return m_simpleTables;
    }

    bool TableManager::useImptvd() const {
        return hasImptvd;
    }

    bool TableManager::useEnptvd() const {
        return hasEnptvd;
    }

    bool TableManager::useEqlnum() const {
        return hasEqlnum;
    }

    bool TableManager::useShrate() const {
        return hasShrate;
    }

    bool TableManager::useJFunc() const {
        if (jfunc)
            return true;
        else
            return false;
    }

    void TableManager::complainAboutAmbiguousKeyword(const Deck& deck, const std::string& keywordName) {
        OpmLog::error("The " + keywordName + " keyword must be unique in the deck. Ignoring all!");
        const auto& keywords = deck.getKeywordList(keywordName);
        for (size_t i = 0; i < keywords.size(); ++i) {
            std::string msg = "Ambiguous keyword "+keywordName+" defined here";
            OpmLog::error(Log::fileMessage(keywords[i]->location(), msg));
        }
    }

    double TableManager::rtemp() const {
        return this->m_rtemp;
    }

    std::size_t TableManager::gas_comp_index() const {
        return this->m_gas_comp_index;
    }

    bool TableManager::operator==(const TableManager& data) const {
        bool jfuncOk = false;
        if (jfunc && data.jfunc)
            jfuncOk = *jfunc == *data.jfunc;
        if (!jfunc && !data.jfunc)
            jfuncOk = true;

        return m_simpleTables == data.m_simpleTables &&
               m_pvtgTables == data.m_pvtgTables &&
               m_pvtoTables == data.m_pvtoTables &&
               m_rock2dTables == data.m_rock2dTables &&
               m_rock2dtrTables == data.m_rock2dtrTables &&
               m_pvtwTable == data.m_pvtwTable &&
               m_pvcdoTable == data.m_pvcdoTable &&
               m_densityTable == data.m_densityTable &&
               m_plmixparTable == data.m_plmixparTable &&
               m_plyvmhTable == data.m_plyvmhTable &&
               m_shrateTable == data.m_shrateTable &&
               m_stone1exTable == data.m_stone1exTable &&
               m_tlmixparTable == data.m_tlmixparTable &&
               m_viscrefTable == data.m_viscrefTable &&
               m_watdentTable == data.m_watdentTable &&
               m_pvtwsaltTables == data.m_pvtwsaltTables &&
               m_bdensityTables == data.m_bdensityTables &&
               m_sdensityTables == data.m_sdensityTables &&
               m_plymwinjTables == data.m_plymwinjTables &&
               m_skprwatTables == data.m_skprwatTables &&
               m_skprpolyTables == data.m_skprpolyTables &&
               m_tabdims == data.m_tabdims &&
               *m_regdims == *data.m_regdims &&
               *m_eqldims == *data.m_eqldims &&
               m_aqudims == data.m_aqudims &&
               hasImptvd == data.hasImptvd &&
               hasEnptvd == data.hasEnptvd &&
               hasEqlnum == data.hasEqlnum &&
               hasShrate == data.hasShrate &&
               gasDenT == data.gasDenT &&
               oilDenT == data.oilDenT &&
               watDenT == data.watDenT &&
               stcond == data.stcond &&
               jfuncOk &&
               m_rtemp == data.m_rtemp &&
               m_gas_comp_index == data.m_gas_comp_index;
    }

    void TableManager::initSolventTables(const Deck& deck,  std::vector<SolventDensityTable>& solventtables) {
        size_t numTables = m_tabdims.getNumPVTTables();
        solventtables.resize(numTables);

        const auto& keyword = deck.getKeyword("SDENSITY");
        size_t numEntries = keyword.size();
        assert(numEntries == numTables);
        for (unsigned lineIdx = 0; lineIdx < numEntries; ++lineIdx) {
            solventtables[lineIdx].init(keyword.getRecord(lineIdx));
        }
    }
}

