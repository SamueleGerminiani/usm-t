
#include <string>
// Forward declaration
bool isUnary(const std::string& token);
bool isSharedOperator(const std::string& token);
bool canUseSharedOperator(const std::string& unaryOp, const std::string& sharedOp);
bool canTakeThisNot(const std::string& unaryOp, const std::string& ph);


// Generated from temporal.g4 by ANTLR 4.12.0


#include "temporalListener.h"

#include "temporalParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct TemporalParserStaticData final {
  TemporalParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  TemporalParserStaticData(const TemporalParserStaticData&) = delete;
  TemporalParserStaticData(TemporalParserStaticData&&) = delete;
  TemporalParserStaticData& operator=(const TemporalParserStaticData&) = delete;
  TemporalParserStaticData& operator=(TemporalParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag temporalParserOnceFlag;
TemporalParserStaticData *temporalParserStaticData = nullptr;

void temporalParserInitialize() {
  assert(temporalParserStaticData == nullptr);
  auto staticData = std::make_unique<TemporalParserStaticData>(
    std::vector<std::string>{
      "formula", "sere", "booleanLayer", "tformula", "sere_implication", 
      "startBoolean", "startInt", "startFloat", "boolean", "booleanAtom", 
      "numeric", "range", "intAtom", "int_constant", "floatAtom", "relop", 
      "cls_op"
    },
    std::vector<std::string>{
      "", "", "", "'s_eventually'", "", "'s_always'", "", "'s_nexttime'", 
      "", "", "'R'", "'M'", "'..'", "'=>'", "'->'", "'<->'", "", "", "'='", 
      "'##'", "';'", "'first_match'", "'not'", "'and'", "'intersect'", "'or'", 
      "", "", "", "", "", "", "'{'", "'}'", "'['", "']'", "'('", "')'", 
      "'inside'", "", "", "", "", "", "", "", "", "'''", "'+'", "'-'", "'*'", 
      "'/'", "'>'", "'>='", "'<'", "'<='", "'=='", "'!='", "'&'", "'|'", 
      "'^'", "'~'", "'<<'", "'>>'", "'&&'", "'||'", "'!'", "':'", "'::'", 
      "'$'", "'><'"
    },
    std::vector<std::string>{
      "", "PLACEHOLDER", "EVENTUALLY", "STRONG_EVENTUALLY", "ALWAYS", "STRONG_ALWAYS", 
      "WEAK_NEXT", "STRONG_NEXT", "WEAK_UNTIL", "STRONG_UNTIL", "WEAK_RELEASE", 
      "STRONG_RELEASE", "DOTS", "IMPL", "IMPLO", "IFF", "SEREIMPL", "SEREIMPLO", 
      "ASS", "DELAY", "SCOL", "FIRST_MATCH", "TNOT", "TAND", "INTERSECT", 
      "TOR", "BOOLEAN_CONSTANT", "BOOLEAN_VARIABLE", "INT_VARIABLE", "CONST_SUFFIX", 
      "FLOAT_CONSTANT", "FLOAT_VARIABLE", "LCURLY", "RCURLY", "LSQUARED", 
      "RSQUARED", "LROUND", "RROUND", "INSIDE", "FUNCTION", "SINTEGER", 
      "UINTEGER", "FLOAT", "GCC_BINARY", "HEX", "VERILOG_BINARY", "FVL", 
      "SINGLE_QUOTE", "PLUS", "MINUS", "TIMES", "DIV", "GT", "GE", "LT", 
      "LE", "EQ", "NEQ", "BAND", "BOR", "BXOR", "NEG", "LSHIFT", "RSHIFT", 
      "AND", "OR", "NOT", "COL", "DCOL", "DOLLAR", "RANGE", "CLS_TYPE", 
      "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,72,379,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,56,8,1,1,1,3,1,59,8,1,1,1,3,
  	1,62,8,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,70,8,1,1,1,3,1,73,8,1,1,1,3,1,76,
  	8,1,1,1,1,1,1,1,1,1,3,1,82,8,1,1,1,3,1,85,8,1,1,1,3,1,88,8,1,1,1,3,1,
  	91,8,1,1,1,3,1,94,8,1,1,1,1,1,3,1,98,8,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,1,1,1,1,1,1,1,1,1,3,1,112,8,1,1,1,3,1,115,8,1,1,1,3,1,118,8,1,1,1,3,
  	1,121,8,1,1,1,3,1,124,8,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	3,1,137,8,1,1,1,3,1,140,8,1,1,1,3,1,143,8,1,1,1,1,1,1,1,1,1,1,1,5,1,150,
  	8,1,10,1,12,1,153,9,1,1,2,1,2,1,2,1,2,1,2,1,2,3,2,161,8,2,1,2,3,2,164,
  	8,2,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,3,177,8,3,1,3,3,3,180,
  	8,3,1,3,3,3,183,8,3,1,3,3,3,186,8,3,1,3,1,3,1,3,3,3,191,8,3,1,3,3,3,194,
  	8,3,1,3,3,3,197,8,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,3,207,8,3,1,3,1,
  	3,3,3,211,8,3,3,3,213,8,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,
  	1,3,5,3,227,8,3,10,3,12,3,230,9,3,1,4,1,4,3,4,234,8,4,1,4,1,4,3,4,238,
  	8,4,1,4,1,4,1,4,1,4,3,4,244,8,4,1,4,1,4,3,4,248,8,4,1,4,1,4,1,4,3,4,253,
  	8,4,1,5,1,5,1,5,1,6,1,6,1,6,1,7,1,7,1,7,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,
  	8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,3,8,285,8,8,1,8,
  	1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,5,8,299,8,8,10,8,12,8,302,
  	9,8,1,9,1,9,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,3,10,315,8,10,
  	1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,
  	1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,5,10,340,8,10,10,10,12,10,
  	343,9,10,1,11,1,11,1,11,1,11,3,11,349,8,11,1,11,1,11,1,12,1,12,3,12,355,
  	8,12,1,13,1,13,1,13,3,13,360,8,13,1,13,1,13,3,13,364,8,13,1,13,3,13,367,
  	8,13,1,13,1,13,3,13,371,8,13,1,14,1,14,1,15,1,15,1,16,1,16,1,16,0,4,2,
  	6,16,20,17,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,0,17,2,0,12,
  	12,67,67,2,0,41,41,69,69,2,0,23,24,64,64,3,0,25,25,59,59,65,65,2,0,22,
  	22,66,66,1,0,4,5,1,0,2,3,1,0,8,11,2,0,23,23,64,64,1,0,13,14,1,0,26,27,
  	1,0,50,51,1,0,48,49,1,0,40,41,1,0,30,31,1,0,52,55,2,0,52,56,70,70,454,
  	0,34,1,0,0,0,2,97,1,0,0,0,4,163,1,0,0,0,6,212,1,0,0,0,8,252,1,0,0,0,10,
  	254,1,0,0,0,12,257,1,0,0,0,14,260,1,0,0,0,16,284,1,0,0,0,18,303,1,0,0,
  	0,20,314,1,0,0,0,22,344,1,0,0,0,24,354,1,0,0,0,26,370,1,0,0,0,28,372,
  	1,0,0,0,30,374,1,0,0,0,32,376,1,0,0,0,34,35,3,6,3,0,35,36,5,0,0,1,36,
  	1,1,0,0,0,37,38,6,1,-1,0,38,39,5,21,0,0,39,40,5,36,0,0,40,41,3,2,1,0,
  	41,42,5,37,0,0,42,98,1,0,0,0,43,44,5,36,0,0,44,45,3,2,1,0,45,46,5,37,
  	0,0,46,98,1,0,0,0,47,48,5,32,0,0,48,49,3,2,1,0,49,50,5,33,0,0,50,98,1,
  	0,0,0,51,52,3,4,2,0,52,53,5,34,0,0,53,55,5,18,0,0,54,56,5,41,0,0,55,54,
  	1,0,0,0,55,56,1,0,0,0,56,58,1,0,0,0,57,59,7,0,0,0,58,57,1,0,0,0,58,59,
  	1,0,0,0,59,61,1,0,0,0,60,62,7,1,0,0,61,60,1,0,0,0,61,62,1,0,0,0,62,63,
  	1,0,0,0,63,64,5,35,0,0,64,98,1,0,0,0,65,66,3,4,2,0,66,67,5,34,0,0,67,
  	69,5,14,0,0,68,70,5,41,0,0,69,68,1,0,0,0,69,70,1,0,0,0,70,72,1,0,0,0,
  	71,73,7,0,0,0,72,71,1,0,0,0,72,73,1,0,0,0,73,75,1,0,0,0,74,76,7,1,0,0,
  	75,74,1,0,0,0,75,76,1,0,0,0,76,77,1,0,0,0,77,78,5,35,0,0,78,98,1,0,0,
  	0,79,81,5,19,0,0,80,82,5,34,0,0,81,80,1,0,0,0,81,82,1,0,0,0,82,84,1,0,
  	0,0,83,85,5,41,0,0,84,83,1,0,0,0,84,85,1,0,0,0,85,87,1,0,0,0,86,88,7,
  	0,0,0,87,86,1,0,0,0,87,88,1,0,0,0,88,90,1,0,0,0,89,91,7,1,0,0,90,89,1,
  	0,0,0,90,91,1,0,0,0,91,93,1,0,0,0,92,94,5,35,0,0,93,92,1,0,0,0,93,94,
  	1,0,0,0,94,95,1,0,0,0,95,98,3,2,1,4,96,98,3,4,2,0,97,37,1,0,0,0,97,43,
  	1,0,0,0,97,47,1,0,0,0,97,51,1,0,0,0,97,65,1,0,0,0,97,79,1,0,0,0,97,96,
  	1,0,0,0,98,151,1,0,0,0,99,100,10,8,0,0,100,101,5,58,0,0,101,150,3,2,1,
  	9,102,103,10,7,0,0,103,104,7,2,0,0,104,150,3,2,1,8,105,106,10,6,0,0,106,
  	107,7,3,0,0,107,150,3,2,1,7,108,109,10,5,0,0,109,111,5,19,0,0,110,112,
  	5,34,0,0,111,110,1,0,0,0,111,112,1,0,0,0,112,114,1,0,0,0,113,115,5,41,
  	0,0,114,113,1,0,0,0,114,115,1,0,0,0,115,117,1,0,0,0,116,118,7,0,0,0,117,
  	116,1,0,0,0,117,118,1,0,0,0,118,120,1,0,0,0,119,121,7,1,0,0,120,119,1,
  	0,0,0,120,121,1,0,0,0,121,123,1,0,0,0,122,124,5,35,0,0,123,122,1,0,0,
  	0,123,124,1,0,0,0,124,125,1,0,0,0,125,150,3,2,1,6,126,127,10,3,0,0,127,
  	128,5,67,0,0,128,150,3,2,1,4,129,130,10,2,0,0,130,131,5,20,0,0,131,150,
  	3,2,1,3,132,133,10,12,0,0,133,134,5,34,0,0,134,136,5,50,0,0,135,137,5,
  	41,0,0,136,135,1,0,0,0,136,137,1,0,0,0,137,139,1,0,0,0,138,140,7,0,0,
  	0,139,138,1,0,0,0,139,140,1,0,0,0,140,142,1,0,0,0,141,143,7,1,0,0,142,
  	141,1,0,0,0,142,143,1,0,0,0,143,144,1,0,0,0,144,150,5,35,0,0,145,146,
  	10,11,0,0,146,147,5,34,0,0,147,148,5,48,0,0,148,150,5,35,0,0,149,99,1,
  	0,0,0,149,102,1,0,0,0,149,105,1,0,0,0,149,108,1,0,0,0,149,126,1,0,0,0,
  	149,129,1,0,0,0,149,132,1,0,0,0,149,145,1,0,0,0,150,153,1,0,0,0,151,149,
  	1,0,0,0,151,152,1,0,0,0,152,3,1,0,0,0,153,151,1,0,0,0,154,155,5,36,0,
  	0,155,156,3,4,2,0,156,157,5,37,0,0,157,164,1,0,0,0,158,164,3,16,8,0,159,
  	161,5,66,0,0,160,159,1,0,0,0,160,161,1,0,0,0,161,162,1,0,0,0,162,164,
  	5,1,0,0,163,154,1,0,0,0,163,158,1,0,0,0,163,160,1,0,0,0,164,5,1,0,0,0,
  	165,166,6,3,-1,0,166,213,3,4,2,0,167,168,5,36,0,0,168,169,3,6,3,0,169,
  	170,5,37,0,0,170,213,1,0,0,0,171,172,4,3,8,0,172,173,7,4,0,0,173,213,
  	3,6,3,11,174,176,5,6,0,0,175,177,5,34,0,0,176,175,1,0,0,0,176,177,1,0,
  	0,0,177,179,1,0,0,0,178,180,5,41,0,0,179,178,1,0,0,0,179,180,1,0,0,0,
  	180,182,1,0,0,0,181,183,5,66,0,0,182,181,1,0,0,0,182,183,1,0,0,0,183,
  	185,1,0,0,0,184,186,5,35,0,0,185,184,1,0,0,0,185,186,1,0,0,0,186,187,
  	1,0,0,0,187,213,3,6,3,10,188,190,5,7,0,0,189,191,5,34,0,0,190,189,1,0,
  	0,0,190,191,1,0,0,0,191,193,1,0,0,0,192,194,5,41,0,0,193,192,1,0,0,0,
  	193,194,1,0,0,0,194,196,1,0,0,0,195,197,5,35,0,0,196,195,1,0,0,0,196,
  	197,1,0,0,0,197,198,1,0,0,0,198,213,3,6,3,9,199,200,7,5,0,0,200,213,3,
  	6,3,8,201,202,7,6,0,0,202,213,3,6,3,7,203,213,3,8,4,0,204,206,4,3,9,0,
  	205,207,5,32,0,0,206,205,1,0,0,0,206,207,1,0,0,0,207,208,1,0,0,0,208,
  	210,3,2,1,0,209,211,5,33,0,0,210,209,1,0,0,0,210,211,1,0,0,0,211,213,
  	1,0,0,0,212,165,1,0,0,0,212,167,1,0,0,0,212,171,1,0,0,0,212,174,1,0,0,
  	0,212,188,1,0,0,0,212,199,1,0,0,0,212,201,1,0,0,0,212,203,1,0,0,0,212,
  	204,1,0,0,0,213,228,1,0,0,0,214,215,10,6,0,0,215,216,7,7,0,0,216,227,
  	3,6,3,6,217,218,10,5,0,0,218,219,7,8,0,0,219,227,3,6,3,6,220,221,10,4,
  	0,0,221,222,7,3,0,0,222,227,3,6,3,5,223,224,10,3,0,0,224,225,7,9,0,0,
  	225,227,3,6,3,3,226,214,1,0,0,0,226,217,1,0,0,0,226,220,1,0,0,0,226,223,
  	1,0,0,0,227,230,1,0,0,0,228,226,1,0,0,0,228,229,1,0,0,0,229,7,1,0,0,0,
  	230,228,1,0,0,0,231,253,1,0,0,0,232,234,5,32,0,0,233,232,1,0,0,0,233,
  	234,1,0,0,0,234,235,1,0,0,0,235,237,3,2,1,0,236,238,5,33,0,0,237,236,
  	1,0,0,0,237,238,1,0,0,0,238,239,1,0,0,0,239,240,5,16,0,0,240,241,3,6,
  	3,0,241,253,1,0,0,0,242,244,5,32,0,0,243,242,1,0,0,0,243,244,1,0,0,0,
  	244,245,1,0,0,0,245,247,3,2,1,0,246,248,5,33,0,0,247,246,1,0,0,0,247,
  	248,1,0,0,0,248,249,1,0,0,0,249,250,5,17,0,0,250,251,3,6,3,0,251,253,
  	1,0,0,0,252,231,1,0,0,0,252,233,1,0,0,0,252,243,1,0,0,0,253,9,1,0,0,0,
  	254,255,3,16,8,0,255,256,5,0,0,1,256,11,1,0,0,0,257,258,3,20,10,0,258,
  	259,5,0,0,1,259,13,1,0,0,0,260,261,3,20,10,0,261,262,5,0,0,1,262,15,1,
  	0,0,0,263,264,6,8,-1,0,264,265,5,66,0,0,265,285,3,16,8,11,266,267,3,20,
  	10,0,267,268,3,30,15,0,268,269,3,20,10,0,269,285,1,0,0,0,270,271,3,20,
  	10,0,271,272,5,56,0,0,272,273,3,20,10,0,273,285,1,0,0,0,274,275,3,20,
  	10,0,275,276,5,57,0,0,276,277,3,20,10,0,277,285,1,0,0,0,278,285,3,18,
  	9,0,279,285,3,20,10,0,280,281,5,36,0,0,281,282,3,16,8,0,282,283,5,37,
  	0,0,283,285,1,0,0,0,284,263,1,0,0,0,284,266,1,0,0,0,284,270,1,0,0,0,284,
  	274,1,0,0,0,284,278,1,0,0,0,284,279,1,0,0,0,284,280,1,0,0,0,285,300,1,
  	0,0,0,286,287,10,7,0,0,287,288,5,56,0,0,288,299,3,16,8,8,289,290,10,6,
  	0,0,290,291,5,57,0,0,291,299,3,16,8,7,292,293,10,5,0,0,293,294,5,64,0,
  	0,294,299,3,16,8,6,295,296,10,4,0,0,296,297,5,65,0,0,297,299,3,16,8,5,
  	298,286,1,0,0,0,298,289,1,0,0,0,298,292,1,0,0,0,298,295,1,0,0,0,299,302,
  	1,0,0,0,300,298,1,0,0,0,300,301,1,0,0,0,301,17,1,0,0,0,302,300,1,0,0,
  	0,303,304,7,10,0,0,304,19,1,0,0,0,305,306,6,10,-1,0,306,307,5,61,0,0,
  	307,315,3,20,10,12,308,315,3,24,12,0,309,315,3,28,14,0,310,311,5,36,0,
  	0,311,312,3,20,10,0,312,313,5,37,0,0,313,315,1,0,0,0,314,305,1,0,0,0,
  	314,308,1,0,0,0,314,309,1,0,0,0,314,310,1,0,0,0,315,341,1,0,0,0,316,317,
  	10,10,0,0,317,318,7,11,0,0,318,340,3,20,10,11,319,320,10,9,0,0,320,321,
  	7,12,0,0,321,340,3,20,10,10,322,323,10,8,0,0,323,324,5,62,0,0,324,340,
  	3,20,10,9,325,326,10,7,0,0,326,327,5,63,0,0,327,340,3,20,10,8,328,329,
  	10,6,0,0,329,330,5,58,0,0,330,340,3,20,10,7,331,332,10,5,0,0,332,333,
  	5,60,0,0,333,340,3,20,10,6,334,335,10,4,0,0,335,336,5,59,0,0,336,340,
  	3,20,10,5,337,338,10,11,0,0,338,340,3,22,11,0,339,316,1,0,0,0,339,319,
  	1,0,0,0,339,322,1,0,0,0,339,325,1,0,0,0,339,328,1,0,0,0,339,331,1,0,0,
  	0,339,334,1,0,0,0,339,337,1,0,0,0,340,343,1,0,0,0,341,339,1,0,0,0,341,
  	342,1,0,0,0,342,21,1,0,0,0,343,341,1,0,0,0,344,345,5,34,0,0,345,348,7,
  	13,0,0,346,347,5,67,0,0,347,349,7,13,0,0,348,346,1,0,0,0,348,349,1,0,
  	0,0,349,350,1,0,0,0,350,351,5,35,0,0,351,23,1,0,0,0,352,355,3,26,13,0,
  	353,355,5,28,0,0,354,352,1,0,0,0,354,353,1,0,0,0,355,25,1,0,0,0,356,371,
  	5,43,0,0,357,359,5,40,0,0,358,360,5,29,0,0,359,358,1,0,0,0,359,360,1,
  	0,0,0,360,371,1,0,0,0,361,363,5,41,0,0,362,364,5,29,0,0,363,362,1,0,0,
  	0,363,364,1,0,0,0,364,371,1,0,0,0,365,367,5,41,0,0,366,365,1,0,0,0,366,
  	367,1,0,0,0,367,368,1,0,0,0,368,371,5,45,0,0,369,371,5,44,0,0,370,356,
  	1,0,0,0,370,357,1,0,0,0,370,361,1,0,0,0,370,366,1,0,0,0,370,369,1,0,0,
  	0,371,27,1,0,0,0,372,373,7,14,0,0,373,29,1,0,0,0,374,375,7,15,0,0,375,
  	31,1,0,0,0,376,377,7,16,0,0,377,33,1,0,0,0,53,55,58,61,69,72,75,81,84,
  	87,90,93,97,111,114,117,120,123,136,139,142,149,151,160,163,176,179,182,
  	185,190,193,196,206,210,212,226,228,233,237,243,247,252,284,298,300,314,
  	339,341,348,354,359,363,366,370
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  temporalParserStaticData = staticData.release();
}

}

temporalParser::temporalParser(TokenStream *input) : temporalParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

temporalParser::temporalParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  temporalParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *temporalParserStaticData->atn, temporalParserStaticData->decisionToDFA, temporalParserStaticData->sharedContextCache, options);
}

