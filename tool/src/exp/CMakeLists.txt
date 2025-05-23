project(exp)

SET(EXP_SRCS
    #propositional
    src/formula/atom/NumericExpression.cc
    src/formula/atom/Constant.cc
    src/formula/atom/Variable.cc 
    src/formula/expression/GenericExpression.cc
    src/formula/expression/BitSelector.cc
    src/formula/expression/TypeCast.cc
    #temporal
    src/formula/temporal/BooleanLayer.cc
    src/formula/temporal/Property.cc
    src/formula/temporal/Sere.cc
    #visitors
    src/visitors/PrinterVisitor.cc
    src/visitors/RemapPrinterVisitor.cc
    src/visitors/ExpVisitor.cc
    src/visitors/CopyVisitor.cc
    src/visitors/VarExtractVisitor.cc
    src/visitors/RemapVisitor.cc
    src/visitors/TraceChangerVisitor.cc
    src/visitors/ExpToZ3Visitor.cc
    #utils
    src/expUtils/expUtils.cc
    )


add_library(exp ${EXP_SRCS})
target_link_libraries(exp ${Z3_LIBRARY})

target_include_directories(exp PUBLIC include ../miner/utils/include/ ${Z3_INCLUDE_DIRS})
