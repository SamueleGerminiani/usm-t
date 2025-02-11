
// Generated from temporal.g4 by ANTLR 4.12.0

#pragma once


#include "antlr4-runtime.h"




class  temporalLexer : public antlr4::Lexer {
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

  explicit temporalLexer(antlr4::CharStream *input);

  ~temporalLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

