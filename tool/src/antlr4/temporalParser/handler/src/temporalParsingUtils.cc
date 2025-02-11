#include "temporalParsingUtils.hh"
#include "ANTLRInputStream.h"
#include "CommonTokenStream.h"
#include "Float.hh"
#include "TemplateImplication.hh"
#include "TemporalParserHandler.hh"
#include "Trace.hh"
#include "VarDeclaration.hh"

#include "formula/temporal/Property.hh"
#include "formula/temporal/Sere.hh"
#include "formula/temporal/TemporalExpression.hh"
#include "message.hh"
#include "propositionParsingUtils.hh"
#include "temporalLexer.h"
#include "temporalParser.h"
#include "tree/ParseTreeWalker.h"
#include <memory>

namespace antlr4 {
namespace tree {
class ParseTree;
} // namespace tree
} // namespace antlr4

namespace hparser {
using namespace expression;

static std::shared_ptr<hparser::TemporalParserHandler>
parse(std::string formula, const harm::TracePtr &trace,
      bool useCache) {

  auto decls = trace->getDeclarations();
  addTypeToExp(formula, decls);

  // parse typed propositions
  std::shared_ptr<hparser::TemporalParserHandler> listener =
      std::make_shared<hparser::TemporalParserHandler>(trace);

  listener->addErrorMessage("\t\t\tIn formula: " + formula);
  listener->_useCache = useCache;
  antlr4::ANTLRInputStream input(formula);
  temporalLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  temporalParser parser(&tokens);
  //print tokens
  /*
    std::map<size_t, std::string> indexToToken;
    for (auto [token,index] : parser.getTokenTypeMap()) {
        indexToToken[index] = token;
    }
    for (auto &i : lexer.getAllTokens()) {
        std::cout << i->toString() <<" "<<indexToToken.at(i->getType())<<"\n";
    }
    */
  antlr4::tree::ParseTree *treeFragAnt = parser.formula();
  /*
  DEBUG
  std::cout << treeFragAnt->toStringTree(&parser) << "\n\n\n";
    exit(0);
  */
  antlr4::tree::ParseTreeWalker::DEFAULT.walk(&*listener,
                                              treeFragAnt);

  return listener;
}

TemporalExpressionPtr
parseTemporalExpression(std::string formula,
                        const harm::TracePtr &trace) {
  static std::map<std::pair<std::string, harm::TracePtr>,
                  TemporalExpressionPtr>
      cache;

  if (cache.count({formula, trace})) {
    return cache.at({formula, trace});
  }

  auto parsedFormula =
      parse(formula, trace, false)->getTemporalExpression();
  cache[std::make_pair(formula, trace)] = parsedFormula;

  return parsedFormula;
}

} // namespace hparser
