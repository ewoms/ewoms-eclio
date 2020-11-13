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
#include "config.h"

#include <ewoms/eclio/output/writeinit.hh>

#include <ewoms/eclio/io/outputstream.hh>

#include <ewoms/eclio/output/data/solution.hh>
#include <ewoms/eclio/output/logihead.hh>
#include <ewoms/eclio/output/tables.hh>
#include <ewoms/eclio/output/writerestarthelpers.hh>

#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/endpointscaling.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fieldpropsmanager.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/nnc.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>

#include <ewoms/eclio/parser/units/unitsystem.hh>

#include <cstddef>
#include <exception>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace {

    struct CellProperty
    {
        std::string                name;
        ::Ewoms::UnitSystem::measure unit;
    };

    using Properties = std::vector<CellProperty>;

    class ScalingVectors
    {
    public:
        ScalingVectors& withHysteresis(const bool active);
        ScalingVectors& collect(const ::Ewoms::Phases& ph);

        const Properties& getVectors() const
        {
            return this->vectors_;
        }

    private:
        Properties vectors_{};
        bool       useHysteresis_{false};

        void insertScaledWaterEndPoints();
        void insertScaledGasEndPoints();
        void insertScaledOilEndPoints(const ::Ewoms::Phases& ph);
        void insertScaledRelpermValues(const ::Ewoms::Phases& ph);
        void insertScaledCapillaryPressure(const ::Ewoms::Phases& ph);
        void insertImbibitionPoints();
    };

    ScalingVectors& ScalingVectors::withHysteresis(const bool active)
    {
        this->useHysteresis_ = active;

        return *this;
    }

    ScalingVectors& ScalingVectors::collect(const ::Ewoms::Phases& ph)
    {
        if (ph.active(::Ewoms::Phase::WATER)) {
            this->insertScaledWaterEndPoints();
        }

        if (ph.active(::Ewoms::Phase::GAS)) {
            this->insertScaledGasEndPoints();
        }

        if (ph.active(::Ewoms::Phase::OIL)) {
            this->insertScaledOilEndPoints(ph);
        }

        this->insertScaledRelpermValues(ph);

        if (ph.active(::Ewoms::Phase::OIL)) {
            this->insertScaledCapillaryPressure(ph);
        }

        if (this->useHysteresis_) {
            // Run uses hysteresis.  Output scaled imbibition curve too.
            this->insertImbibitionPoints();
        }

        return *this;
    }

    void ScalingVectors::insertScaledWaterEndPoints()
    {
        this->vectors_.insert(this->vectors_.end(),
        {
            {"SWL" , ::Ewoms::UnitSystem::measure::identity },
            {"SWCR", ::Ewoms::UnitSystem::measure::identity },
            {"SWU" , ::Ewoms::UnitSystem::measure::identity },
        });
    }

    void ScalingVectors::insertScaledGasEndPoints()
    {
        this->vectors_.insert(this->vectors_.end(),
        {
            {"SGL" , ::Ewoms::UnitSystem::measure::identity },
            {"SGCR", ::Ewoms::UnitSystem::measure::identity },
            {"SGU" , ::Ewoms::UnitSystem::measure::identity },
        });
    }

    void ScalingVectors::insertScaledOilEndPoints(const ::Ewoms::Phases& ph)
    {
        if (ph.active(::Ewoms::Phase::WATER)) {
            this->vectors_.push_back(CellProperty {
                "SOWCR", ::Ewoms::UnitSystem::measure::identity
            });
        }

        if (ph.active(::Ewoms::Phase::GAS)) {
            this->vectors_.push_back(CellProperty {
                "SOGCR", ::Ewoms::UnitSystem::measure::identity
            });
        }
    }

    void ScalingVectors::insertScaledRelpermValues(const ::Ewoms::Phases& ph)
    {
        if (ph.active(::Ewoms::Phase::WATER)) {
            this->vectors_.insert(this->vectors_.end(),
            {
                {"KRW" , ::Ewoms::UnitSystem::measure::identity },
                {"KRWR", ::Ewoms::UnitSystem::measure::identity },
            });
        }

        if (ph.active(::Ewoms::Phase::GAS)) {
            this->vectors_.insert(this->vectors_.end(),
            {
                {"KRG" , ::Ewoms::UnitSystem::measure::identity },
                {"KRGR", ::Ewoms::UnitSystem::measure::identity },
            });
        }

        if (ph.active(::Ewoms::Phase::OIL)) {
            this->vectors_.push_back(CellProperty {
                "KRO", ::Ewoms::UnitSystem::measure::identity
            });

            if (ph.active(::Ewoms::Phase::WATER)) {
                this->vectors_.push_back(CellProperty {
                    "KRORW", ::Ewoms::UnitSystem::measure::identity
                });
            }

            if (ph.active(::Ewoms::Phase::GAS)) {
                this->vectors_.push_back(CellProperty {
                    "KRORG", ::Ewoms::UnitSystem::measure::identity
                });
            }
        }
    }

    void ScalingVectors::insertScaledCapillaryPressure(const ::Ewoms::Phases& ph)
    {
        if (ph.active(::Ewoms::Phase::WATER)) {
            this->vectors_.insert(this->vectors_.end(),
            {
                {"SWLPC", ::Ewoms::UnitSystem::measure::identity },
                {"PCW"  , ::Ewoms::UnitSystem::measure::pressure },
            });
        }

        if (ph.active(::Ewoms::Phase::GAS)) {
            this->vectors_.insert(this->vectors_.end(),
            {
                {"SGLPC", ::Ewoms::UnitSystem::measure::identity },
                {"PCG"  , ::Ewoms::UnitSystem::measure::pressure },
            });
        }
    }

    void ScalingVectors::insertImbibitionPoints()
    {
        const auto start = static_cast<std::string::size_type>(0);
        const auto count = static_cast<std::string::size_type>(1);
        const auto npt   = this->vectors_.size();

        this->vectors_.reserve(2 * npt);

        for (auto i = 0*npt; i < npt; ++i) {
            auto pt = this->vectors_[i]; // Copy, preserve unit of measure.

            // Imbibition vector.  E.g., SOWCR -> ISOWCR
            pt.name.insert(start, count, 'I');

            this->vectors_.push_back(std::move(pt));
        }
    }

    // =================================================================

    std::vector<float> singlePrecision(const std::vector<double>& x)
    {
        return { x.begin(), x.end() };
    }

    ::Ewoms::RestartIO::LogiHEAD::PVTModel
    pvtFlags(const ::Ewoms::Runspec& rspec, const ::Ewoms::TableManager& tabMgr)
    {
        auto pvt = ::Ewoms::RestartIO::LogiHEAD::PVTModel{};

        const auto& phases = rspec.phases();

        pvt.isLiveOil = phases.active(::Ewoms::Phase::OIL) &&
            !tabMgr.getPvtoTables().empty();

        pvt.isWetGas = phases.active(::Ewoms::Phase::GAS) &&
            !tabMgr.getPvtgTables().empty();

        pvt.constComprOil = phases.active(::Ewoms::Phase::OIL) &&
            !(pvt.isLiveOil ||
              tabMgr.hasTables("PVDO") ||
              tabMgr.getPvcdoTable().empty());

        return pvt;
    }

    ::Ewoms::RestartIO::LogiHEAD::SatfuncFlags
    satfuncFlags(const ::Ewoms::Runspec& rspec)
    {
        auto flags = ::Ewoms::RestartIO::LogiHEAD::SatfuncFlags{};

        const auto& eps = rspec.endpointScaling();
        if (eps) {
            flags.useEndScale       = true;
            flags.useDirectionalEPS = eps.directional();
            flags.useReversibleEPS  = eps.reversible();
            flags.useAlternateEPS   = eps.threepoint();
        }

        return flags;
    }

    std::vector<bool> logihead(const ::Ewoms::EclipseState& es)
    {
        const auto& rspec   = es.runspec();
        const auto& wsd     = rspec.wellSegmentDimensions();
        const auto& hystPar = rspec.hysterPar();

        const auto lh = ::Ewoms::RestartIO::LogiHEAD{}
            .variousParam(false, false, wsd.maxSegmentedWells(), hystPar.active())
            .pvtModel(pvtFlags(rspec, es.getTableManager()))
            .saturationFunction(satfuncFlags(rspec))
            ;

        return lh.data();
    }

    void writeInitFileHeader(const ::Ewoms::EclipseState&      es,
                             const ::Ewoms::EclipseGrid&       grid,
                             const ::Ewoms::Schedule&          sched,
                             Ewoms::EclIO::OutputStream::Init& initFile)
    {
        {
            const auto ih = ::Ewoms::RestartIO::Helpers::
                createInteHead(es, grid, sched, 0.0, 0, 0, 0);

            initFile.write("INTEHEAD", ih);
        }

        {
            initFile.write("LOGIHEAD", logihead(es));
        }

        {
            const auto dh = ::Ewoms::RestartIO::Helpers::
                createDoubHead(es, sched, 0, 0.0, 0.0);

            initFile.write("DOUBHEAD", dh);
        }
    }

    void writePoreVolume(const ::Ewoms::EclipseState&        es,
                         const ::Ewoms::UnitSystem&          units,
                         ::Ewoms::EclIO::OutputStream::Init& initFile)
    {
        auto porv = es.globalFieldProps().porv(true);
        units.from_si(::Ewoms::UnitSystem::measure::volume, porv);
        initFile.write("PORV", singlePrecision(porv));
    }

    void writeIntegerCellProperties(const ::Ewoms::EclipseState&        es,
                                    ::Ewoms::EclIO::OutputStream::Init& initFile)
    {

        // The INIT file should always contain PVT, saturation function,
        // equilibration, and fluid-in-place region vectors.  Call
        // assertKeyword() here--on a 'const' GridProperties object--to
        // invoke the autocreation property, and ensure that the keywords
        // exist in the properties container.
        const auto& fp = es.globalFieldProps();
        fp.get_int("PVTNUM");
        fp.get_int("SATNUM");
        fp.get_int("EQLNUM");
        fp.get_int("FIPNUM");

        for (const auto& keyword : fp.keys<int>())
            initFile.write(keyword, fp.get_int(keyword));

    }

    void writeGridGeometry(const ::Ewoms::EclipseGrid&         grid,
                           const ::Ewoms::UnitSystem&          units,
                           ::Ewoms::EclIO::OutputStream::Init& initFile)
    {
        const auto length = ::Ewoms::UnitSystem::measure::length;
        const auto nAct   = grid.getNumActive();

        auto dx    = std::vector<float>{};  dx   .reserve(nAct);
        auto dy    = std::vector<float>{};  dy   .reserve(nAct);
        auto dz    = std::vector<float>{};  dz   .reserve(nAct);
        auto depth = std::vector<float>{};  depth.reserve(nAct);

        for (auto cell = 0*nAct; cell < nAct; ++cell) {
            const auto  globCell = grid.getGlobalIndex(cell);
            const auto& dims     = grid.getCellDims(globCell);

            dx   .push_back(units.from_si(length, dims[0]));
            dy   .push_back(units.from_si(length, dims[1]));
            dz   .push_back(units.from_si(length, dims[2]));
            depth.push_back(units.from_si(length, grid.getCellDepth(globCell)));
        }

        initFile.write("DEPTH", depth);
        initFile.write("DX"   , dx);
        initFile.write("DY"   , dy);
        initFile.write("DZ"   , dz);
    }

    template <class WriteVector>
    void writeCellDoublePropertiesWithDefaultFlag(const Properties& propList,
                                                  const ::Ewoms::FieldPropsManager& fp,
                                                  WriteVector&&  write)
    {
        for (const auto& prop : propList) {
            if (! fp.has_double(prop.name))
                continue;

            auto data = fp.get_double(prop.name);
            auto defaulted = fp.defaulted<double>(prop.name);
            write(prop, std::move(defaulted), std::move(data));
        }
    }

    template <class WriteVector>
    void writeCellPropertiesValuesOnly(const Properties& propList,
                                       const ::Ewoms::FieldPropsManager& fp,
                                       WriteVector&&                   write)
    {
        for (const auto& prop : propList) {

            if (!fp.has_double(prop.name))
                continue;
            auto data = fp.get_double(prop.name);
            write(prop, std::move(data));
        }
    }

    void writeDoubleCellProperties(const Properties&                    propList,
                                   const ::Ewoms::FieldPropsManager&      fp,
                                   const ::Ewoms::UnitSystem&             units,
                                   const bool                           needDflt,
                                   ::Ewoms::EclIO::OutputStream::Init&    initFile)
    {
        if (needDflt) {
            writeCellDoublePropertiesWithDefaultFlag(propList, fp,
                [&units, &initFile](const CellProperty&   prop,
                                    std::vector<bool>&&   dflt,
                                    std::vector<double>&& value)
            {
                units.from_si(prop.unit, value);

                for (auto n = dflt.size(), i = 0*n; i < n; ++i) {
                    if (dflt[i]) {
                        // Element defaulted.  Output sentinel value
                        // (-1.0e+20) to signify defaulted element.
                        //
                        // Note: Start as float for roundtripping through
                        // function singlePrecision().
                        value[i] = static_cast<double>(-1.0e+20f);
                    }
                }

                initFile.write(prop.name, singlePrecision(value));
            });
        }
        else {
            writeCellPropertiesValuesOnly(propList, fp,
                [&units, &initFile](const CellProperty&   prop,
                                    std::vector<double>&& value)
            {
                units.from_si(prop.unit, value);
                initFile.write(prop.name, singlePrecision(value));
            });
        }
    }

    void writeDoubleCellProperties(const ::Ewoms::EclipseState&        es,
                                   const ::Ewoms::UnitSystem&          units,
                                   ::Ewoms::EclIO::OutputStream::Init& initFile)
    {
        const auto doubleKeywords = Properties {
            {"PORO"  , ::Ewoms::UnitSystem::measure::identity },
            {"PERMX" , ::Ewoms::UnitSystem::measure::permeability },
            {"PERMY" , ::Ewoms::UnitSystem::measure::permeability },
            {"PERMZ" , ::Ewoms::UnitSystem::measure::permeability },
            {"NTG"   , ::Ewoms::UnitSystem::measure::identity },
        };

        // The INIT file should always contain the NTG property, we
        // therefore invoke the auto create functionality to ensure
        // that "NTG" is included in the properties container.
        const auto& fp = es.globalFieldProps();
        fp.get_double("NTG");
        writeDoubleCellProperties(doubleKeywords, fp,
                                  units, false, initFile);
    }

    void writeSimulatorProperties(const ::Ewoms::EclipseGrid&         grid,
                                  const ::Ewoms::data::Solution&      simProps,
                                  ::Ewoms::EclIO::OutputStream::Init& initFile)
    {
        for (const auto& prop : simProps) {
            const auto& value = grid.compressedVector(prop.second.data);

            initFile.write(prop.first, singlePrecision(value));
        }
    }

    void writeTableData(const ::Ewoms::EclipseState&        es,
                        const ::Ewoms::UnitSystem&          units,
                        ::Ewoms::EclIO::OutputStream::Init& initFile)
    {
        ::Ewoms::Tables tables(units);

        tables.addPVTTables(es);
        tables.addDensity(es.getTableManager().getDensityTable());
        tables.addSatFunc(es);

        initFile.write("TABDIMS", tables.tabdims());
        initFile.write("TAB"    , tables.tab());
    }

    void writeIntegerMaps(std::map<std::string, std::vector<int>> mapData,
                          ::Ewoms::EclIO::OutputStream::Init&       initFile)
    {
        for (const auto& pair : mapData) {
            const auto& key = pair.first;

            if (key.size() > std::size_t{8}) {
                throw std::invalid_argument {
                    "Keyword '" + key + "'is too long."
                };
            }

            initFile.write(key, pair.second);
        }
    }

    void writeFilledSatFuncScaling(const Properties&                 propList,
                                   ::Ewoms::FieldPropsManager&&        fp,
                                   const ::Ewoms::UnitSystem&          units,
                                   ::Ewoms::EclIO::OutputStream::Init& initFile)
    {
        for (const auto& prop : propList)
            fp.get_double(prop.name);

        // Don't write sentinel value if input defaulted.
        writeDoubleCellProperties(propList, fp,
                                  units, false, initFile);
    }

    void writeSatFuncScaling(const ::Ewoms::EclipseState&        es,
                             const ::Ewoms::UnitSystem&          units,
                             ::Ewoms::EclIO::OutputStream::Init& initFile)
    {
        const auto epsVectors = ScalingVectors{}
            .withHysteresis(es.runspec().hysterPar().active())
            .collect       (es.runspec().phases());

        const auto& fp = es.globalFieldProps();
        if (! es.cfg().init().filleps()) {
            // No FILLEPS in input deck or number of active phases
            // unsupported by EFlow's saturation function finalizers.
            //
            // Output only those endpoint arrays that exist in the input
            // deck.  Write sentinel value if input defaulted.
            writeDoubleCellProperties(epsVectors.getVectors(), fp,
                                      units, true, initFile);
        }
        else {
            // Input deck specified FILLEPS so we should output all endpoint
            // arrays, whether explicitly defined in the input deck or not.
            // However, downstream clients of GridProperties<double> should
            // not see scaling arrays created for output purposes only, so
            // make a copy of the properties object and modify that copy in
            // order to leave the original intact.  Don't write sentinel
            // value if input defaulted.
            auto fp_copy = fp;
            writeFilledSatFuncScaling(epsVectors.getVectors(),
                                      std::move(fp_copy),
                                      units, initFile);
        }
    }

    void writeNonNeighbourConnections(const std::vector<::Ewoms::NNCdata>& nnc,
                                      const ::Ewoms::UnitSystem&           units,
                                      ::Ewoms::EclIO::OutputStream::Init&  initFile)
    {
        auto tran = std::vector<double>{};
        tran.reserve(nnc.size());

        for (const auto& nd : nnc) {
            tran.push_back(nd.trans);
        }

        units.from_si(::Ewoms::UnitSystem::measure::transmissibility, tran);

        initFile.write("TRANNNC", singlePrecision(tran));
    }
} // Anonymous namespace

