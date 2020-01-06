#include <ewoms/eclio/parser/deck/udavalue.hh>
#include <ewoms/eclio/parser/parseritem.hh>
#include <ewoms/eclio/parser/parserrecord.hh>
#include <ewoms/eclio/parser/parser.hh>





#include <ewoms/eclio/parser/parserkeywords/k.hh>
namespace Ewoms {
namespace ParserKeywords {
KRNUM::KRNUM( ) : ParserKeyword("KRNUM")
{
  setFixedSize( (size_t) 1);
  addValidSectionName("REGIONS");
  clearDeckNames();
  addDeckName("KRNUM");
  {
     ParserRecord record;
     {
        ParserItem item("data", ParserItem::itype::INT);
        item.setSizeType(ParserItem::item_size::ALL);
        record.addDataItem(item);
     }
     addDataRecord( record );
  }
}
const std::string KRNUM::keywordName = "KRNUM";
const std::string KRNUM::data::itemName = "data";


KRNUMMF::KRNUMMF( ) : ParserKeyword("KRNUMMF")
{
  setFixedSize( (size_t) 1);
  addValidSectionName("REGIONS");
  clearDeckNames();
  addDeckName("KRNUMMF");
  {
     ParserRecord record;
     {
        ParserItem item("data", ParserItem::itype::INT);
        item.setSizeType(ParserItem::item_size::ALL);
        record.addDataItem(item);
     }
     addDataRecord( record );
  }
}
const std::string KRNUMMF::keywordName = "KRNUMMF";
const std::string KRNUMMF::data::itemName = "data";


}
}
