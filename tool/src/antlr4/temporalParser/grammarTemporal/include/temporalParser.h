
#include <string>
// Forward declaration
bool isUnary(const std::string& token);
bool isSharedOperator(const std::string& token);
bool canUseSharedOperator(const std::string& unaryOp, const std::string& sharedOp);
bool canTakeThisNot(const std::string& unaryOp, const std::string& ph);


// Generated from temporal.g4 by ANTLR 4.12.0

#pragma once


#include "antlr4-runtime.h"




class  temporalParser : public antlr4::Parser {
public:
  enum {
    PLACEHOLDER = 1, EVENTUALLY = 2, ALWAYS = 3, NEXT = 4, UNTIL = 5, RELEASE = 6, 
    DOTS = 7, IMPL = 8, IMPLO = 9, IFF = 10, SEREIMPL = 11, SEREIMPLO = 12, 
    ASS = 13, DELAY = 14, SCOL = 15, FIRST_MATCH = 16, TNOT = 17, TAND = 18, 
    INTERSECT = 19, TOR = 20, BOOLEAN_CONSTANT = 21, BOOLEAN_VARIABLE = 22, 
    INT_VARIABLE = 23, CONST_SUFFIX = 24, FLOAT_CONSTANT = 25, FLOAT_VARIABLE = 26, 
    LCURLY = 27, RCURLY = 28, LSQUARED = 29, RSQUARED = 30, LROUND = 31, 
    RROUND = 32, INSIDE = 33, FUNCTION = 34, SINTEGER = 35, UINTEGER = 36, 
    FLOAT = 37, GCC_BINARY = 38, HEX = 39, VERILOG_BINARY = 40, FVL = 41, 
    SINGLE_QUOTE = 42, PLUS = 43, MINUS = 44, TIMES = 45, DIV = 46, GT = 47, 
    GE = 48, LT = 49, LE = 50, EQ = 51, NEQ = 52, BAND = 53, BOR = 54, BXOR = 55, 
    NEG = 56, LSHIFT = 57, RSHIFT = 58, AND = 59, OR = 60, NOT = 61, COL = 62, 
    DCOL = 63, DOLLAR = 64, RANGE = 65, CLS_TYPE = 66, WS = 67
  };

  enum {
    RuleFormula = 0, RuleSere = 1, RuleBooleanLayer = 2, RuleTformula = 3, 
    RuleSere_implication = 4, RuleStartBoolean = 5, RuleStartInt = 6, RuleStartFloat = 7, 
    RuleBoolean = 8, RuleBooleanAtom = 9, RuleNumeric = 10, RuleRange = 11, 
    RuleIntAtom = 12, RuleInt_constant = 13, RuleFloatAtom = 14, RuleRelop = 15, 
    RuleCls_op = 16
  };

  explicit temporalParser(antlr4::TokenStream *input);

  temporalParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~temporalParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  // Definition
  bool isUnary(const std::string& token){
  return token=="X" || token=="nexttime" || token=="F" || token=="G" || token=="always" || token=="eventually" || token=="!" || token=="not";
  }
  bool isSharedOperator(const std::string& token){
  return token=="and" || token=="&&" || token=="or" || token=="||" || token=="|";
  }
  bool canUseSharedOperator(const std::string& unaryOp, const std::string& sharedOp){
  return !(isSharedOperator(sharedOp) && isUnary(unaryOp));
  }

  bool canTakeThisNot(const std::string& unaryOp, const std::string& ph){
  return !(unaryOp=="!" && ph[0]=='P');
  }



  class FormulaContext;
  class SereContext;
  class BooleanLayerContext;
  class TformulaContext;
  class Sere_implicationContext;
  class StartBooleanContext;
  class StartIntContext;
  class StartFloatContext;
  class BooleanContext;
  class BooleanAtomContext;
  class NumericContext;
  class RangeContext;
  class IntAtomContext;
  class Int_constantContext;
  class FloatAtomContext;
  class RelopContext;
  class Cls_opContext; 

  class  FormulaContext : public antlr4::ParserRuleContext {
  public:
    FormulaContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TformulaContext *tformula();
    antlr4::tree::TerminalNode *EOF();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FormulaContext* formula();