temporalParser::~temporalParser() {
  delete _interpreter;
}

const atn::ATN& temporalParser::getATN() const {
  return *temporalParserStaticData->atn;
}

std::string temporalParser::getGrammarFileName() const {
  return "temporal.g4";
}

const std::vector<std::string>& temporalParser::getRuleNames() const {
  return temporalParserStaticData->ruleNames;
}

const dfa::Vocabulary& temporalParser::getVocabulary() const {
  return temporalParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView temporalParser::getSerializedATN() const {
  return temporalParserStaticData->serializedATN;
}


//----------------- FormulaContext ------------------------------------------------------------------

temporalParser::FormulaContext::FormulaContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

temporalParser::TformulaContext* temporalParser::FormulaContext::tformula() {
  return getRuleContext<temporalParser::TformulaContext>(0);
}

tree::TerminalNode* temporalParser::FormulaContext::EOF() {
  return getToken(temporalParser::EOF, 0);
}


size_t temporalParser::FormulaContext::getRuleIndex() const {
  return temporalParser::RuleFormula;
}

void temporalParser::FormulaContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFormula(this);
}

void temporalParser::FormulaContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFormula(this);
}

temporalParser::FormulaContext* temporalParser::formula() {
  FormulaContext *_localctx = _tracker.createInstance<FormulaContext>(_ctx, getState());
  enterRule(_localctx, 0, temporalParser::RuleFormula);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(34);
    tformula(0);
    setState(35);
    match(temporalParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SereContext ------------------------------------------------------------------

temporalParser::SereContext::SereContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* temporalParser::SereContext::FIRST_MATCH() {
  return getToken(temporalParser::FIRST_MATCH, 0);
}

tree::TerminalNode* temporalParser::SereContext::LROUND() {
  return getToken(temporalParser::LROUND, 0);
}

std::vector<temporalParser::SereContext *> temporalParser::SereContext::sere() {
  return getRuleContexts<temporalParser::SereContext>();
}

temporalParser::SereContext* temporalParser::SereContext::sere(size_t i) {
  return getRuleContext<temporalParser::SereContext>(i);
}

tree::TerminalNode* temporalParser::SereContext::RROUND() {
  return getToken(temporalParser::RROUND, 0);
}

tree::TerminalNode* temporalParser::SereContext::LCURLY() {
  return getToken(temporalParser::LCURLY, 0);
}

tree::TerminalNode* temporalParser::SereContext::RCURLY() {
  return getToken(temporalParser::RCURLY, 0);
}

temporalParser::BooleanLayerContext* temporalParser::SereContext::booleanLayer() {
  return getRuleContext<temporalParser::BooleanLayerContext>(0);
}

tree::TerminalNode* temporalParser::SereContext::LSQUARED() {
  return getToken(temporalParser::LSQUARED, 0);
}

tree::TerminalNode* temporalParser::SereContext::ASS() {
  return getToken(temporalParser::ASS, 0);
}

tree::TerminalNode* temporalParser::SereContext::RSQUARED() {
  return getToken(temporalParser::RSQUARED, 0);
}

std::vector<tree::TerminalNode *> temporalParser::SereContext::UINTEGER() {
  return getTokens(temporalParser::UINTEGER);
}

tree::TerminalNode* temporalParser::SereContext::UINTEGER(size_t i) {
  return getToken(temporalParser::UINTEGER, i);
}

tree::TerminalNode* temporalParser::SereContext::DOTS() {
  return getToken(temporalParser::DOTS, 0);
}

tree::TerminalNode* temporalParser::SereContext::COL() {
  return getToken(temporalParser::COL, 0);
}

tree::TerminalNode* temporalParser::SereContext::DOLLAR() {
  return getToken(temporalParser::DOLLAR, 0);
}

tree::TerminalNode* temporalParser::SereContext::IMPLO() {
  return getToken(temporalParser::IMPLO, 0);
}

tree::TerminalNode* temporalParser::SereContext::DELAY() {
  return getToken(temporalParser::DELAY, 0);
}

tree::TerminalNode* temporalParser::SereContext::BAND() {
  return getToken(temporalParser::BAND, 0);
}

tree::TerminalNode* temporalParser::SereContext::TAND() {
  return getToken(temporalParser::TAND, 0);
}

tree::TerminalNode* temporalParser::SereContext::INTERSECT() {
  return getToken(temporalParser::INTERSECT, 0);
}

tree::TerminalNode* temporalParser::SereContext::AND() {
  return getToken(temporalParser::AND, 0);
}

tree::TerminalNode* temporalParser::SereContext::TOR() {
  return getToken(temporalParser::TOR, 0);
}

tree::TerminalNode* temporalParser::SereContext::OR() {
  return getToken(temporalParser::OR, 0);
}

tree::TerminalNode* temporalParser::SereContext::BOR() {
  return getToken(temporalParser::BOR, 0);
}

tree::TerminalNode* temporalParser::SereContext::SCOL() {
  return getToken(temporalParser::SCOL, 0);
}

tree::TerminalNode* temporalParser::SereContext::TIMES() {
  return getToken(temporalParser::TIMES, 0);
}

tree::TerminalNode* temporalParser::SereContext::PLUS() {
  return getToken(temporalParser::PLUS, 0);
}


size_t temporalParser::SereContext::getRuleIndex() const {
  return temporalParser::RuleSere;
}

void temporalParser::SereContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSere(this);
}

void temporalParser::SereContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSere(this);
}


