grammar temporal;

@parser::header {
#include <string>
// Forward declaration
bool isUnary(const std::string& token);
bool isSharedOperator(const std::string& token);
bool canUseSharedOperator(const std::string& unaryOp, const std::string& sharedOp);
bool canTakeThisNot(const std::string& unaryOp, const std::string& ph);
}

@parser::members {
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

}

import proposition;

formula : tformula EOF
        ;

sere : FIRST_MATCH LROUND sere RROUND
     | LROUND sere RROUND
	 | LCURLY sere RCURLY
	 | <assoc=right> sere LSQUARED TIMES UINTEGER? (DOTS | COL)? (UINTEGER | DOLLAR)? RSQUARED
	 | <assoc=right> sere LSQUARED PLUS RSQUARED
	 | <assoc=right> booleanLayer LSQUARED ASS UINTEGER? (DOTS | COL)? (UINTEGER | DOLLAR)? RSQUARED
	 | <assoc=right> booleanLayer LSQUARED IMPLO UINTEGER? (DOTS | COL)? (UINTEGER | DOLLAR)? RSQUARED
	 | sere BAND sere
	 | sere (TAND|INTERSECT|AND) sere
	 | sere (TOR|OR|BOR) sere
	 | sere DELAY LSQUARED? UINTEGER? (DOTS | COL)? (UINTEGER | DOLLAR)? RSQUARED? sere
	 | DELAY LSQUARED? UINTEGER? (DOTS | COL)? (UINTEGER | DOLLAR)? RSQUARED? sere
	 | sere COL sere
	 | sere SCOL sere
     | booleanLayer
	 ;

booleanLayer: LROUND booleanLayer RROUND
            | boolean 
            | NOT? PLACEHOLDER 
            ;


tformula: booleanLayer
        | LROUND tformula RROUND 
        | <assoc=right> {canTakeThisNot(_input->LT(1)->getText(),_input->LT(2)->getText())}? (TNOT|NOT) tformula 
    	| <assoc=right> NEXT LSQUARED? UINTEGER? RSQUARED? tformula
    	| <assoc=right> ALWAYS tformula 
    	| <assoc=right> EVENTUALLY tformula 
    	| <assoc=right> tformula (UNTIL|RELEASE) tformula
     	| tformula (TAND|AND) tformula 
    	| tformula (TOR|OR|BOR) tformula 
        | <assoc=right> tformula (IMPLO|IMPL) tformula
        | sere_implication
        | {_input->index()==0 || canUseSharedOperator(_input->LT(-1)->getText(),_input->LT(2)->getText())}? LCURLY? sere RCURLY?  
    	;

sere_implication :
    	    | <assoc=right> LCURLY? sere RCURLY? SEREIMPL tformula
    	    | <assoc=right> LCURLY? sere RCURLY? SEREIMPLO tformula
        	;

PLACEHOLDER: 'P' UINTEGER;


EVENTUALLY
    : 'F' | 'eventually'
    ;

ALWAYS
    : 'G' | 'always'
    ;

NEXT
    : 'X' | 'nexttime' | 'next'
    ;

UNTIL
    : 'W' | 'until' | 'U'
    ;

RELEASE
    : 'R'
    ;

DOTS
    : '..'
    ;

IMPL
    : '=>'
    ;

IMPLO
    : '->'
    ;

IFF
    : '<->'
    ;

SEREIMPL
    : '|=>'
    | '[]=>'
    ;

SEREIMPLO
    : '|->'
    | '[]->'
    ;

ASS
    : '='
    ;

DELAY
    : '##'
    ;

SCOL
    : ';'
    ;

FIRST_MATCH
    : 'first_match'
    ;

TNOT: 'not';

TAND: 'and';

INTERSECT : 'intersect';

TOR : 'or';

