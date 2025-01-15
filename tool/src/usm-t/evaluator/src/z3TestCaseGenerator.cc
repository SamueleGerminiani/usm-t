#include "z3TestCaseGenerator.hh"
#include "expUtils/Z3ExpWrapper.hh"
#include "expUtils/expUtils.hh"
#include <exp.hh>

using namespace expression;

namespace z3 {

z3::expr negateTestCases(const std::vector<TestCase> &tcs,
                         context &c);

std::vector<TestCase> Z3TestCaseGenerator::generateTestCase(
    expression::PropositionPtr &p, size_t n,
    const std::vector<TestCase> &alreadyGen) {

  //const size_t manyTC = 100;
  //if (n >= manyTC) {
  //  messageInfo("Generating many testcases(" + std::to_string(n) +
  //              ")... this could take a while");
  //}

  Z3ExpWrapper z3Expr = expression::to_z3exp(p);
  std::vector<std::pair<VarName, VarType>> contained_vars =
      expression::getVars(p);

  // DEBUG
  // std::cout << "z3Expr: " << z3Expr << "\n";
  // bool doubleTC = false;

  z3::solver s(*z3Expr._ctx);
  z3::params params(*z3Expr._ctx);

  // max 2 seconds before aborting the solver
  params.set(":timeout", 2000U);
  s.set(params);
  s.add(*z3Expr._exp);

  std::vector<TestCase> res;

  // add counterexamples
  if (!alreadyGen.empty()) {
    s.add(negateTestCases(alreadyGen, *z3Expr._ctx, contained_vars));
  }

  for (size_t i = 0; i < n; i++) {
    // DEBUG
    // std::cout << "-----------------TestCase[" << i <<
    // "]---------------\n"; std::cout << "Solver:" << s << "\n"; std::cout << s << "\n";
    if (s.check() == sat) {
      // this variables is a chain of &&s of all models found
      z3::expr thisInstanceTestCase = z3Expr._ctx->bool_val(true);
      z3::model model = s.get_model();
      /* enable model completion, otherwise the solver might produce a
       partial model */
      model.eval(*z3Expr._exp, true);

      // DEBUG
      // std::cout<<"-------->"<<_converter.getNumberVariables()<<" vs
      // "<<model.size()<<"\n";
      // std::cout << "Model.size():" << model.size() << "\n";
      // std::cout << model << "\n";
      assert(model.size() == contained_vars.size());

      TestCase tc;
      for (size_t j = 0; j < model.size(); j++) {
        Assignment assign;
        TypeValue tv;
        VarValue vv;

        // proceed accordingly for each variable type

        // NUMERIC
        if (model.get_const_interp(model[j]).is_fpa()) {
          std::string varName = model[j].name().str();
          // DEBUG
          // doubleTC = true;

          // 32 bit float
          if (Z3_fpa_get_sbits(
                  model.get_const_interp(model[j]).ctx(),
                  model.get_const_interp(model[j]).get_sort()) ==
              24) {
            uint64_t v_significand;
            Z3_fpa_get_numeral_significand_uint64(
                model.get_const_interp(model[j]).ctx(),
                model.get_const_interp(model[j]), &v_significand);
            int64_t v_exp;
            Z3_fpa_get_numeral_exponent_int64(
                model.get_const_interp(model[j]).ctx(),
                model.get_const_interp(model[j]), &v_exp, true);
            int v_sign_int;
            Z3_fpa_get_numeral_sign(
                model.get_const_interp(model[j]).ctx(),
                model.get_const_interp(model[j]), &v_sign_int);

            /* convert to un unsigned type to perform memory
             operations*/
            uint64_t v_sign = v_sign_int;

            // compose the 3 parts of the float
            uint32_t composed =
                ((v_sign << 63ULL) | (v_exp << 55ULL) |
                 (v_significand << 32ULL)) >>
                32ULL;

            /* convert the integer representation of a float to a C
             float*/
            intToFloat convertedComposed;
            convertedComposed.i = composed;

            vv.dVal = convertedComposed.f;
            tv = std::make_pair(expression::ExpType::Float, vv);
            assign = std::make_pair(varName, tv);

            expr var = z3Expr._ctx->fpa_const(varName.c_str(), 8, 24);
            thisInstanceTestCase =
                thisInstanceTestCase &&
                (var == model.get_const_interp(model[j]));

            // 64 bit float
          } else if (Z3_fpa_get_sbits(
                         model.get_const_interp(model[j]).ctx(),
                         model.get_const_interp(model[j])
                             .get_sort()) == 53) {

            vv.dVal = Z3_get_numeral_double(
                model.get_const_interp(model[j]).ctx(),
                model.get_const_interp(model[j]));
            tv = std::make_pair(expression::ExpType::Float, vv);
            assign = std::make_pair(varName, tv);

            expr var =
                z3Expr._ctx->fpa_const(varName.c_str(), 11, 53);
            thisInstanceTestCase =
                thisInstanceTestCase &&
                (var == model.get_const_interp(model[j]));

          } else {
            messageError("Uknown float sort!");
          }

          // BOOL
        } else if (model.get_const_interp(model[j]).is_bool()) {
          std::string varName = model[j].name().str();
          bool v = Z3_get_bool_value(
                       model.get_const_interp(model[j]).ctx(),
                       model.get_const_interp(model[j])) == Z3_L_TRUE
                       ? true
                       : false;

          vv.bVal = v;
          tv = std::make_pair(expression::ExpType::Bool, vv);
          assign = std::make_pair(varName, tv);

          expr var = z3Expr._ctx->bool_const(varName.c_str());
          thisInstanceTestCase =
              thisInstanceTestCase && (v ? var : !var);

          /* LOGIC (char,short,int,long long and their unsigned
           version)*/
        } else if (model.get_const_interp(model[j]).is_bv()) {
          std::string varName = model[j].name().str();
          uint64_t v =
              model.get_const_interp(model[j]).get_numeral_uint64();

          messageErrorIf(
              std::find_if(
                  contained_vars.begin(), contained_vars.end(),
                  [&varName](const std::pair<VarName, VarType> &e) {
                    return e.first == varName;
                  }) == contained_vars.end(),
              "Variable " + varName +
                  " not found in contained_vars!");

          VarType varTypeSize =
              std::find_if(
                  contained_vars.begin(), contained_vars.end(),
                  [&varName](const std::pair<VarName, VarType> &e) {
                    return e.first == varName;
                  })
                  ->second;

          // SLOGIC(signed integers)
          if (varTypeSize.first == expression::ExpType::SInt) {
            /*check if the most significand bit is 1 (negative
            integer) or 0 (positive integer) and proceed
            accordinly*/

            if (v & (1ULL << (varTypeSize.second - 1))) {
              // negative value needs conversion to 2s complement
              uint64_t mask = ULLONG_MAX;
              mask = mask << (varTypeSize.second - 1);
              vv.sVal = (int64_t)(v | mask);
            } else {
              // positive value
              vv.sVal = (int64_t)(v);
            }
            tv = std::make_pair(expression::ExpType::SInt, vv);
          } else if (varTypeSize.first == expression::ExpType::UInt) {
            // ULOGIC(unsigned integers)
            vv.uVal = v;
            tv = std::make_pair(expression::ExpType::UInt, vv);
          } else {
            messageError(
                "Variable is of type z3 integer but an non-integer "
                "expression type is found!");
          }

          assign = std::make_pair(varName, tv);

          expr var = z3Expr._ctx->bv_const(varName.c_str(),
                                           varTypeSize.second);
          thisInstanceTestCase =
              thisInstanceTestCase &&
              (var == model.get_const_interp(model[j]));
        } else {
          messageError("Uknown variable type!");
        } // if type

        tc.push_back(assign);

      } // for model

      res.push_back(tc);

      /* add the model negated to generate a different model in the next
       * cycle*/
      s.add(!thisInstanceTestCase);
    } else {
      if (res.empty()) {
        messageError(
            "Z3 couldn't find any testcases for proposition: " +
            prop2String(p));
      } else {
        //  messageWarning("Z3 couldn't find all the requested testcases "
        //                 "for proposition: " +
        //                 prop2String(p));
      }

      //std::cout << "Unsat! Can't generate more testcases...\n";
      break;
    } // if check

  } // for n testcases

  // sort assignments by name
  for (TestCase &tc : res) {
    std::sort(std::begin(tc), std::end(tc),
              [](Assignment &e1, Assignment &e2) {
                return e1.first < e2.first;
              });
  }

  // DEBUG - check if all testcases are different(for float/double)
  // if (doubleTC) {
  //    for (size_t i = 0; i < res.size(); i++) {
  //        for (size_t j = i + 1; j < res.size(); j++) {
  //            for (size_t k = 0; k < res[0].size(); k++) {
  //                if (res[i][k].second.second.dVal !=
  //                    res[j][k].second.second.dVal) {
  //                    goto nextComparison;
  //                }
  //            }
  //            messageError("Found two equal testcases!");
  //        nextComparison:;
  //        }
  //    }
  //}
  return res;

} // end fun

z3::expr Z3TestCaseGenerator::negateTestCases(
    const std::vector<TestCase> &tcs, context &c,
    const std::vector<std::pair<VarName, VarType>> &contained_vars) {

  z3::expr res = c.bool_val(true);
  for (const auto &tc : tcs) {
    z3::expr chain = c.bool_val(true);
    for (const auto &assignment : tc) {
      z3::expr var(c);
      z3::expr val(c);
      messageErrorIf(
          std::find_if(
              contained_vars.begin(), contained_vars.end(),
              [&assignment](const std::pair<VarName, VarType> &e) {
                return e.first == assignment.first;
              }) == contained_vars.end(),
          "Variable " + assignment.first +
              " not found in contained_vars!");
      auto ts =
          std::find_if(
              contained_vars.begin(), contained_vars.end(),
              [&assignment](const std::pair<VarName, VarType> &e) {
                return e.first == assignment.first;
              })
              ->second;
      switch (assignment.second.first) {
      case expression::ExpType::Bool:
        var = c.bool_const(assignment.first.c_str());
        val = c.bool_val(assignment.second.second.bVal);
        break;
      case expression::ExpType::UInt:
        var = c.bv_const(assignment.first.c_str(), ts.second);
        val = c.bv_val(assignment.second.second.uVal, ts.second);
        break;
      case expression::ExpType::SInt:
        var = c.bv_const(assignment.first.c_str(), ts.second);
        val = c.bv_val(assignment.second.second.sVal, ts.second);
        break;
      case expression::ExpType::Float:
        if (ts.second == 32) {
          var = c.fpa_const(assignment.first.c_str(), 8, 24);
          val = c.fpa_val((float)assignment.second.second.dVal);
        } else {
          var = c.fpa_const(assignment.first.c_str(), 11, 53);
          val = c.fpa_val((double)assignment.second.second.dVal);
        }
        break;
      default:
        messageError("This should not happen!");
        break;
      }
      chain = chain && var == val;
    }
    chain = !(chain);
    res = res && chain;
  }
  return res;
}

std::ostream &operator<<(std::ostream &o, const TestCase &tc) {
  o << "--------------------\n";
  for (const auto &assignment : tc) {
    const z3::VarName &vn = assignment.first;
    const z3::TypeValue &tv = assignment.second;
    std::cout << (int)tv.first << " ";
    switch (tv.first) {
    case expression::ExpType::Bool:
      std::cout << vn << ":" << tv.second.bVal << "\n";
      break;
    case expression::ExpType::UInt:
      std::cout << vn << ":" << tv.second.uVal << "\n";
      break;
    case expression::ExpType::SInt:
      std::cout << vn << ":" << tv.second.sVal << "\n";
      break;
    case expression::ExpType::Float:
      std::cout << vn << ":" << tv.second.dVal << "\n";
      break;
    default:
      break;
    }
  }

  return o;
}

bool operator==(TypeValue e1, TypeValue e2) {

  messageErrorIf(e1.first != e2.first,
                 "== on variables with different types: " +
                     std::to_string((int)e1.first) + " vs " +
                     std::to_string((int)e2.first));

  switch (e1.first) {
  case expression::ExpType::Bool:
    return e1.second.bVal == e2.second.bVal;
    break;
  case expression::ExpType::UInt:
    return e1.second.uVal == e2.second.uVal;
    break;
  case expression::ExpType::SInt:
    return e1.second.sVal == e2.second.sVal;
    break;
  case expression::ExpType::Float:
    return e1.second.dVal == e2.second.dVal;
    break;
  default:
    return 0;
  }

  return false;
}

std::ostream &operator<<(std::ostream &os, const TypeValue e1) {

  switch (e1.first) {
  case expression::ExpType::Bool:
    os << e1.second.bVal;
    break;
  case expression::ExpType::UInt:
    os << e1.second.uVal;
    break;
  case expression::ExpType::SInt:
    os << e1.second.sVal;
    break;
  case expression::ExpType::Float:
    os << e1.second.dVal;
    break;
  default:
    return os;
  }

  return os;
}

} // namespace z3