temporalParser::SereContext* temporalParser::sere() {
   return sere(0);
}

temporalParser::SereContext* temporalParser::sere(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  temporalParser::SereContext *_localctx = _tracker.createInstance<SereContext>(_ctx, parentState);
  temporalParser::SereContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 2;
  enterRecursionRule(_localctx, 2, temporalParser::RuleSere, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(97);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 11, _ctx)) {
    case 1: {
      setState(38);
      match(temporalParser::FIRST_MATCH);
      setState(39);
      match(temporalParser::LROUND);
      setState(40);
      sere(0);
      setState(41);
      match(temporalParser::RROUND);
      break;
    }

    case 2: {
      setState(43);
      match(temporalParser::LROUND);
      setState(44);
      sere(0);
      setState(45);
      match(temporalParser::RROUND);
      break;
    }

    case 3: {
      setState(47);
      match(temporalParser::LCURLY);
      setState(48);
      sere(0);
      setState(49);
      match(temporalParser::RCURLY);
      break;
    }

    case 4: {
      setState(51);
      booleanLayer();
      setState(52);
      match(temporalParser::LSQUARED);
      setState(53);
      match(temporalParser::ASS);
      setState(55);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx)) {
      case 1: {
        setState(54);
        match(temporalParser::UINTEGER);
        break;
      }

      default:
        break;
      }
      setState(58);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == temporalParser::DOTS

      || _la == temporalParser::COL) {
        setState(57);
        _la = _input->LA(1);
        if (!(_la == temporalParser::DOTS

        || _la == temporalParser::COL)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(61);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == temporalParser::UINTEGER

      || _la == temporalParser::DOLLAR) {
        setState(60);
        _la = _input->LA(1);
        if (!(_la == temporalParser::UINTEGER

        || _la == temporalParser::DOLLAR)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(63);
      match(temporalParser::RSQUARED);
      break;
    }

    case 5: {
      setState(65);
      booleanLayer();
      setState(66);
      match(temporalParser::LSQUARED);
      setState(67);
      match(temporalParser::IMPLO);
      setState(69);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx)) {
      case 1: {
        setState(68);
        match(temporalParser::UINTEGER);
        break;
      }

      default:
        break;
      }
      setState(72);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == temporalParser::DOTS

      || _la == temporalParser::COL) {
        setState(71);
        _la = _input->LA(1);
        if (!(_la == temporalParser::DOTS

        || _la == temporalParser::COL)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(75);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == temporalParser::UINTEGER

      || _la == temporalParser::DOLLAR) {
        setState(74);
        _la = _input->LA(1);
        if (!(_la == temporalParser::UINTEGER

        || _la == temporalParser::DOLLAR)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(77);
      match(temporalParser::RSQUARED);
      break;
    }

    case 6: {
      setState(79);
      match(temporalParser::DELAY);
      setState(81);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == temporalParser::LSQUARED) {
        setState(80);
        match(temporalParser::LSQUARED);
      }
      setState(84);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
      case 1: {
        setState(83);
        match(temporalParser::UINTEGER);
        break;
      }

      default:
        break;
      }
      setState(87);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == temporalParser::DOTS

      || _la == temporalParser::COL) {
        setState(86);
        _la = _input->LA(1);
        if (!(_la == temporalParser::DOTS

        || _la == temporalParser::COL)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(90);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
      case 1: {
        setState(89);
        _la = _input->LA(1);
        if (!(_la == temporalParser::UINTEGER

        || _la == temporalParser::DOLLAR)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        break;
      }

      default:
        break;
      }
      setState(93);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == temporalParser::RSQUARED) {
        setState(92);
        match(temporalParser::RSQUARED);
      }
      setState(95);
      sere(4);
      break;
    }

    case 7: {
      setState(96);
      booleanLayer();
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(151);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(149);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 20, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<SereContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleSere);
          setState(99);

          if (!(precpred(_ctx, 8))) throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(100);
          match(temporalParser::BAND);
          setState(101);
          sere(9);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<SereContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleSere);
          setState(102);

          if (!(precpred(_ctx, 7))) throw FailedPredicateException(this, "precpred(_ctx, 7)");
          setState(103);
          _la = _input->LA(1);
          if (!(((((_la - 23) & ~ 0x3fULL) == 0) &&
            ((1ULL << (_la - 23)) & 2199023255555) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(104);
          sere(8);
          break;
        }

        case 3: {
          _localctx = _tracker.createInstance<SereContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleSere);
          setState(105);

          if (!(precpred(_ctx, 6))) throw FailedPredicateException(this, "precpred(_ctx, 6)");
          setState(106);
          _la = _input->LA(1);
          if (!(((((_la - 25) & ~ 0x3fULL) == 0) &&
            ((1ULL << (_la - 25)) & 1116691496961) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(107);
          sere(7);
          break;
        }

        case 4: {
          _localctx = _tracker.createInstance<SereContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleSere);
          setState(108);

          if (!(precpred(_ctx, 5))) throw FailedPredicateException(this, "precpred(_ctx, 5)");
          setState(109);
          match(temporalParser::DELAY);
          setState(111);
          _errHandler->sync(this);

          _la = _input->LA(1);
          if (_la == temporalParser::LSQUARED) {
            setState(110);
            match(temporalParser::LSQUARED);
          }
          setState(114);
          _errHandler->sync(this);

          switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 13, _ctx)) {
          case 1: {
            setState(113);
            match(temporalParser::UINTEGER);
            break;
          }

          default:
            break;
          }
          setState(117);
          _errHandler->sync(this);

          _la = _input->LA(1);
          if (_la == temporalParser::DOTS

          || _la == temporalParser::COL) {
            setState(116);
            _la = _input->LA(1);
            if (!(_la == temporalParser::DOTS

            || _la == temporalParser::COL)) {
            _errHandler->recoverInline(this);
            }
            else {
              _errHandler->reportMatch(this);
              consume();
            }
          }
          setState(120);
          _errHandler->sync(this);

          switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx)) {
          case 1: {
            setState(119);
            _la = _input->LA(1);
            if (!(_la == temporalParser::UINTEGER

            || _la == temporalParser::DOLLAR)) {
            _errHandler->recoverInline(this);
            }
            else {
              _errHandler->reportMatch(this);
              consume();
            }
            break;
          }

          default:
            break;
          }
          setState(123);
          _errHandler->sync(this);

          _la = _input->LA(1);
          if (_la == temporalParser::RSQUARED) {
            setState(122);
            match(temporalParser::RSQUARED);
          }
          setState(125);
          sere(6);
          break;
        }

        case 5: {
          _localctx = _tracker.createInstance<SereContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleSere);
          setState(126);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(127);
          match(temporalParser::COL);
          setState(128);
          sere(4);
          break;
        }

        case 6: {
          _localctx = _tracker.createInstance<SereContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleSere);
          setState(129);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
          setState(130);
          match(temporalParser::SCOL);
          setState(131);
          sere(3);
          break;
        }

        case 7: {
          _localctx = _tracker.createInstance<SereContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleSere);
          setState(132);

          if (!(precpred(_ctx, 12))) throw FailedPredicateException(this, "precpred(_ctx, 12)");
          setState(133);
          match(temporalParser::LSQUARED);
          setState(134);
          match(temporalParser::TIMES);
          setState(136);
          _errHandler->sync(this);

          switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx)) {
          case 1: {
            setState(135);
            match(temporalParser::UINTEGER);
            break;
          }

          default:
            break;
          }
          setState(139);
          _errHandler->sync(this);

          _la = _input->LA(1);
          if (_la == temporalParser::DOTS

          || _la == temporalParser::COL) {
            setState(138);
            _la = _input->LA(1);
            if (!(_la == temporalParser::DOTS

            || _la == temporalParser::COL)) {
            _errHandler->recoverInline(this);
            }
            else {
              _errHandler->reportMatch(this);
              consume();
            }
          }
          setState(142);
          _errHandler->sync(this);

          _la = _input->LA(1);
          if (_la == temporalParser::UINTEGER

          || _la == temporalParser::DOLLAR) {
            setState(141);
            _la = _input->LA(1);
            if (!(_la == temporalParser::UINTEGER

            || _la == temporalParser::DOLLAR)) {
            _errHandler->recoverInline(this);
            }
            else {
              _errHandler->reportMatch(this);
              consume();
            }
          }
          setState(144);
          match(temporalParser::RSQUARED);
          break;
        }

        case 8: {
          _localctx = _tracker.createInstance<SereContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleSere);
          setState(145);

          if (!(precpred(_ctx, 11))) throw FailedPredicateException(this, "precpred(_ctx, 11)");
          setState(146);
          match(temporalParser::LSQUARED);
          setState(147);
          match(temporalParser::PLUS);
          setState(148);
          match(temporalParser::RSQUARED);
          break;
        }

        default:
          break;
        } 
      }
      setState(153);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- BooleanLayerContext ------------------------------------------------------------------

