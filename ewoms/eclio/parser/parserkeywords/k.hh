#ifndef PARSER_KEYWORDS_K_HH
#define PARSER_KEYWORDS_K_HH
#include <ewoms/eclio/parser/parserkeyword.hh>
namespace Ewoms {
namespace ParserKeywords {

   class KRNUM : public ParserKeyword {
   public:
       KRNUM();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



   class KRNUMMF : public ParserKeyword {
   public:
       KRNUMMF();
       static const std::string keywordName;

       class data {
       public:
           static const std::string itemName;
       };
   };



}
}
#endif
