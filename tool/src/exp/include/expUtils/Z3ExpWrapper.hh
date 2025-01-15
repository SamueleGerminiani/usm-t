#pragma once
#include "z3++.h"
#include "z3.h"
#include <memory>

namespace expression {
using Z3ExpPtr = std::shared_ptr<z3::expr>;
using Z3ContextPtr = std::shared_ptr<z3::context>;
class Z3ExpWrapper {
public:
  Z3ExpWrapper(const z3::expr &exp, const Z3ContextPtr &ctx) {
    _exp = std::make_shared<z3::expr>(exp);
    _ctx = ctx;
  }

  virtual ~Z3ExpWrapper() = default;

  Z3ExpPtr _exp;
  Z3ContextPtr _ctx;
};

} // namespace expression