temporalParser::BooleanLayerContext::BooleanLayerContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* temporalParser::BooleanLayerContext::LROUND() {
  return getToken(temporalParser::LROUND, 0);
}

temporalParser::BooleanLayerContext* temporalParser::BooleanLayerContext::booleanLayer() {
  return getRuleContext<temporalParser::BooleanLayerContext>(0);
}

tree::TerminalNode* temporalParser::BooleanLayerContext::RROUND() {
  return getToken(temporalParser::RROUND, 0);
}

temporalParser::BooleanContext* temporalParser::BooleanLayerContext::boolean() {
  return getRuleContext<temporalParser::BooleanContext>(0);
}

tree::TerminalNode* temporalParser::BooleanLayerContext::PLACEHOLDER() {
  return getToken(temporalParser::PLACEHOLDER, 0);
}

tree::TerminalNode* temporalParser::BooleanLayerContext::NOT() {
  return getToken(temporalParser::NOT, 0);
}


size_t temporalParser::BooleanLayerContext::getRuleIndex() const {
  return temporalParser::RuleBooleanLayer;
}

void temporalParser::BooleanLayerContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBooleanLayer(this);
}

void temporalParser::BooleanLayerContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBooleanLayer(this);
}

temporalParser::BooleanLayerContext* temporalParser::booleanLayer() {
  BooleanLayerContext *_localctx = _tracker.createInstance<BooleanLayerContext>(_ctx, getState());
  enterRule(_localctx, 4, temporalParser::RuleBooleanLayer);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(163);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(154);
      match(temporalParser::LROUND);
      setState(155);
      booleanLayer();
      setState(156);
      match(temporalParser::RROUND);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(158);
      boolean(0);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(160);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == temporalParser::NOT) {
        setState(159);
        match(temporalParser::NOT);
      }
      setState(162);
      match(temporalParser::PLACEHOLDER);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TformulaContext ------------------------------------------------------------------

temporalParser::TformulaContext::TformulaContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

