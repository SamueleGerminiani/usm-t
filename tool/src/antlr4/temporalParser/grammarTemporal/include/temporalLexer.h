
// Generated from temporal.g4 by ANTLR 4.12.0

#pragma once


#include "antlr4-runtime.h"




class  temporalLexer : public antlr4::Lexer {
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

