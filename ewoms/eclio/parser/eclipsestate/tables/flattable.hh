#ifndef EWOMS_FLAT_TABLE_H
#define EWOMS_FLAT_TABLE_H

namespace Ewoms {

class DeckKeyword;

template< typename T >
struct FlatTable : public std::vector< T > {
    FlatTable() = default;
    explicit FlatTable( const DeckKeyword& );
    explicit FlatTable(const std::vector<T>& data) :
        std::vector<T>(data)
    {}
};

struct DENSITYRecord {
    static constexpr std::size_t size = 3;

    double oil;
    double water;
    double gas;

    bool operator==(const DENSITYRecord& data) const {
        return oil == data.oil &&
               water == data.water &&
               gas == data.gas;
    }
};

struct DensityTable : public FlatTable< DENSITYRecord > {
    using FlatTable< DENSITYRecord >::FlatTable;
};

struct PVTWRecord {
    static constexpr std::size_t size = 5;

    double reference_pressure;
    double volume_factor;
    double compressibility;
    double viscosity;
    double viscosibility;

    bool operator==(const PVTWRecord& data) const {
        return reference_pressure == data.reference_pressure &&
               volume_factor == data.volume_factor &&
               compressibility == data.compressibility &&
               viscosity == data.viscosity &&
               viscosibility == data.viscosibility;
    }
};

struct PvtwTable : public FlatTable< PVTWRecord > {
    using FlatTable< PVTWRecord >::FlatTable;
};

struct ROCKRecord {
    static constexpr std::size_t size = 2;

    double reference_pressure;
    double compressibility;

    bool operator==(const ROCKRecord& data) const {
        return reference_pressure == data.reference_pressure &&
               compressibility == data.compressibility;
    }

};

struct RockTable : public FlatTable< ROCKRecord > {
    using FlatTable< ROCKRecord >::FlatTable;
};

struct PVCDORecord {
    static constexpr std::size_t size = 5;

    double reference_pressure;
    double volume_factor;
    double compressibility;
    double viscosity;
    double viscosibility;

    bool operator==(const PVCDORecord& data) const {
        return reference_pressure == data.reference_pressure &&
               volume_factor == data.volume_factor &&
               compressibility == data.compressibility &&
               viscosity == data.viscosity &&
               viscosibility == data.viscosibility;
    }
};

struct PvcdoTable : public FlatTable< PVCDORecord > {
    using FlatTable< PVCDORecord >::FlatTable;
};

struct PlmixparRecord {
    static constexpr std::size_t size = 1;

    double todd_langstaff;

    bool operator==(const PlmixparRecord& data) const {
        return todd_langstaff == data.todd_langstaff;
    }
};

struct PlmixparTable : public FlatTable< PlmixparRecord> {
    using FlatTable< PlmixparRecord >::FlatTable;
};

struct PlyvmhRecord {
    static constexpr std::size_t size = 4;

    double k_mh;
    double a_mh;
    double gamma;
    double kappa;

    bool operator==(const PlyvmhRecord& data) const {
        return k_mh == data.k_mh &&
               a_mh == data.a_mh &&
               gamma == data.gamma &&
               kappa == data.kappa;
    }
};

struct PlyvmhTable : public FlatTable<PlyvmhRecord> {
    using FlatTable< PlyvmhRecord >::FlatTable;
};

struct ShrateRecord {
    static constexpr std::size_t size = 1;

    double rate;

    bool operator==(const ShrateRecord& data) const {
        return rate == data.rate;
    }
};

struct ShrateTable : public FlatTable<ShrateRecord> {
    using FlatTable< ShrateRecord >::FlatTable;
};

struct Stone1exRecord {
    static constexpr std::size_t size = 1;

    double eta;

    bool operator==(const Stone1exRecord& data) const {
        return eta == data.eta;
    }
};

struct Stone1exTable : public FlatTable<Stone1exRecord> {
    using FlatTable< Stone1exRecord >::FlatTable;
};

struct TlmixparRecord {
    static constexpr std::size_t size = 2;

    double viscosity;
    double density;

    bool operator==(const TlmixparRecord& data) const {
        return viscosity == data.viscosity &&
               density == data.density;
    }
};

struct TlmixparTable : public FlatTable< TlmixparRecord> {
    using FlatTable< TlmixparRecord >::FlatTable;
};

struct VISCREFRecord {
    static constexpr std::size_t size = 2;

    double reference_pressure;
    double reference_rs;

    bool operator==(const VISCREFRecord& data) const {
        return reference_pressure == data.reference_pressure &&
              reference_rs == data.reference_rs;
    }
};

struct ViscrefTable : public FlatTable< VISCREFRecord > {
    using FlatTable< VISCREFRecord >::FlatTable;
};

struct WATDENTRecord {
    static constexpr std::size_t size = 3;

    double reference_temperature;
    double first_coefficient;
    double second_coefficient;

    bool operator==(const WATDENTRecord& data) const {
        return reference_temperature == data.reference_temperature &&
               first_coefficient == data.first_coefficient &&
               second_coefficient == data.second_coefficient;
    }
};

struct WatdentTable : public FlatTable< WATDENTRecord > {
    using FlatTable< WATDENTRecord >::FlatTable;
};

}

#endif //EWOMS_FLAT_TABLE_H