temporalParser::BooleanLayerContext* temporalParser::TformulaContext::booleanLayer() {
  return getRuleContext<temporalParser::BooleanLayerContext>(0);
}

tree::TerminalNode* temporalParser::TformulaContext::LROUND() {
  return getToken(temporalParser::LROUND, 0);
}

std::vector<temporalParser::TformulaContext *> temporalParser::TformulaContext::tformula() {
  return getRuleContexts<temporalParser::TformulaContext>();
}

temporalParser::TformulaContext* temporalParser::TformulaContext::tformula(size_t i) {
  return getRuleContext<temporalParser::TformulaContext>(i);
}

tree::TerminalNode* temporalParser::TformulaContext::RROUND() {
  return getToken(temporalParser::RROUND, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::TNOT() {
  return getToken(temporalParser::TNOT, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::NOT() {
  return getToken(temporalParser::NOT, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::WEAK_NEXT() {
  return getToken(temporalParser::WEAK_NEXT, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::LSQUARED() {
  return getToken(temporalParser::LSQUARED, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::UINTEGER() {
  return getToken(temporalParser::UINTEGER, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::RSQUARED() {
  return getToken(temporalParser::RSQUARED, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::STRONG_NEXT() {
  return getToken(temporalParser::STRONG_NEXT, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::ALWAYS() {
  return getToken(temporalParser::ALWAYS, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::STRONG_ALWAYS() {
  return getToken(temporalParser::STRONG_ALWAYS, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::EVENTUALLY() {
  return getToken(temporalParser::EVENTUALLY, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::STRONG_EVENTUALLY() {
  return getToken(temporalParser::STRONG_EVENTUALLY, 0);
}

temporalParser::Sere_implicationContext* temporalParser::TformulaContext::sere_implication() {
  return getRuleContext<temporalParser::Sere_implicationContext>(0);
}

temporalParser::SereContext* temporalParser::TformulaContext::sere() {
  return getRuleContext<temporalParser::SereContext>(0);
}

tree::TerminalNode* temporalParser::TformulaContext::LCURLY() {
  return getToken(temporalParser::LCURLY, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::RCURLY() {
  return getToken(temporalParser::RCURLY, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::WEAK_UNTIL() {
  return getToken(temporalParser::WEAK_UNTIL, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::STRONG_UNTIL() {
  return getToken(temporalParser::STRONG_UNTIL, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::WEAK_RELEASE() {
  return getToken(temporalParser::WEAK_RELEASE, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::STRONG_RELEASE() {
  return getToken(temporalParser::STRONG_RELEASE, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::TAND() {
  return getToken(temporalParser::TAND, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::AND() {
  return getToken(temporalParser::AND, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::TOR() {
  return getToken(temporalParser::TOR, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::OR() {
  return getToken(temporalParser::OR, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::BOR() {
  return getToken(temporalParser::BOR, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::IMPLO() {
  return getToken(temporalParser::IMPLO, 0);
}

tree::TerminalNode* temporalParser::TformulaContext::IMPL() {
  return getToken(temporalParser::IMPL, 0);
}


size_t temporalParser::TformulaContext::getRuleIndex() const {
  return temporalParser::RuleTformula;
}

void temporalParser::TformulaContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTformula(this);
}

void temporalParser::TformulaContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTformula(this);
}


temporalParser::TformulaContext* temporalParser::tformula() {
   return tformula(0);
}

temporalParser::TformulaContext* temporalParser::tformula(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  temporalParser::TformulaContext *_localctx = _tracker.createInstance<TformulaContext>(_ctx, parentState);
  temporalParser::TformulaContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 6;
  enterRecursionRule(_localctx, 6, temporalParser::RuleTformula, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(212);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx)) {
    case 1: {
      setState(166);
      booleanLayer();
      break;
    }

    case 2: {
      setState(167);
      match(temporalParser::LROUND);
      setState(168);
      tformula(0);
      setState(169);
      match(temporalParser::RROUND);
      break;
    }

    case 3: {
      setState(171);

      if (!(canTakeThisNot(_input->LT(1)->getText(),_input->LT(2)->getText()))) throw FailedPredicateException(this, "canTakeThisNot(_input->LT(1)->getText(),_input->LT(2)->getText())");
      setState(172);
      _la = _input->LA(1);
      if (!(_la == temporalParser::TNOT

      || _la == temporalParser::NOT)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(173);
      tformula(11);
      break;
    }

    case 4: {
      setState(174);
      match(temporalParser::WEAK_NEXT);
      setState(176);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 24, _ctx)) {
      case 1: {
        setState(175);
        match(temporalParser::LSQUARED);
        break;
      }

      default:
        break;
      }
      setState(179);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx)) {
      case 1: {
        setState(178);
        match(temporalParser::UINTEGER);
        break;
      }

      default:
        break;
      }
      setState(182);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 26, _ctx)) {
      case 1: {
        setState(181);
        match(temporalParser::NOT);
        break;
      }

      default:
        break;
      }
      setState(185);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 27, _ctx)) {
      case 1: {
        setState(184);
        match(temporalParser::RSQUARED);
        break;
      }

      default:
        break;
      }
      setState(187);
      tformula(10);
      break;
    }

    case 5: {
      setState(188);
      match(temporalParser::STRONG_NEXT);
      setState(190);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 28, _ctx)) {
      case 1: {
        setState(189);
        match(temporalParser::LSQUARED);
        break;
      }

      default:
        break;
      }
      setState(193);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 29, _ctx)) {
      case 1: {
        setState(192);
        match(temporalParser::UINTEGER);
        break;
      }

      default:
        break;
      }
      setState(196);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 30, _ctx)) {
      case 1: {
        setState(195);
        match(temporalParser::RSQUARED);
        break;
      }

      default:
        break;
      }
      setState(198);
      tformula(9);
      break;
    }

    case 6: {
      setState(199);
      _la = _input->LA(1);
      if (!(_la == temporalParser::ALWAYS

      || _la == temporalParser::STRONG_ALWAYS)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(200);
      tformula(8);
      break;
    }

    case 7: {
      setState(201);
      _la = _input->LA(1);
      if (!(_la == temporalParser::EVENTUALLY

      || _la == temporalParser::STRONG_EVENTUALLY)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(202);
      tformula(7);
      break;
    }

    case 8: {
      setState(203);
      sere_implication();
      break;
    }

    case 9: {
      setState(204);

      if (!(_input->index()==0 || canUseSharedOperator(_input->LT(-1)->getText(),_input->LT(2)->getText()))) throw FailedPredicateException(this, "_input->index()==0 || canUseSharedOperator(_input->LT(-1)->getText(),_input->LT(2)->getText())");
      setState(206);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 31, _ctx)) {
      case 1: {
        setState(205);
        match(temporalParser::LCURLY);
        break;
      }

      default:
        break;
      }
      setState(208);
      sere(0);
      setState(210);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 32, _ctx)) {
      case 1: {
        setState(209);
        match(temporalParser::RCURLY);
        break;
      }

      default:
        break;
      }
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(228);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 35, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(226);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 34, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<TformulaContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleTformula);
          setState(214);

          if (!(precpred(_ctx, 6))) throw FailedPredicateException(this, "precpred(_ctx, 6)");
          setState(215);
          _la = _input->LA(1);
          if (!((((_la & ~ 0x3fULL) == 0) &&
            ((1ULL << _la) & 3840) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(216);
          tformula(6);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<TformulaContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleTformula);
          setState(217);

          if (!(precpred(_ctx, 5))) throw FailedPredicateException(this, "precpred(_ctx, 5)");
          setState(218);
          _la = _input->LA(1);
          if (!(_la == temporalParser::TAND

          || _la == temporalParser::AND)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(219);
          tformula(6);
          break;
        }

        case 3: {
          _localctx = _tracker.createInstance<TformulaContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleTformula);
          setState(220);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(221);
          _la = _input->LA(1);
          if (!(((((_la - 25) & ~ 0x3fULL) == 0) &&
            ((1ULL << (_la - 25)) & 1116691496961) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(222);
          tformula(5);
          break;
        }

        case 4: {
          _localctx = _tracker.createInstance<TformulaContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleTformula);
          setState(223);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(224);
          _la = _input->LA(1);
          if (!(_la == temporalParser::IMPL

          || _la == temporalParser::IMPLO)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(225);
          tformula(3);
          break;
        }

        default:
          break;
        } 
      }
      setState(230);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 35, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Sere_implicationContext ------------------------------------------------------------------

temporalParser::Sere_implicationContext::Sere_implicationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

temporalParser::SereContext* temporalParser::Sere_implicationContext::sere() {
  return getRuleContext<temporalParser::SereContext>(0);
}

tree::TerminalNode* temporalParser::Sere_implicationContext::SEREIMPL() {
  return getToken(temporalParser::SEREIMPL, 0);
}

temporalParser::TformulaContext* temporalParser::Sere_implicationContext::tformula() {
  return getRuleContext<temporalParser::TformulaContext>(0);
}

tree::TerminalNode* temporalParser::Sere_implicationContext::LCURLY() {
  return getToken(temporalParser::LCURLY, 0);
}

tree::TerminalNode* temporalParser::Sere_implicationContext::RCURLY() {
  return getToken(temporalParser::RCURLY, 0);
}

tree::TerminalNode* temporalParser::Sere_implicationContext::SEREIMPLO() {
  return getToken(temporalParser::SEREIMPLO, 0);
}


size_t temporalParser::Sere_implicationContext::getRuleIndex() const {
  return temporalParser::RuleSere_implication;
}

void temporalParser::Sere_implicationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSere_implication(this);
}

void temporalParser::Sere_implicationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSere_implication(this);
}

temporalParser::Sere_implicationContext* temporalParser::sere_implication() {
  Sere_implicationContext *_localctx = _tracker.createInstance<Sere_implicationContext>(_ctx, getState());
  enterRule(_localctx, 8, temporalParser::RuleSere_implication);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(252);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 40, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);

      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(233);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx)) {
      case 1: {
        setState(232);
        match(temporalParser::LCURLY);
        break;
      }

      default:
        break;
      }
      setState(235);
      sere(0);
      setState(237);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == temporalParser::RCURLY) {
        setState(236);
        match(temporalParser::RCURLY);
      }
      setState(239);
      match(temporalParser::SEREIMPL);
      setState(240);
      tformula(0);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(243);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx)) {
      case 1: {
        setState(242);
        match(temporalParser::LCURLY);
        break;
      }

      default:
        break;
      }
      setState(245);
      sere(0);
      setState(247);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == temporalParser::RCURLY) {
        setState(246);
        match(temporalParser::RCURLY);
      }
      setState(249);
      match(temporalParser::SEREIMPLO);
      setState(250);
      tformula(0);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StartBooleanContext ------------------------------------------------------------------

temporalParser::StartBooleanContext::StartBooleanContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

temporalParser::BooleanContext* temporalParser::StartBooleanContext::boolean() {
  return getRuleContext<temporalParser::BooleanContext>(0);
}

tree::TerminalNode* temporalParser::StartBooleanContext::EOF() {
  return getToken(temporalParser::EOF, 0);
}


size_t temporalParser::StartBooleanContext::getRuleIndex() const {
  return temporalParser::RuleStartBoolean;
}

void temporalParser::StartBooleanContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStartBoolean(this);
}

void temporalParser::StartBooleanContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStartBoolean(this);
}

temporalParser::StartBooleanContext* temporalParser::startBoolean() {
  StartBooleanContext *_localctx = _tracker.createInstance<StartBooleanContext>(_ctx, getState());
  enterRule(_localctx, 10, temporalParser::RuleStartBoolean);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(254);
    boolean(0);
    setState(255);
    match(temporalParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StartIntContext ------------------------------------------------------------------

temporalParser::StartIntContext::StartIntContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

temporalParser::NumericContext* temporalParser::StartIntContext::numeric() {
  return getRuleContext<temporalParser::NumericContext>(0);
}

tree::TerminalNode* temporalParser::StartIntContext::EOF() {
  return getToken(temporalParser::EOF, 0);
}


size_t temporalParser::StartIntContext::getRuleIndex() const {
  return temporalParser::RuleStartInt;
}

void temporalParser::StartIntContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStartInt(this);
}

void temporalParser::StartIntContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStartInt(this);
}

temporalParser::StartIntContext* temporalParser::startInt() {
  StartIntContext *_localctx = _tracker.createInstance<StartIntContext>(_ctx, getState());
  enterRule(_localctx, 12, temporalParser::RuleStartInt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(257);
    numeric(0);
    setState(258);
    match(temporalParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StartFloatContext ------------------------------------------------------------------

temporalParser::StartFloatContext::StartFloatContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

temporalParser::NumericContext* temporalParser::StartFloatContext::numeric() {
  return getRuleContext<temporalParser::NumericContext>(0);
}

tree::TerminalNode* temporalParser::StartFloatContext::EOF() {
  return getToken(temporalParser::EOF, 0);
}


size_t temporalParser::StartFloatContext::getRuleIndex() const {
  return temporalParser::RuleStartFloat;
}

void temporalParser::StartFloatContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStartFloat(this);
}

void temporalParser::StartFloatContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStartFloat(this);
}

temporalParser::StartFloatContext* temporalParser::startFloat() {
  StartFloatContext *_localctx = _tracker.createInstance<StartFloatContext>(_ctx, getState());
  enterRule(_localctx, 14, temporalParser::RuleStartFloat);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(260);
    numeric(0);
    setState(261);
    match(temporalParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BooleanContext ------------------------------------------------------------------

temporalParser::BooleanContext::BooleanContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* temporalParser::BooleanContext::NOT() {
  return getToken(temporalParser::NOT, 0);
}

std::vector<temporalParser::BooleanContext *> temporalParser::BooleanContext::boolean() {
  return getRuleContexts<temporalParser::BooleanContext>();
}

temporalParser::BooleanContext* temporalParser::BooleanContext::boolean(size_t i) {
  return getRuleContext<temporalParser::BooleanContext>(i);
}

std::vector<temporalParser::NumericContext *> temporalParser::BooleanContext::numeric() {
  return getRuleContexts<temporalParser::NumericContext>();
}

temporalParser::NumericContext* temporalParser::BooleanContext::numeric(size_t i) {
  return getRuleContext<temporalParser::NumericContext>(i);
}

temporalParser::RelopContext* temporalParser::BooleanContext::relop() {
  return getRuleContext<temporalParser::RelopContext>(0);
}

tree::TerminalNode* temporalParser::BooleanContext::EQ() {
  return getToken(temporalParser::EQ, 0);
}

tree::TerminalNode* temporalParser::BooleanContext::NEQ() {
  return getToken(temporalParser::NEQ, 0);
}

temporalParser::BooleanAtomContext* temporalParser::BooleanContext::booleanAtom() {
  return getRuleContext<temporalParser::BooleanAtomContext>(0);
}

tree::TerminalNode* temporalParser::BooleanContext::LROUND() {
  return getToken(temporalParser::LROUND, 0);
}

tree::TerminalNode* temporalParser::BooleanContext::RROUND() {
  return getToken(temporalParser::RROUND, 0);
}

tree::TerminalNode* temporalParser::BooleanContext::AND() {
  return getToken(temporalParser::AND, 0);
}

tree::TerminalNode* temporalParser::BooleanContext::OR() {
  return getToken(temporalParser::OR, 0);
}


size_t temporalParser::BooleanContext::getRuleIndex() const {
  return temporalParser::RuleBoolean;
}

void temporalParser::BooleanContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBoolean(this);
}

void temporalParser::BooleanContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBoolean(this);
}


temporalParser::BooleanContext* temporalParser::boolean() {
   return boolean(0);
}

temporalParser::BooleanContext* temporalParser::boolean(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  temporalParser::BooleanContext *_localctx = _tracker.createInstance<BooleanContext>(_ctx, parentState);
  temporalParser::BooleanContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 16;
  enterRecursionRule(_localctx, 16, temporalParser::RuleBoolean, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(284);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx)) {
    case 1: {
      setState(264);
      match(temporalParser::NOT);
      setState(265);
      boolean(11);
      break;
    }

    case 2: {
      setState(266);
      numeric(0);
      setState(267);
      relop();
      setState(268);
      numeric(0);
      break;
    }

    case 3: {
      setState(270);
      numeric(0);
      setState(271);
      match(temporalParser::EQ);
      setState(272);
      numeric(0);
      break;
    }

    case 4: {
      setState(274);
      numeric(0);
      setState(275);
      match(temporalParser::NEQ);
      setState(276);
      numeric(0);
      break;
    }

    case 5: {
      setState(278);
      booleanAtom();
      break;
    }

    case 6: {
      setState(279);
      numeric(0);
      break;
    }

    case 7: {
      setState(280);
      match(temporalParser::LROUND);
      setState(281);
      boolean(0);
      setState(282);
      match(temporalParser::RROUND);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(300);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 43, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(298);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<BooleanContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleBoolean);
          setState(286);

          if (!(precpred(_ctx, 7))) throw FailedPredicateException(this, "precpred(_ctx, 7)");
          setState(287);
          match(temporalParser::EQ);
          setState(288);
          boolean(8);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<BooleanContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleBoolean);
          setState(289);

          if (!(precpred(_ctx, 6))) throw FailedPredicateException(this, "precpred(_ctx, 6)");
          setState(290);
          match(temporalParser::NEQ);
          setState(291);
          boolean(7);
          break;
        }

        case 3: {
          _localctx = _tracker.createInstance<BooleanContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleBoolean);
          setState(292);

          if (!(precpred(_ctx, 5))) throw FailedPredicateException(this, "precpred(_ctx, 5)");
          setState(293);
          antlrcpp::downCast<BooleanContext *>(_localctx)->booleanop = match(temporalParser::AND);
          setState(294);
          boolean(6);
          break;
        }

        case 4: {
          _localctx = _tracker.createInstance<BooleanContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleBoolean);
          setState(295);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(296);
          antlrcpp::downCast<BooleanContext *>(_localctx)->booleanop = match(temporalParser::OR);
          setState(297);
          boolean(5);
          break;
        }

        default:
          break;
        } 
      }
      setState(302);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 43, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- BooleanAtomContext ------------------------------------------------------------------

temporalParser::BooleanAtomContext::BooleanAtomContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* temporalParser::BooleanAtomContext::BOOLEAN_CONSTANT() {
  return getToken(temporalParser::BOOLEAN_CONSTANT, 0);
}

tree::TerminalNode* temporalParser::BooleanAtomContext::BOOLEAN_VARIABLE() {
  return getToken(temporalParser::BOOLEAN_VARIABLE, 0);
}


size_t temporalParser::BooleanAtomContext::getRuleIndex() const {
  return temporalParser::RuleBooleanAtom;
}

void temporalParser::BooleanAtomContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBooleanAtom(this);
}

void temporalParser::BooleanAtomContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBooleanAtom(this);
}

temporalParser::BooleanAtomContext* temporalParser::booleanAtom() {
  BooleanAtomContext *_localctx = _tracker.createInstance<BooleanAtomContext>(_ctx, getState());
  enterRule(_localctx, 18, temporalParser::RuleBooleanAtom);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(303);
    _la = _input->LA(1);
    if (!(_la == temporalParser::BOOLEAN_CONSTANT

    || _la == temporalParser::BOOLEAN_VARIABLE)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NumericContext ------------------------------------------------------------------

temporalParser::NumericContext::NumericContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* temporalParser::NumericContext::NEG() {
  return getToken(temporalParser::NEG, 0);
}

std::vector<temporalParser::NumericContext *> temporalParser::NumericContext::numeric() {
  return getRuleContexts<temporalParser::NumericContext>();
}

temporalParser::NumericContext* temporalParser::NumericContext::numeric(size_t i) {
  return getRuleContext<temporalParser::NumericContext>(i);
}

temporalParser::IntAtomContext* temporalParser::NumericContext::intAtom() {
  return getRuleContext<temporalParser::IntAtomContext>(0);
}

temporalParser::FloatAtomContext* temporalParser::NumericContext::floatAtom() {
  return getRuleContext<temporalParser::FloatAtomContext>(0);
}

tree::TerminalNode* temporalParser::NumericContext::LROUND() {
  return getToken(temporalParser::LROUND, 0);
}

tree::TerminalNode* temporalParser::NumericContext::RROUND() {
  return getToken(temporalParser::RROUND, 0);
}

tree::TerminalNode* temporalParser::NumericContext::TIMES() {
  return getToken(temporalParser::TIMES, 0);
}

tree::TerminalNode* temporalParser::NumericContext::DIV() {
  return getToken(temporalParser::DIV, 0);
}

tree::TerminalNode* temporalParser::NumericContext::PLUS() {
  return getToken(temporalParser::PLUS, 0);
}

tree::TerminalNode* temporalParser::NumericContext::MINUS() {
  return getToken(temporalParser::MINUS, 0);
}

tree::TerminalNode* temporalParser::NumericContext::LSHIFT() {
  return getToken(temporalParser::LSHIFT, 0);
}

tree::TerminalNode* temporalParser::NumericContext::RSHIFT() {
  return getToken(temporalParser::RSHIFT, 0);
}

tree::TerminalNode* temporalParser::NumericContext::BAND() {
  return getToken(temporalParser::BAND, 0);
}

tree::TerminalNode* temporalParser::NumericContext::BXOR() {
  return getToken(temporalParser::BXOR, 0);
}

tree::TerminalNode* temporalParser::NumericContext::BOR() {
  return getToken(temporalParser::BOR, 0);
}

temporalParser::RangeContext* temporalParser::NumericContext::range() {
  return getRuleContext<temporalParser::RangeContext>(0);
}


size_t temporalParser::NumericContext::getRuleIndex() const {
  return temporalParser::RuleNumeric;
}

void temporalParser::NumericContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNumeric(this);
}

void temporalParser::NumericContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNumeric(this);
}


temporalParser::NumericContext* temporalParser::numeric() {
   return numeric(0);
}

temporalParser::NumericContext* temporalParser::numeric(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  temporalParser::NumericContext *_localctx = _tracker.createInstance<NumericContext>(_ctx, parentState);
  temporalParser::NumericContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 20;
  enterRecursionRule(_localctx, 20, temporalParser::RuleNumeric, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(314);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case temporalParser::NEG: {
        setState(306);
        match(temporalParser::NEG);
        setState(307);
        numeric(12);
        break;
      }

      case temporalParser::INT_VARIABLE:
      case temporalParser::SINTEGER:
      case temporalParser::UINTEGER:
      case temporalParser::GCC_BINARY:
      case temporalParser::HEX:
      case temporalParser::VERILOG_BINARY: {
        setState(308);
        intAtom();
        break;
      }

      case temporalParser::FLOAT_CONSTANT:
      case temporalParser::FLOAT_VARIABLE: {
        setState(309);
        floatAtom();
        break;
      }

      case temporalParser::LROUND: {
        setState(310);
        match(temporalParser::LROUND);
        setState(311);
        numeric(0);
        setState(312);
        match(temporalParser::RROUND);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(341);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(339);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 45, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<NumericContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleNumeric);
          setState(316);

          if (!(precpred(_ctx, 10))) throw FailedPredicateException(this, "precpred(_ctx, 10)");
          setState(317);
          antlrcpp::downCast<NumericContext *>(_localctx)->artop = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == temporalParser::TIMES

          || _la == temporalParser::DIV)) {
            antlrcpp::downCast<NumericContext *>(_localctx)->artop = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(318);
          numeric(11);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<NumericContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleNumeric);
          setState(319);

          if (!(precpred(_ctx, 9))) throw FailedPredicateException(this, "precpred(_ctx, 9)");
          setState(320);
          antlrcpp::downCast<NumericContext *>(_localctx)->artop = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == temporalParser::PLUS

          || _la == temporalParser::MINUS)) {
            antlrcpp::downCast<NumericContext *>(_localctx)->artop = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(321);
          numeric(10);
          break;
        }

        case 3: {
          _localctx = _tracker.createInstance<NumericContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleNumeric);
          setState(322);

          if (!(precpred(_ctx, 8))) throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(323);
          antlrcpp::downCast<NumericContext *>(_localctx)->logop = match(temporalParser::LSHIFT);
          setState(324);
          numeric(9);
          break;
        }

        case 4: {
          _localctx = _tracker.createInstance<NumericContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleNumeric);
          setState(325);

          if (!(precpred(_ctx, 7))) throw FailedPredicateException(this, "precpred(_ctx, 7)");
          setState(326);
          antlrcpp::downCast<NumericContext *>(_localctx)->logop = match(temporalParser::RSHIFT);
          setState(327);
          numeric(8);
          break;
        }

        case 5: {
          _localctx = _tracker.createInstance<NumericContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleNumeric);
          setState(328);

          if (!(precpred(_ctx, 6))) throw FailedPredicateException(this, "precpred(_ctx, 6)");
          setState(329);
          antlrcpp::downCast<NumericContext *>(_localctx)->logop = match(temporalParser::BAND);
          setState(330);
          numeric(7);
          break;
        }

        case 6: {
          _localctx = _tracker.createInstance<NumericContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleNumeric);
          setState(331);

          if (!(precpred(_ctx, 5))) throw FailedPredicateException(this, "precpred(_ctx, 5)");
          setState(332);
          antlrcpp::downCast<NumericContext *>(_localctx)->logop = match(temporalParser::BXOR);
          setState(333);
          numeric(6);
          break;
        }

        case 7: {
          _localctx = _tracker.createInstance<NumericContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleNumeric);
          setState(334);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(335);
          antlrcpp::downCast<NumericContext *>(_localctx)->logop = match(temporalParser::BOR);
          setState(336);
          numeric(5);
          break;
        }

        case 8: {
          _localctx = _tracker.createInstance<NumericContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleNumeric);
          setState(337);

          if (!(precpred(_ctx, 11))) throw FailedPredicateException(this, "precpred(_ctx, 11)");
          setState(338);
          range();
          break;
        }

        default:
          break;
        } 
      }
      setState(343);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- RangeContext ------------------------------------------------------------------

temporalParser::RangeContext::RangeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* temporalParser::RangeContext::LSQUARED() {
  return getToken(temporalParser::LSQUARED, 0);
}

tree::TerminalNode* temporalParser::RangeContext::RSQUARED() {
  return getToken(temporalParser::RSQUARED, 0);
}

std::vector<tree::TerminalNode *> temporalParser::RangeContext::SINTEGER() {
  return getTokens(temporalParser::SINTEGER);
}

tree::TerminalNode* temporalParser::RangeContext::SINTEGER(size_t i) {
  return getToken(temporalParser::SINTEGER, i);
}

std::vector<tree::TerminalNode *> temporalParser::RangeContext::UINTEGER() {
  return getTokens(temporalParser::UINTEGER);
}

tree::TerminalNode* temporalParser::RangeContext::UINTEGER(size_t i) {
  return getToken(temporalParser::UINTEGER, i);
}

tree::TerminalNode* temporalParser::RangeContext::COL() {
  return getToken(temporalParser::COL, 0);
}


size_t temporalParser::RangeContext::getRuleIndex() const {
  return temporalParser::RuleRange;
}

void temporalParser::RangeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRange(this);
}

void temporalParser::RangeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRange(this);
}

temporalParser::RangeContext* temporalParser::range() {
  RangeContext *_localctx = _tracker.createInstance<RangeContext>(_ctx, getState());
  enterRule(_localctx, 22, temporalParser::RuleRange);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(344);
    match(temporalParser::LSQUARED);
    setState(345);
    _la = _input->LA(1);
    if (!(_la == temporalParser::SINTEGER

    || _la == temporalParser::UINTEGER)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(348);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == temporalParser::COL) {
      setState(346);
      match(temporalParser::COL);
      setState(347);
      _la = _input->LA(1);
      if (!(_la == temporalParser::SINTEGER

      || _la == temporalParser::UINTEGER)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
    }
    setState(350);
    match(temporalParser::RSQUARED);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IntAtomContext ------------------------------------------------------------------

temporalParser::IntAtomContext::IntAtomContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

temporalParser::Int_constantContext* temporalParser::IntAtomContext::int_constant() {
  return getRuleContext<temporalParser::Int_constantContext>(0);
}

tree::TerminalNode* temporalParser::IntAtomContext::INT_VARIABLE() {
  return getToken(temporalParser::INT_VARIABLE, 0);
}


size_t temporalParser::IntAtomContext::getRuleIndex() const {
  return temporalParser::RuleIntAtom;
}

void temporalParser::IntAtomContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIntAtom(this);
}

void temporalParser::IntAtomContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIntAtom(this);
}

temporalParser::IntAtomContext* temporalParser::intAtom() {
  IntAtomContext *_localctx = _tracker.createInstance<IntAtomContext>(_ctx, getState());
  enterRule(_localctx, 24, temporalParser::RuleIntAtom);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(354);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case temporalParser::SINTEGER:
      case temporalParser::UINTEGER:
      case temporalParser::GCC_BINARY:
      case temporalParser::HEX:
      case temporalParser::VERILOG_BINARY: {
        enterOuterAlt(_localctx, 1);
        setState(352);
        int_constant();
        break;
      }

      case temporalParser::INT_VARIABLE: {
        enterOuterAlt(_localctx, 2);
        setState(353);
        match(temporalParser::INT_VARIABLE);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Int_constantContext ------------------------------------------------------------------

temporalParser::Int_constantContext::Int_constantContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* temporalParser::Int_constantContext::GCC_BINARY() {
  return getToken(temporalParser::GCC_BINARY, 0);
}

tree::TerminalNode* temporalParser::Int_constantContext::SINTEGER() {
  return getToken(temporalParser::SINTEGER, 0);
}

tree::TerminalNode* temporalParser::Int_constantContext::CONST_SUFFIX() {
  return getToken(temporalParser::CONST_SUFFIX, 0);
}

tree::TerminalNode* temporalParser::Int_constantContext::UINTEGER() {
  return getToken(temporalParser::UINTEGER, 0);
}

tree::TerminalNode* temporalParser::Int_constantContext::VERILOG_BINARY() {
  return getToken(temporalParser::VERILOG_BINARY, 0);
}

tree::TerminalNode* temporalParser::Int_constantContext::HEX() {
  return getToken(temporalParser::HEX, 0);
}


size_t temporalParser::Int_constantContext::getRuleIndex() const {
  return temporalParser::RuleInt_constant;
}

void temporalParser::Int_constantContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInt_constant(this);
}

void temporalParser::Int_constantContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInt_constant(this);
}

temporalParser::Int_constantContext* temporalParser::int_constant() {
  Int_constantContext *_localctx = _tracker.createInstance<Int_constantContext>(_ctx, getState());
  enterRule(_localctx, 26, temporalParser::RuleInt_constant);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(370);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 52, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(356);
      match(temporalParser::GCC_BINARY);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(357);
      match(temporalParser::SINTEGER);
      setState(359);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 49, _ctx)) {
      case 1: {
        setState(358);
        match(temporalParser::CONST_SUFFIX);
        break;
      }

      default:
        break;
      }
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(361);
      match(temporalParser::UINTEGER);
      setState(363);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 50, _ctx)) {
      case 1: {
        setState(362);
        match(temporalParser::CONST_SUFFIX);
        break;
      }

      default:
        break;
      }
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(366);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == temporalParser::UINTEGER) {
        setState(365);
        match(temporalParser::UINTEGER);
      }
      setState(368);
      match(temporalParser::VERILOG_BINARY);
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(369);
      match(temporalParser::HEX);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FloatAtomContext ------------------------------------------------------------------

