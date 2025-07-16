#include <memory>
#include <vector>

namespace cmp {

class SymbolTable;

class ClassSymbol;
using ClassPtr = std::shared_ptr<ClassSymbol>;
using ClassVec = std::vector<ClassPtr>;

class VariableSymbol;
using VarPtr = std::shared_ptr<VariableSymbol>;
using VarVec = std::vector<VarPtr>;

class FunctionSymbol;
using FuncPtr = std::shared_ptr<FunctionSymbol>;
using FuncVec = std::vector<FuncPtr>;

class Scope;
using ScopePtr = std::shared_ptr<Scope>;
using ScopeVec = std::vector<ScopePtr>;

class Type;
using TypePtr = std::shared_ptr<Type>;
using TypeVec = std::vector<TypePtr>;

class Statement;
using StmtPtr = std::shared_ptr<Statement>;
using StmtVec = std::vector<StmtPtr>;

class Expression;
using ExprPtr = std::shared_ptr<Expression>;
using ExprVec = std::vector<ExprPtr>;

}
