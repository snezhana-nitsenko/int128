#include "expressions.hpp"

#include <ostream>

Expression::~Expression() = default;

Int128 Expression::eval(const Variables& variables) const {
    return eval_with_lookup([&variables](std::string_view name) -> const Int128* {
        const auto it = variables.find(std::string(name));
        return it == variables.end() ? nullptr : &it->second;
    });
}

Int128 Expression::eval(std::initializer_list<std::pair<std::string, Int128>> variables) const {
    Variables named_values;
    for (const auto& variable : variables) {
        named_values.emplace(variable.first, variable.second);
    }
    return eval(named_values);
}

std::ostream& operator<<(std::ostream& out, const Expression& expression) {
    expression.print(out);
    return out;
}

std::unique_ptr<Expression> Expression::clone_expression(const Expression& expression) {
    return std::unique_ptr<Expression>(expression.clone());
}

Int128 Expression::evaluate(const Expression& expression, const Lookup& lookup) {
    return expression.eval_with_lookup(lookup);
}

Const::Const(const Int128& value)
    : value_(value) {
}

Const::Const(int64_t value)
    : value_(value) {
}

Const* Const::clone() const {
    return new Const(*this);
}

Int128 Const::eval_with_lookup(const Lookup&) const {
    return value_;
}

void Const::print(std::ostream& out) const {
    out << value_;
}

Variable::Variable(std::string_view name)
    : name_(name) {
}

Variable* Variable::clone() const {
    return new Variable(*this);
}

Int128 Variable::eval_with_lookup(const Lookup& lookup) const {
    const Int128* value = lookup(name_);
    return value == nullptr ? Int128() : *value;
}

void Variable::print(std::ostream& out) const {
    out << name_;
}

UnaryExpression::UnaryExpression(const Expression& operand)
    : operand_(clone_expression(operand)) {
}

UnaryExpression::UnaryExpression(const UnaryExpression& other)
    : operand_(clone_expression(*other.operand_)) {
}

UnaryExpression::UnaryExpression(UnaryExpression&&) noexcept = default;

UnaryExpression& UnaryExpression::operator=(const UnaryExpression& other) {
    if (this != &other) {
        operand_ = clone_expression(*other.operand_);
    }
    return *this;
}

UnaryExpression& UnaryExpression::operator=(UnaryExpression&&) noexcept = default;

UnaryExpression::~UnaryExpression() = default;

const Expression& UnaryExpression::operand() const {
    return *operand_;
}

BinaryExpression::BinaryExpression(const Expression& left, const Expression& right)
    : left_(clone_expression(left))
    , right_(clone_expression(right)) {
}

BinaryExpression::BinaryExpression(const BinaryExpression& other)
    : left_(clone_expression(*other.left_))
    , right_(clone_expression(*other.right_)) {
}

BinaryExpression::BinaryExpression(BinaryExpression&&) noexcept = default;

BinaryExpression& BinaryExpression::operator=(const BinaryExpression& other) {
    if (this != &other) {
        left_ = clone_expression(*other.left_);
        right_ = clone_expression(*other.right_);
    }
    return *this;
}

BinaryExpression& BinaryExpression::operator=(BinaryExpression&&) noexcept = default;

BinaryExpression::~BinaryExpression() = default;

const Expression& BinaryExpression::left() const {
    return *left_;
}

const Expression& BinaryExpression::right() const {
    return *right_;
}

Negate::Negate(const Expression& operand)
    : UnaryExpression(operand) {
}

Negate* Negate::clone() const {
    return new Negate(*this);
}

Int128 Negate::eval_with_lookup(const Lookup& lookup) const {
    return -evaluate(operand(), lookup);
}

void Negate::print(std::ostream& out) const {
    out << "(-" << operand() << ")";
}

Add::Add(const Expression& left, const Expression& right)
    : BinaryExpression(left, right) {
}

Add* Add::clone() const {
    return new Add(*this);
}

Int128 Add::eval_with_lookup(const Lookup& lookup) const {
    return evaluate(left(), lookup) + evaluate(right(), lookup);
}

void Add::print(std::ostream& out) const {
    out << "(" << left() << " + " << right() << ")";
}

Subtract::Subtract(const Expression& left, const Expression& right)
    : BinaryExpression(left, right) {
}

Subtract* Subtract::clone() const {
    return new Subtract(*this);
}

Int128 Subtract::eval_with_lookup(const Lookup& lookup) const {
    return evaluate(left(), lookup) - evaluate(right(), lookup);
}

void Subtract::print(std::ostream& out) const {
    out << "(" << left() << " - " << right() << ")";
}

Multiply::Multiply(const Expression& left, const Expression& right)
    : BinaryExpression(left, right) {
}

Multiply* Multiply::clone() const {
    return new Multiply(*this);
}

Int128 Multiply::eval_with_lookup(const Lookup& lookup) const {
    return evaluate(left(), lookup) * evaluate(right(), lookup);
}

void Multiply::print(std::ostream& out) const {
    out << "(" << left() << " * " << right() << ")";
}

Divide::Divide(const Expression& left, const Expression& right)
    : BinaryExpression(left, right) {
}

Divide* Divide::clone() const {
    return new Divide(*this);
}

Int128 Divide::eval_with_lookup(const Lookup& lookup) const {
    return evaluate(left(), lookup) / evaluate(right(), lookup);
}

void Divide::print(std::ostream& out) const {
    out << "(" << left() << " / " << right() << ")";
}

Negate operator-(const Expression& expression) {
    return Negate(expression);
}

Add operator+(const Expression& left, const Expression& right) {
    return Add(left, right);
}

Subtract operator-(const Expression& left, const Expression& right) {
    return Subtract(left, right);
}

Multiply operator*(const Expression& left, const Expression& right) {
    return Multiply(left, right);
}

Divide operator/(const Expression& left, const Expression& right) {
    return Divide(left, right);
}