temporalParser::FloatAtomContext::FloatAtomContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* temporalParser::FloatAtomContext::FLOAT_CONSTANT() {
  return getToken(temporalParser::FLOAT_CONSTANT, 0);
}

tree::TerminalNode* temporalParser::FloatAtomContext::FLOAT_VARIABLE() {
  return getToken(temporalParser::FLOAT_VARIABLE, 0);
}


size_t temporalParser::FloatAtomContext::getRuleIndex() const {
  return temporalParser::RuleFloatAtom;
}

void temporalParser::FloatAtomContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFloatAtom(this);
}

void temporalParser::FloatAtomContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFloatAtom(this);
}

temporalParser::FloatAtomContext* temporalParser::floatAtom() {
  FloatAtomContext *_localctx = _tracker.createInstance<FloatAtomContext>(_ctx, getState());
  enterRule(_localctx, 28, temporalParser::RuleFloatAtom);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(372);
    _la = _input->LA(1);
    if (!(_la == temporalParser::FLOAT_CONSTANT

    || _la == temporalParser::FLOAT_VARIABLE)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RelopContext ------------------------------------------------------------------

temporalParser::RelopContext::RelopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* temporalParser::RelopContext::GT() {
  return getToken(temporalParser::GT, 0);
}

tree::TerminalNode* temporalParser::RelopContext::GE() {
  return getToken(temporalParser::GE, 0);
}

tree::TerminalNode* temporalParser::RelopContext::LT() {
  return getToken(temporalParser::LT, 0);
}

tree::TerminalNode* temporalParser::RelopContext::LE() {
  return getToken(temporalParser::LE, 0);
}


size_t temporalParser::RelopContext::getRuleIndex() const {
  return temporalParser::RuleRelop;
}

void temporalParser::RelopContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRelop(this);
}

void temporalParser::RelopContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRelop(this);
}

