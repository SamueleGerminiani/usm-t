
// Generated from temporal.g4 by ANTLR 4.12.0

#pragma once


#include "antlr4-runtime.h"
#include "temporalListener.h"


/**
 * This class provides an empty implementation of temporalListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  temporalBaseListener : public temporalListener {
public:

  virtual void enterFormula(temporalParser::FormulaContext * /*ctx*/) override { }
  virtual void exitFormula(temporalParser::FormulaContext * /*ctx*/) override { }

  virtual void enterSere(temporalParser::SereContext * /*ctx*/) override { }
  virtual void exitSere(temporalParser::SereContext * /*ctx*/) override { }

  virtual void enterBooleanLayer(temporalParser::BooleanLayerContext * /*ctx*/) override { }
  virtual void exitBooleanLayer(temporalParser::BooleanLayerContext * /*ctx*/) override { }

  virtual void enterTformula(temporalParser::TformulaContext * /*ctx*/) override { }
  virtual void exitTformula(temporalParser::TformulaContext * /*ctx*/) override { }

  virtual void enterSere_implication(temporalParser::Sere_implicationContext * /*ctx*/) override { }
  virtual void exitSere_implication(temporalParser::Sere_implicationContext * /*ctx*/) override { }

  virtual void enterStartBoolean(temporalParser::StartBooleanContext * /*ctx*/) override { }
  virtual void exitStartBoolean(temporalParser::StartBooleanContext * /*ctx*/) override { }

  virtual void enterStartInt(temporalParser::StartIntContext * /*ctx*/) override { }
  virtual void exitStartInt(temporalParser::StartIntContext * /*ctx*/) override { }

  virtual void enterStartFloat(temporalParser::StartFloatContext * /*ctx*/) override { }
  virtual void exitStartFloat(temporalParser::StartFloatContext * /*ctx*/) override { }

  virtual void enterBoolean(temporalParser::BooleanContext * /*ctx*/) override { }
  virtual void exitBoolean(temporalParser::BooleanContext * /*ctx*/) override { }

  virtual void enterBooleanAtom(temporalParser::BooleanAtomContext * /*ctx*/) override { }
  virtual void exitBooleanAtom(temporalParser::BooleanAtomContext * /*ctx*/) override { }

  virtual void enterNumeric(temporalParser::NumericContext * /*ctx*/) override { }
  virtual void exitNumeric(temporalParser::NumericContext * /*ctx*/) override { }

  virtual void enterRange(temporalParser::RangeContext * /*ctx*/) override { }
  virtual void exitRange(temporalParser::RangeContext * /*ctx*/) override { }

  virtual void enterIntAtom(temporalParser::IntAtomContext * /*ctx*/) override { }
  virtual void exitIntAtom(temporalParser::IntAtomContext * /*ctx*/) override { }

  virtual void enterInt_constant(temporalParser::Int_constantContext * /*ctx*/) override { }
  virtual void exitInt_constant(temporalParser::Int_constantContext * /*ctx*/) override { }

  virtual void enterFloatAtom(temporalParser::FloatAtomContext * /*ctx*/) override { }
  virtual void exitFloatAtom(temporalParser::FloatAtomContext * /*ctx*/) override { }

  virtual void enterRelop(temporalParser::RelopContext * /*ctx*/) override { }
  virtual void exitRelop(temporalParser::RelopContext * /*ctx*/) override { }

  virtual void enterCls_op(temporalParser::Cls_opContext * /*ctx*/) override { }
  virtual void exitCls_op(temporalParser::Cls_opContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

