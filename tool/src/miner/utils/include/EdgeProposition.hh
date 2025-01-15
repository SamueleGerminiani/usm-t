#pragma once

#include <stddef.h>
#include <string>
#include <vector>

#include "formula/atom/Atom.hh"

namespace harm {

/*! \class EdgeProposition
    \brief Class representing an edge of Automaton
*/
class EdgeProposition {
public:
  virtual bool evaluate(size_t time) = 0;
  virtual std::string toString() = 0;
  virtual ~EdgeProposition() {}
  std::vector<EdgeProposition *> getOperands() { return _operands; }

protected:
  std::vector<EdgeProposition *> _operands;
};
class EdgeAnd : public EdgeProposition {
public:
  EdgeAnd(EdgeProposition *p1, EdgeProposition *p2);

  EdgeAnd(std::vector<EdgeProposition *> pp);
  ~EdgeAnd() override;
  bool evaluate(size_t time) override;
  std::string toString() override;
};

class EdgeOr : public EdgeProposition {
public:
  EdgeOr(EdgeProposition *p1, EdgeProposition *p2);
  EdgeOr(std::vector<EdgeProposition *> pp);
  ~EdgeOr() override;
  bool evaluate(size_t time) override;
  std::string toString() override;
};
class EdgeNot : public EdgeProposition {
public:
  EdgeNot(EdgeProposition *p);

  ~EdgeNot() override;

  bool evaluate(size_t time) override;

  std::string toString() override;
};
class EdgeTrue : public EdgeProposition {
public:
  EdgeTrue() : EdgeProposition() {}
  ~EdgeTrue() override {}
  bool evaluate(size_t time) override;
  std::string toString() override;
};
class EdgeFalse : public EdgeProposition {
public:
  EdgeFalse() : EdgeProposition() {}
  ~EdgeFalse() override {}
  bool evaluate(size_t time) override;
  std::string toString() override;
};

class EdgePlaceholder : public EdgeProposition {
public:
  EdgePlaceholder(const expression::PropositionPtrPtr &toProp,
                  const std::string &phName = "");
  bool evaluate(size_t time);
  std::string toString();
  expression::PropositionPtrPtr _toProp;
  std::string _phName = "";
};

class EdgeInst : public EdgeProposition {
public:
  EdgeInst(const expression::PropositionPtr &toInst,
           const std::string &phName = "");
  bool evaluate(size_t time);
  std::string toString();
  expression::PropositionPtr _toInst;
  std::string _phName = "";
};
inline bool isEdgeAnd(EdgeProposition *p) {
  return dynamic_cast<EdgeAnd *>(p) != nullptr;
}
inline bool isEdgeOr(EdgeProposition *p) {
  return dynamic_cast<EdgeOr *>(p) != nullptr;
}
inline bool isEdgeNot(EdgeProposition *p) {
  return dynamic_cast<EdgeNot *>(p) != nullptr;
}
inline bool isEdgeTrue(EdgeProposition *p) {
  return dynamic_cast<EdgeTrue *>(p) != nullptr;
}
inline bool isEdgeFalse(EdgeProposition *p) {
  return dynamic_cast<EdgeFalse *>(p) != nullptr;
}
inline bool isEdgePlaceholder(EdgeProposition *p) {
  return dynamic_cast<EdgePlaceholder *>(p) != nullptr;
}
inline bool isEdgeInst(EdgeProposition *p) {
  return dynamic_cast<EdgeInst *>(p) != nullptr;
}

} // namespace harm