temporalParser::RelopContext* temporalParser::relop() {
  RelopContext *_localctx = _tracker.createInstance<RelopContext>(_ctx, getState());
  enterRule(_localctx, 30, temporalParser::RuleRelop);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(374);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 67553994410557440) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Cls_opContext ------------------------------------------------------------------

temporalParser::Cls_opContext::Cls_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* temporalParser::Cls_opContext::RANGE() {
  return getToken(temporalParser::RANGE, 0);
}

tree::TerminalNode* temporalParser::Cls_opContext::GT() {
  return getToken(temporalParser::GT, 0);
}

tree::TerminalNode* temporalParser::Cls_opContext::GE() {
  return getToken(temporalParser::GE, 0);
}

tree::TerminalNode* temporalParser::Cls_opContext::LT() {
  return getToken(temporalParser::LT, 0);
}

tree::TerminalNode* temporalParser::Cls_opContext::LE() {
  return getToken(temporalParser::LE, 0);
}

tree::TerminalNode* temporalParser::Cls_opContext::EQ() {
  return getToken(temporalParser::EQ, 0);
}


size_t temporalParser::Cls_opContext::getRuleIndex() const {
  return temporalParser::RuleCls_op;
}

void temporalParser::Cls_opContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCls_op(this);
}

void temporalParser::Cls_opContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<temporalListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCls_op(this);
}