  class  SereContext : public antlr4::ParserRuleContext {
  public:
    SereContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FIRST_MATCH();
    antlr4::tree::TerminalNode *LROUND();
    std::vector<SereContext *> sere();
    SereContext* sere(size_t i);
    antlr4::tree::TerminalNode *RROUND();
    antlr4::tree::TerminalNode *LCURLY();
    antlr4::tree::TerminalNode *RCURLY();
    BooleanLayerContext *booleanLayer();
    antlr4::tree::TerminalNode *LSQUARED();
    antlr4::tree::TerminalNode *ASS();
    antlr4::tree::TerminalNode *RSQUARED();
    std::vector<antlr4::tree::TerminalNode *> UINTEGER();
    antlr4::tree::TerminalNode* UINTEGER(size_t i);
    antlr4::tree::TerminalNode *DOTS();
    antlr4::tree::TerminalNode *COL();
    antlr4::tree::TerminalNode *DOLLAR();
    antlr4::tree::TerminalNode *IMPLO();
    antlr4::tree::TerminalNode *DELAY();
    antlr4::tree::TerminalNode *BAND();
    antlr4::tree::TerminalNode *TAND();
    antlr4::tree::TerminalNode *INTERSECT();
    antlr4::tree::TerminalNode *AND();
    antlr4::tree::TerminalNode *TOR();
    antlr4::tree::TerminalNode *OR();
    antlr4::tree::TerminalNode *BOR();
    antlr4::tree::TerminalNode *SCOL();
    antlr4::tree::TerminalNode *TIMES();
    antlr4::tree::TerminalNode *PLUS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  SereContext* sere();
  SereContext* sere(int precedence);
  class  BooleanLayerContext : public antlr4::ParserRuleContext {
  public:
    BooleanLayerContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LROUND();
    BooleanLayerContext *booleanLayer();
    antlr4::tree::TerminalNode *RROUND();
    BooleanContext *boolean();
    antlr4::tree::TerminalNode *PLACEHOLDER();
    antlr4::tree::TerminalNode *NOT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  BooleanLayerContext* booleanLayer();

  class  TformulaContext : public antlr4::ParserRuleContext {
  public:
    TformulaContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BooleanLayerContext *booleanLayer();
    antlr4::tree::TerminalNode *LROUND();
    std::vector<TformulaContext *> tformula();
    TformulaContext* tformula(size_t i);
    antlr4::tree::TerminalNode *RROUND();
    antlr4::tree::TerminalNode *TNOT();
    antlr4::tree::TerminalNode *NOT();
    antlr4::tree::TerminalNode *NEXT();
    antlr4::tree::TerminalNode *LSQUARED();
    antlr4::tree::TerminalNode *UINTEGER();
    antlr4::tree::TerminalNode *RSQUARED();
    antlr4::tree::TerminalNode *ALWAYS();
    antlr4::tree::TerminalNode *EVENTUALLY();
    Sere_implicationContext *sere_implication();
    SereContext *sere();
    antlr4::tree::TerminalNode *LCURLY();
    antlr4::tree::TerminalNode *RCURLY();
    antlr4::tree::TerminalNode *UNTIL();
    antlr4::tree::TerminalNode *RELEASE();
    antlr4::tree::TerminalNode *TAND();
    antlr4::tree::TerminalNode *AND();
    antlr4::tree::TerminalNode *TOR();
    antlr4::tree::TerminalNode *OR();
    antlr4::tree::TerminalNode *BOR();
    antlr4::tree::TerminalNode *IMPLO();
    antlr4::tree::TerminalNode *IMPL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  TformulaContext* tformula();
  TformulaContext* tformula(int precedence);
  class  Sere_implicationContext : public antlr4::ParserRuleContext {
  public:
    Sere_implicationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    SereContext *sere();
    antlr4::tree::TerminalNode *SEREIMPL();
    TformulaContext *tformula();
    antlr4::tree::TerminalNode *LCURLY();
    antlr4::tree::TerminalNode *RCURLY();
    antlr4::tree::TerminalNode *SEREIMPLO();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Sere_implicationContext* sere_implication();

  class  StartBooleanContext : public antlr4::ParserRuleContext {
  public:
    StartBooleanContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BooleanContext *boolean();
    antlr4::tree::TerminalNode *EOF();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  StartBooleanContext* startBoolean();

  class  StartIntContext : public antlr4::ParserRuleContext {
  public:
    StartIntContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NumericContext *numeric();
    antlr4::tree::TerminalNode *EOF();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  StartIntContext* startInt();

  class  StartFloatContext : public antlr4::ParserRuleContext {
  public:
    StartFloatContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NumericContext *numeric();
    antlr4::tree::TerminalNode *EOF();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  StartFloatContext* startFloat();

