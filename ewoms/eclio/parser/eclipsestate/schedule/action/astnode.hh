#ifndef ASTNODE_H
#define ASTNODE_H

#include <ewoms/eclio/parser/eclipsestate/schedule/action/actioncontext.hh>

#include "actionvalue.hh"

namespace Ewoms {
namespace Action {

class ActionContext;
class WellSet;
class ASTNode {
public:

    ASTNode();
    ASTNode(TokenType type_arg);
    ASTNode(double value);
    ASTNode(TokenType type_arg, const std::string& func_arg, const std::vector<std::string>& arg_list_arg);

    Action::Result eval(const Action::Context& context) const;
    Action::Value value(const Action::Context& context) const;
    TokenType type;
    void add_child(const ASTNode& child);
    size_t size() const;
    std::string func;

private:
    std::vector<std::string> arg_list;
    double number = 0.0;

    /*
      To have a member std::vector<ASTNode> inside the ASTNode class is
      supposedly borderline undefined behaviour; it compiles without warnings
      and works. Good for enough for me.
    */
    std::vector<ASTNode> children;
};
}
}
#endif
