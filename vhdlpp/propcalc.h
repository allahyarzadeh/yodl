#ifndef IVL_PROPCALC
#define IVL_PROPCALC

#include <string>
#include <vector>
#include <map>
#include <set>

struct PropcalcFormula {
    virtual ~PropcalcFormula() = default;

};

struct PropcalcVar : PropcalcFormula {
    std::string name_;
    PropcalcVar(const char *n) : name_(n) {}
};

struct PropcalcConstant : PropcalcFormula {
    bool value_;
    PropcalcConstant(const bool v) : value_(v) {}
};

struct PropcalcTerm : PropcalcFormula {
    enum operator_t { AND, OR, NOR, XOR, NAND, XNOR, IFTHEN };
    PropcalcFormula *l_, *r_;
    operator_t op_;
    PropcalcTerm(PropcalcFormula *a, operator_t o, PropcalcFormula *b)
        : l_(a), r_(b), op_(o) {}
};

struct PropcalcNot : PropcalcFormula {
    PropcalcFormula *r_;
    PropcalcNot(PropcalcFormula *f) : r_(f) {}
};

std::ostream &operator<<(std::ostream &, PropcalcFormula *);

class PropcalcApi {
public:
    static bool evaluate(PropcalcFormula *,
                         std::map<std::string, bool> &);
    static void extractNames(PropcalcFormula *, std::set<std::string> &);
    static bool prove(PropcalcFormula *form);

private:
    static bool proveH(PropcalcFormula *form,
                        std::vector<std::string> todo,
                        std::map<std::string, bool> m);
};

#endif /* IVL_PROPCALC */