  class  BooleanContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *booleanop = nullptr;
    BooleanContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NOT();
    std::vector<BooleanContext *> boolean();
    BooleanContext* boolean(size_t i);
    std::vector<NumericContext *> numeric();
    NumericContext* numeric(size_t i);
    RelopContext *relop();
    antlr4::tree::TerminalNode *EQ();
    antlr4::tree::TerminalNode *NEQ();
    BooleanAtomContext *booleanAtom();
    antlr4::tree::TerminalNode *LROUND();
    antlr4::tree::TerminalNode *RROUND();
    antlr4::tree::TerminalNode *AND();
    antlr4::tree::TerminalNode *OR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  BooleanContext* boolean();
  BooleanContext* boolean(int precedence);
  class  BooleanAtomContext : public antlr4::ParserRuleContext {
  public:
    BooleanAtomContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BOOLEAN_CONSTANT();
    antlr4::tree::TerminalNode *BOOLEAN_VARIABLE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  BooleanAtomContext* booleanAtom();

  class  NumericContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *artop = nullptr;
    antlr4::Token *logop = nullptr;
    NumericContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NEG();
    std::vector<NumericContext *> numeric();
    NumericContext* numeric(size_t i);
    IntAtomContext *intAtom();
    FloatAtomContext *floatAtom();
    antlr4::tree::TerminalNode *LROUND();
    antlr4::tree::TerminalNode *RROUND();
    antlr4::tree::TerminalNode *TIMES();
    antlr4::tree::TerminalNode *DIV();
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *MINUS();
    antlr4::tree::TerminalNode *LSHIFT();
    antlr4::tree::TerminalNode *RSHIFT();
    antlr4::tree::TerminalNode *BAND();
    antlr4::tree::TerminalNode *BXOR();
    antlr4::tree::TerminalNode *BOR();
    RangeContext *range();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  NumericContext* numeric();
  NumericContext* numeric(int precedence);
  class  RangeContext : public antlr4::ParserRuleContext {
  public:
    RangeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LSQUARED();
    antlr4::tree::TerminalNode *RSQUARED();
    std::vector<antlr4::tree::TerminalNode *> SINTEGER();
    antlr4::tree::TerminalNode* SINTEGER(size_t i);
    std::vector<antlr4::tree::TerminalNode *> UINTEGER();
    antlr4::tree::TerminalNode* UINTEGER(size_t i);
    antlr4::tree::TerminalNode *COL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  RangeContext* range();

  class  IntAtomContext : public antlr4::ParserRuleContext {
  public:
    IntAtomContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Int_constantContext *int_constant();
    antlr4::tree::TerminalNode *INT_VARIABLE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  IntAtomContext* intAtom();

  class  Int_constantContext : public antlr4::ParserRuleContext {
  public:
    Int_constantContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *GCC_BINARY();
    antlr4::tree::TerminalNode *SINTEGER();
    antlr4::tree::TerminalNode *CONST_SUFFIX();
    antlr4::tree::TerminalNode *UINTEGER();
    antlr4::tree::TerminalNode *VERILOG_BINARY();
    antlr4::tree::TerminalNode *HEX();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Int_constantContext* int_constant();

  class  FloatAtomContext : public antlr4::ParserRuleContext {
  public:
    FloatAtomContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FLOAT_CONSTANT();
    antlr4::tree::TerminalNode *FLOAT_VARIABLE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FloatAtomContext* floatAtom();

  class  RelopContext : public antlr4::ParserRuleContext {
  public:
    RelopContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *GT();
    antlr4::tree::TerminalNode *GE();
    antlr4::tree::TerminalNode *LT();
    antlr4::tree::TerminalNode *LE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  RelopContext* relop();

  class  Cls_opContext : public antlr4::ParserRuleContext {
  public:
    Cls_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RANGE();
    antlr4::tree::TerminalNode *GT();
    antlr4::tree::TerminalNode *GE();
    antlr4::tree::TerminalNode *LT();
    antlr4::tree::TerminalNode *LE();
    antlr4::tree::TerminalNode *EQ();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Cls_opContext* cls_op();


  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool sereSempred(SereContext *_localctx, size_t predicateIndex);
  bool tformulaSempred(TformulaContext *_localctx, size_t predicateIndex);
  bool booleanSempred(BooleanContext *_localctx, size_t predicateIndex);
  bool numericSempred(NumericContext *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

