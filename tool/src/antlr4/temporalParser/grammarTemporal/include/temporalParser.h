
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
    PLACEHOLDER = 1, EVENTUALLY = 2, STRONG_EVENTUALLY = 3, ALWAYS = 4, 
    STRONG_ALWAYS = 5, WEAK_NEXT = 6, STRONG_NEXT = 7, WEAK_UNTIL = 8, STRONG_UNTIL = 9, 
    WEAK_RELEASE = 10, STRONG_RELEASE = 11, DOTS = 12, IMPL = 13, IMPLO = 14, 
    IFF = 15, SEREIMPL = 16, SEREIMPLO = 17, ASS = 18, DELAY = 19, SCOL = 20, 
    FIRST_MATCH = 21, TNOT = 22, TAND = 23, INTERSECT = 24, TOR = 25, BOOLEAN_CONSTANT = 26, 
    BOOLEAN_VARIABLE = 27, INT_VARIABLE = 28, CONST_SUFFIX = 29, FLOAT_CONSTANT = 30, 
    FLOAT_VARIABLE = 31, LCURLY = 32, RCURLY = 33, LSQUARED = 34, RSQUARED = 35, 
    LROUND = 36, RROUND = 37, INSIDE = 38, FUNCTION = 39, SINTEGER = 40, 
    UINTEGER = 41, FLOAT = 42, GCC_BINARY = 43, HEX = 44, VERILOG_BINARY = 45, 
    FVL = 46, SINGLE_QUOTE = 47, PLUS = 48, MINUS = 49, TIMES = 50, DIV = 51, 
    GT = 52, GE = 53, LT = 54, LE = 55, EQ = 56, NEQ = 57, BAND = 58, BOR = 59, 
    BXOR = 60, NEG = 61, LSHIFT = 62, RSHIFT = 63, AND = 64, OR = 65, NOT = 66, 
    COL = 67, DCOL = 68, DOLLAR = 69, RANGE = 70, CLS_TYPE = 71, WS = 72
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
  return token=="X" || token=="nexttime" || token=="F" || token=="G" || token=="always" || token=="s_always" || token=="eventually" || token=="s_eventually" || token=="!" || token=="not";
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
    antlr4::tree::TerminalNode *WEAK_NEXT();
    antlr4::tree::TerminalNode *LSQUARED();
    antlr4::tree::TerminalNode *UINTEGER();
    antlr4::tree::TerminalNode *RSQUARED();
    antlr4::tree::TerminalNode *STRONG_NEXT();
    antlr4::tree::TerminalNode *ALWAYS();
    antlr4::tree::TerminalNode *STRONG_ALWAYS();
    antlr4::tree::TerminalNode *EVENTUALLY();
    antlr4::tree::TerminalNode *STRONG_EVENTUALLY();
    Sere_implicationContext *sere_implication();
    SereContext *sere();
    antlr4::tree::TerminalNode *LCURLY();
    antlr4::tree::TerminalNode *RCURLY();
    antlr4::tree::TerminalNode *WEAK_UNTIL();
    antlr4::tree::TerminalNode *STRONG_UNTIL();
    antlr4::tree::TerminalNode *WEAK_RELEASE();
    antlr4::tree::TerminalNode *STRONG_RELEASE();
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