void Ewoms::InitIO::write(const ::Ewoms::EclipseState&              es,
                        const ::Ewoms::EclipseGrid&               grid,
                        const ::Ewoms::Schedule&                  schedule,
                        const ::Ewoms::data::Solution&            simProps,
                        std::map<std::string, std::vector<int>> int_data,
                        const std::vector<::Ewoms::NNCdata>&      nnc,
                        ::Ewoms::EclIO::OutputStream::Init&       initFile)
{
    const auto& units = es.getUnits();

    writeInitFileHeader(es, grid, schedule, initFile);

    // The PORV vector is a special case.  This particular vector always
    // holds a total of nx*ny*nz elements, and the elements are explicitly
    // set to zero for inactive cells.  This treatment implies that the
    // active/inactive cell mapping can be inferred by reading the PORV
    // vector from the result set.
    writePoreVolume(es, units, initFile);
    writeGridGeometry(grid, units, initFile);
    writeDoubleCellProperties(es, units, initFile);
    writeSimulatorProperties(grid, simProps, initFile);
    writeTableData(es, units, initFile);
    writeIntegerCellProperties(es, initFile);
    writeIntegerMaps(std::move(int_data), initFile);
    writeSatFuncScaling(es, units, initFile);

    if (!nnc.empty()) {
        writeNonNeighbourConnections(nnc, units, initFile);
    }
}