temporalParser::Cls_opContext* temporalParser::cls_op() {
  Cls_opContext *_localctx = _tracker.createInstance<Cls_opContext>(_ctx, getState());
  enterRule(_localctx, 32, temporalParser::RuleCls_op);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(376);
    _la = _input->LA(1);
    if (!(((((_la - 52) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 52)) & 262175) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool temporalParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 1: return sereSempred(antlrcpp::downCast<SereContext *>(context), predicateIndex);
    case 3: return tformulaSempred(antlrcpp::downCast<TformulaContext *>(context), predicateIndex);
    case 8: return booleanSempred(antlrcpp::downCast<BooleanContext *>(context), predicateIndex);
    case 10: return numericSempred(antlrcpp::downCast<NumericContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool temporalParser::sereSempred(SereContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 8);
    case 1: return precpred(_ctx, 7);
    case 2: return precpred(_ctx, 6);
    case 3: return precpred(_ctx, 5);
    case 4: return precpred(_ctx, 3);
    case 5: return precpred(_ctx, 2);
    case 6: return precpred(_ctx, 12);
    case 7: return precpred(_ctx, 11);

  default:
    break;
  }
  return true;
}

bool temporalParser::tformulaSempred(TformulaContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 8: return canTakeThisNot(_input->LT(1)->getText(),_input->LT(2)->getText());
    case 9: return _input->index()==0 || canUseSharedOperator(_input->LT(-1)->getText(),_input->LT(2)->getText());
    case 10: return precpred(_ctx, 6);
    case 11: return precpred(_ctx, 5);
    case 12: return precpred(_ctx, 4);
    case 13: return precpred(_ctx, 3);

  default:
    break;
  }
  return true;
}

bool temporalParser::booleanSempred(BooleanContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 14: return precpred(_ctx, 7);
    case 15: return precpred(_ctx, 6);
    case 16: return precpred(_ctx, 5);
    case 17: return precpred(_ctx, 4);

  default:
    break;
  }
  return true;
}

bool temporalParser::numericSempred(NumericContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 18: return precpred(_ctx, 10);
    case 19: return precpred(_ctx, 9);
    case 20: return precpred(_ctx, 8);
    case 21: return precpred(_ctx, 7);
    case 22: return precpred(_ctx, 6);
    case 23: return precpred(_ctx, 5);
    case 24: return precpred(_ctx, 4);
    case 25: return precpred(_ctx, 11);

  default:
    break;
  }
  return true;
}

void temporalParser::initialize() {
  ::antlr4::internal::call_once(temporalParserOnceFlag, temporalParserInitialize);
}
