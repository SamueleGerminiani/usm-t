#include <algorithm>
#include <ctype.h>
#include <ostream>
#include <sstream>
#include <string>

#include "Float.hh"
#include "TemporalParserHandler.hh"
#include "Trace.hh"
#include "expUtils/ExpType.hh"

#include "formula/atom/Constant.hh"
#include "formula/expression/GenericExpression.hh"
#include "formula/temporal/temporal.hh"
#include "globals.hh"
#include "message.hh"
#include "misc.hh"
#include "propositionParsingUtils.hh"
#include "temporalParsingUtils.hh"
#include "tree/ErrorNode.h"
#include "tree/TerminalNode.h"

using namespace expression;

namespace hparser {

TemporalParserHandler::TemporalParserHandler(
    const harm::TracePtr &trace)
    : _trace(trace) {}

std::string
TemporalParserHandler::handleNewPP(const std::string &ph) {
  if (!_phToPP.count(ph)) {
    _phToPP[ph] = generatePtrPtr<Proposition>((Proposition *)nullptr);
  }

  return ph;
}

std::string
TemporalParserHandler::handleNewInst(const std::string &prop) {

  if (_propStrToInst.count(prop))
    return _propStrToInst.at(prop);

  PropositionPtr p = parsePropositionAlreadyTyped(prop, _trace);

  if (_useCache)
    p->enableCache();

  std::string inst = "_inst" + std::to_string(instCount++);
  _propStrToInst[prop] = inst;
  _instToPP[inst] = p;

  return inst;
}

void TemporalParserHandler::exitFormula(
    temporalParser::FormulaContext *ctx) {
  _temporalExpression = _tsubFormulas.top();
  _tsubFormulas.pop();
  _errorMessages.clear();
}

void TemporalParserHandler::exitSere_implication(
    temporalParser::Sere_implicationContext *ctx) {

  TemporalExpressionPtr right = _tsubFormulas.top();
  _tsubFormulas.pop();
  TemporalExpressionPtr left = _tsubFormulas.top();
  _tsubFormulas.pop();

  messageWarningIf(
      clc::outputLang == Language::SVA &&
          (!isSere(left) && !isBooleanLayer(left)),
      "SystemVerilog output language detected, SVA requires the "
      "antecedent of an "
      "implication to be a "
      "SERE, '" +
          temp2String(left, clc::outputLang,
                      PrintMode::ShowOnlyPermuationPlaceholders) +
          "' is not a SERE.\n"
          "The semantics of the generated assertions might change "
          "once they are printed, leading to hard-to-understand "
          "errors. Do not ignore this.\n" +
          printErrorMessage());

  _tsubFormulas.push(generatePtr<PropertyImplication>(
      left, right, true,
      (ctx->SEREIMPLO() != nullptr) ? true : false));

  return;
}

void TemporalParserHandler::exitTformula(
    temporalParser::TformulaContext *ctx) {

  if (ctx->tformula().size() == 2 &&
      (ctx->TAND() != nullptr || ctx->AND() != nullptr)) {
    TemporalExpressionPtr right = _tsubFormulas.top();
    _tsubFormulas.pop();
    TemporalExpressionPtr left = _tsubFormulas.top();
    _tsubFormulas.pop();
    _tsubFormulas.push(generatePtr<PropertyAnd>({left, right}));
    return;
  }

  if (ctx->tformula().size() == 2 &&
      (ctx->TOR() != nullptr || ctx->BOR() != nullptr ||
       ctx->OR() != nullptr)) {

    TemporalExpressionPtr right = _tsubFormulas.top();
    _tsubFormulas.pop();
    TemporalExpressionPtr left = _tsubFormulas.top();
    _tsubFormulas.pop();
    _tsubFormulas.push(generatePtr<PropertyOr>({left, right}));
    return;
  }

  if (ctx->tformula().size() == 2 &&
      (ctx->IMPL() != nullptr || ctx->IMPLO() != nullptr)) {
    TemporalExpressionPtr right = _tsubFormulas.top();
    _tsubFormulas.pop();
    TemporalExpressionPtr left = _tsubFormulas.top();
    _tsubFormulas.pop();
    messageWarningIf(
        clc::outputLang == Language::SVA &&
            (ctx->IMPL() != nullptr || ctx->IMPLO() != nullptr),
        "SystemVerilog output language detected, SystemVerilog "
        "implications requires to use |-> or |=>\n"
        "The semantics of the generated assertions might change once "
        "they are printed, leading to hard-to-understand errors. Do "
        "not ignore this.\n" +
            printErrorMessage());

    //property
    if (ctx->tformula().size() == 2 &&
        (ctx->IMPL() != nullptr || ctx->IMPLO() != nullptr)) {
      _tsubFormulas.push(generatePtr<PropertyImplication>(
          left, right, false,
          (ctx->IMPLO() != nullptr) ? true : false));

      return;
    }
  }

  if (ctx->tformula().size() == 1 &&
      (ctx->WEAK_NEXT() != nullptr ||
       ctx->STRONG_NEXT() != nullptr)) {
    bool isStrong =
        ctx->STRONG_NEXT() != nullptr || ctx->NOT() != nullptr;
    TemporalExpressionPtr exp = _tsubFormulas.top();
    _tsubFormulas.pop();
    size_t delay = ctx->UINTEGER() != nullptr
                       ? safeStoull(ctx->UINTEGER()->getText())
                       : 1;
    PropertyNextPtr pn = generatePtr<PropertyNext>(exp, delay);
    if (isStrong) {
      pn->setStrongVersion();
    }
    _tsubFormulas.push(pn);

    return;
  }

  if (ctx->tformula().size() == 2 &&
      (ctx->WEAK_UNTIL() != nullptr ||
       ctx->STRONG_UNTIL() != nullptr)) {
    TemporalExpressionPtr right = _tsubFormulas.top();
    _tsubFormulas.pop();
    TemporalExpressionPtr left = _tsubFormulas.top();
    _tsubFormulas.pop();
    PropertyUntilPtr pu = generatePtr<PropertyUntil>({left, right});
    if (ctx->STRONG_UNTIL() != nullptr) {
      pu->setStrongVersion();
    }
    _tsubFormulas.push(pu);
    return;
  }

  if (ctx->tformula().size() == 2 &&
      (ctx->WEAK_RELEASE() != nullptr ||
       ctx->STRONG_RELEASE() != nullptr)) {
    TemporalExpressionPtr right = _tsubFormulas.top();
    _tsubFormulas.pop();
    TemporalExpressionPtr left = _tsubFormulas.top();
    _tsubFormulas.pop();
    PropertyReleasePtr pr =
        generatePtr<PropertyRelease>({left, right});
    if (ctx->STRONG_RELEASE() != nullptr) {
      pr->setStrongVersion();
    }
    _tsubFormulas.push(pr);

    return;
  }

  if (ctx->tformula().size() == 1 &&
      (ctx->TNOT() != nullptr || ctx->NOT() != nullptr)) {
    TemporalExpressionPtr right = _tsubFormulas.top();
    _tsubFormulas.pop();
    _tsubFormulas.push(generatePtr<PropertyNot>(right));
    return;
  }

  if (ctx->tformula().size() == 1 &&
      (ctx->EVENTUALLY() != nullptr ||
       ctx->STRONG_EVENTUALLY() != nullptr)) {
    TemporalExpressionPtr right = _tsubFormulas.top();
    _tsubFormulas.pop();
    PropertyEventuallyPtr pe = generatePtr<PropertyEventually>(right);
    if (ctx->STRONG_EVENTUALLY() != nullptr) {
      pe->setStrongVersion();
    }
    _tsubFormulas.push(pe);
    return;
  }

  if (ctx->tformula().size() == 1 &&
      (ctx->ALWAYS() != nullptr || ctx->STRONG_ALWAYS() != nullptr)) {
    TemporalExpressionPtr right = _tsubFormulas.top();
    _tsubFormulas.pop();
    PropertyAlwaysPtr pa = generatePtr<PropertyAlways>(right);
    if (ctx->STRONG_ALWAYS() != nullptr) {
      pa->setStrongVersion();
    }
    _tsubFormulas.push(pa);
    return;
  }
}

void TemporalParserHandler::enterBooleanLayer(
    temporalParser::BooleanLayerContext *ctx) {

  if (ctx->PLACEHOLDER() != nullptr) {
    std::string ph = handleNewPP(ctx->PLACEHOLDER()->getText());
    _phToIdsDomain[ph];
    TemporalExpressionPtr bl_ph =
        generatePtr<BooleanLayerPermutationPlaceholder>(
            _phToPP.at(ph), ph);
    if (ctx->NOT() != nullptr) {
      bl_ph = generatePtr<BooleanLayerNot>(bl_ph);
    }
    _tsubFormulas.push(bl_ph);
    return;
  }

  if (ctx->boolean() != nullptr) {
    std::string inst = handleNewInst(ctx->boolean()->getText());
    _tsubFormulas.push(
        generatePtr<BooleanLayerInst>(_instToPP.at(inst), inst));
    return;
  }
}

void TemporalParserHandler::exitSere(
    temporalParser::SereContext *ctx) {

  if (ctx->FIRST_MATCH() != nullptr) {
    TemporalExpressionPtr right = _tsubFormulas.top();
    _tsubFormulas.pop();
    _tsubFormulas.push(generatePtr<SereFirstMatch>(right));
    return;
  }

  if (ctx->sere().size() == 2 &&
      (ctx->INTERSECT() != nullptr || ctx->AND() != nullptr)) {
    TemporalExpressionPtr right = _tsubFormulas.top();
    _tsubFormulas.pop();
    TemporalExpressionPtr left = _tsubFormulas.top();
    _tsubFormulas.pop();
    _tsubFormulas.push(generatePtr<SereIntersect>({left, right}));
    return;
  }

  if (ctx->sere().size() == 2 &&
      (ctx->BAND() != nullptr || ctx->TAND() != nullptr)) {
    TemporalExpressionPtr right = _tsubFormulas.top();
    _tsubFormulas.pop();
    TemporalExpressionPtr left = _tsubFormulas.top();
    _tsubFormulas.pop();
    _tsubFormulas.push(generatePtr<SereAnd>({left, right}));
    return;
  }

  if (ctx->sere().size() == 2 &&
      (ctx->TOR() != nullptr || ctx->BOR() != nullptr ||
       ctx->OR() != nullptr)) {
    TemporalExpressionPtr right = _tsubFormulas.top();
    _tsubFormulas.pop();
    TemporalExpressionPtr left = _tsubFormulas.top();
    _tsubFormulas.pop();
    _tsubFormulas.push(generatePtr<SereOr>({left, right}));
    return;
  }

  if (ctx->sere().size() == 1 && ctx->LSQUARED() != nullptr &&
      ctx->PLUS() != nullptr && ctx->RSQUARED() != nullptr) {
    TemporalExpressionPtr right = _tsubFormulas.top();
    _tsubFormulas.pop();
    _tsubFormulas.push(generatePtr<SerePlus>(right));
    return;
  }

  messageErrorIf(ctx->UINTEGER().empty() && ctx->DOLLAR() != nullptr,
                 "Invalid use of unbounded operator $\n" +
                     printErrorMessage());

  if (ctx->DELAY() != nullptr) {
    messageErrorIf(ctx->LSQUARED() != nullptr &&
                       ctx->RSQUARED() == nullptr,
                   "Missing right parenthesis in delay operator\n" +
                       printErrorMessage());
    messageErrorIf(ctx->LSQUARED() == nullptr &&
                       ctx->RSQUARED() != nullptr,
                   "Missing left parenthesis in delay operator\n" +
                       printErrorMessage());

    if (ctx->LSQUARED() != nullptr && ctx->RSQUARED() != nullptr &&
        (ctx->DOTS() != nullptr || ctx->COL() != nullptr) &&
        (ctx->UINTEGER().size() +
             (ctx->DOLLAR() != nullptr ? 1 : 0) ==
         2)) {
      //left ##[l:r] right or left ##[l:$] right or ##[l:r] right or ##[l:$] right

      std::string lw = ctx->UINTEGER()[0]->getText();
      std::string rw = ctx->DOLLAR() != nullptr
                           ? "$"
                           : ctx->UINTEGER()[1]->getText();

      TemporalExpressionPtr right = _tsubFormulas.top();
      _tsubFormulas.pop();
      int l = safeStoull(lw);
      int r = ctx->DOLLAR() != nullptr ? -1 : safeStoull(rw);
      messageErrorIf(r != -1 && l > r,
                     "Invalid use of delay operator: left bound must "
                     "be less than or equal to right bound\n" +
                         printErrorMessage());
      if (ctx->sere().size() == 2) {
        TemporalExpressionPtr left = _tsubFormulas.top();
        _tsubFormulas.pop();
        _tsubFormulas.push(generatePtr<SereDelay>(
            left, right, std::make_pair(l, r)));
      } else {
        _tsubFormulas.push(
            generatePtr<SereDelay>(right, std::make_pair(l, r)));
      }

    } else {
      //left ##[r] right or left ##r right or ##[r] right or ##r right
      messageErrorIf(ctx->DOLLAR() != nullptr,
                     "Invalid use of unbounded operator $\n" +
                         printErrorMessage());

      if (ctx->sere().size() == 2) {
        TemporalExpressionPtr right = _tsubFormulas.top();
        _tsubFormulas.pop();
        TemporalExpressionPtr left = _tsubFormulas.top();
        _tsubFormulas.pop();
        _tsubFormulas.push(generatePtr<SereDelay>(
            left, right,
            std::make_pair(
                safeStoull(ctx->UINTEGER()[0]->getText()),
                safeStoull(ctx->UINTEGER()[0]->getText()))));
      } else {
        TemporalExpressionPtr right = _tsubFormulas.top();
        _tsubFormulas.pop();
        _tsubFormulas.push(generatePtr<SereDelay>(
            right, std::make_pair(
                       safeStoull(ctx->UINTEGER()[0]->getText()),
                       safeStoull(ctx->UINTEGER()[0]->getText()))));
      }
    }
    return;
  }

  if (ctx->sere().empty() && ctx->booleanLayer() != nullptr &&
      ctx->LSQUARED() != nullptr && ctx->ASS() != nullptr &&
      ctx->RSQUARED() != nullptr) {

    if ((ctx->DOTS() != nullptr || ctx->COL() != nullptr) &&
        (ctx->UINTEGER().size() +
             (ctx->DOLLAR() != nullptr ? 1 : 0) ==
         2)) {
      //[=l:r] or [=l:$]

      int lw = safeStoull(ctx->UINTEGER()[0]->getText());
      int rw = ctx->DOLLAR() == nullptr
                   ? safeStoull(ctx->UINTEGER()[1]->getText())
                   : -1;
      messageErrorIf(rw != -1 && lw > rw,
                     "Invalid use of = operator: left bound must "
                     "be less than or equal to right bound\n" +
                         printErrorMessage());
      TemporalExpressionPtr right = _tsubFormulas.top();
      _tsubFormulas.pop();
      _tsubFormulas.push(generatePtr<SereNonConsecutiveRep>(
          right, std::make_pair(lw, rw)));

    } else {
      //[=r] or [=]
      messageErrorIf(ctx->DOLLAR() != nullptr,
                     "Invalid use of unbounded operator $\n" +
                         printErrorMessage());
      TemporalExpressionPtr right = _tsubFormulas.top();
      _tsubFormulas.pop();
      int rep = ctx->UINTEGER().size() == 1
                    ? safeStoull(ctx->UINTEGER()[0]->getText())
                    : 1;
      _tsubFormulas.push(generatePtr<SereNonConsecutiveRep>(
          right, std::make_pair(rep, rep)));
    }
    return;
  }

  if (ctx->sere().empty() && ctx->booleanLayer() != nullptr &&
      ctx->LSQUARED() != nullptr && ctx->IMPLO() != nullptr &&
      ctx->RSQUARED() != nullptr) {

    if ((ctx->DOTS() != nullptr || ctx->COL() != nullptr) &&
        (ctx->UINTEGER().size() +
             (ctx->DOLLAR() != nullptr ? 1 : 0) ==
         2)) {
      //[->l:r] or [->l:$]

      TemporalExpressionPtr right = _tsubFormulas.top();
      _tsubFormulas.pop();
      int lwi = safeStoull(ctx->UINTEGER()[0]->getText());
      int rwi = ctx->DOLLAR() != nullptr
                    ? -1
                    : safeStoull(ctx->UINTEGER()[1]->getText());
      messageErrorIf(rwi != -1 && lwi > rwi,
                     "Invalid use of -> operator: left bound must "
                     "be less than or equal to right bound\n" +
                         printErrorMessage());
      _tsubFormulas.push(
          generatePtr<SereGoto>(right, std::make_pair(lwi, rwi)));

    } else {
      //[->r] or [->]
      TemporalExpressionPtr right = _tsubFormulas.top();
      _tsubFormulas.pop();
      int rep = ctx->UINTEGER().size() == 1
                    ? safeStoull(ctx->UINTEGER()[0]->getText())
                    : 1;
      _tsubFormulas.push(
          generatePtr<SereGoto>(right, std::make_pair(rep, rep)));
    }
    return;
  }

  if (ctx->LSQUARED() != nullptr && ctx->TIMES() != nullptr &&
      ctx->RSQUARED() != nullptr) {

    if ((ctx->DOTS() != nullptr || ctx->COL() != nullptr) &&
        (ctx->UINTEGER().size() +
             (ctx->DOLLAR() != nullptr ? 1 : 0) ==
         2)) {
      //right [*l:r] or right [*l:$]

      TemporalExpressionPtr right = _tsubFormulas.top();
      _tsubFormulas.pop();

      int lwi = safeStoull(ctx->UINTEGER()[0]->getText());
      int rwi = ctx->DOLLAR() != nullptr
                    ? -1
                    : safeStoull(ctx->UINTEGER()[1]->getText());
      messageErrorIf(rwi != -1 && lwi > rwi,
                     "Invalid use of * operator: left bound must "
                     "be less than or equal to right bound\n" +
                         printErrorMessage());
      _tsubFormulas.push(generatePtr<SereConsecutiveRep>(
          right, std::make_pair(lwi, rwi)));
    } else {
      //right [*r] or right [*]
      messageErrorIf(ctx->DOLLAR() != nullptr,
                     "Invalid use of unbounded operator $\n" +
                         printErrorMessage());

      TemporalExpressionPtr right = _tsubFormulas.top();
      _tsubFormulas.pop();

      int lw;
      int rw;

      if (ctx->UINTEGER().empty()) {
        lw = 0;
        rw = -1;
      } else {
        lw = safeStoull(ctx->UINTEGER()[0]->getText());
        rw = lw;
      }

      _tsubFormulas.push(generatePtr<SereConsecutiveRep>(
          right, std::make_pair(lw, rw)));
    }
    return;
  }

  if (ctx->sere().size() == 2 && ctx->COL() != nullptr) {
    TemporalExpressionPtr right = _tsubFormulas.top();
    _tsubFormulas.pop();
    TemporalExpressionPtr left = _tsubFormulas.top();
    _tsubFormulas.pop();
    _tsubFormulas.push(generatePtr<SereConcat>(left, right, true));
    return;
  }

  if (ctx->sere().size() == 2 && ctx->SCOL() != nullptr) {
    TemporalExpressionPtr right = _tsubFormulas.top();
    _tsubFormulas.pop();
    TemporalExpressionPtr left = _tsubFormulas.top();
    _tsubFormulas.pop();
    _tsubFormulas.push(generatePtr<SereConcat>(left, right, false));
    return;
  }
}

std::string TemporalParserHandler::printErrorMessage() {
  std::stringstream ss;
  for (auto &msg : _errorMessages) {
    ss << msg << "\n";
  }
  return ss.str();
}

void TemporalParserHandler::visitErrorNode(
    __attribute__((unused)) antlr4::tree::ErrorNode *node) {
  messageError("Antlr parse error: " + node->getText() + "\n" +
               printErrorMessage());
}

void TemporalParserHandler::addErrorMessage(const std::string &msg) {
  _errorMessages.push_back(msg);
}

std::unordered_map<std::string, std::unordered_set<int>>
TemporalParserHandler::getPhToIdsDomain() {
  return _phToIdsDomain;
}
expression::TemporalExpressionPtr
TemporalParserHandler::getTemporalExpression() {
  messageErrorIf(
      !_tsubFormulas.empty(),
      "Error parsing formula, subformulas leftovers on "
      "the stack, the first one is '" +
          temp2String(_tsubFormulas.top(), Language::SpotLTL,
                      PrintMode::ShowOnlyPermuationPlaceholders) +
          "' " + printErrorMessage());
  return _temporalExpression;
}

} // namespace hparser
