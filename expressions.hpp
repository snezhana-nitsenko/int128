#ifndef EXPRESSIONS_HPP
#define EXPRESSIONS_HPP

#include "int128.hpp"

#include <functional>
#include <initializer_list>
#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

class Expression {
public:
    using Variables = std::map<std::string, Int128>;
    using Lookup = std::function<const Int128*(std::string_view)>;

    virtual ~Expression();

    Int128 eval(const Variables& variables) const;
    Int128 eval(std::initializer_list<std::pair<std::string, Int128>> variables) const;

    template <class Map>
    auto eval(const Map& variables) const
        -> std::enable_if_t<!std::is_same_v<std::decay_t<Map>, Variables>, Int128> {
        return eval_with_lookup([&variables](std::string_view name) -> const Int128* {
            const auto it = variables.find(std::string(name));
            return it == variables.end() ? nullptr : &it->second;
        });
    }

    virtual Expression* clone() const = 0;

    friend std::ostream& operator<<(std::ostream& out, const Expression& expression);

protected:
    static std::unique_ptr<Expression> clone_expression(const Expression& expression);
    static Int128 evaluate(const Expression& expression, const Lookup& lookup);

    virtual Int128 eval_with_lookup(const Lookup& lookup) const = 0;

private:
    virtual void print(std::ostream& out) const = 0;
};

class Const final : public Expression {
public:
    explicit Const(const Int128& value);
    explicit Const(int64_t value);

    Const* clone() const override;

private:
    Int128 eval_with_lookup(const Lookup&) const override;
    void print(std::ostream& out) const override;

    Int128 value_;
};

class Variable final : public Expression {
public:
    explicit Variable(std::string_view name);

    Variable* clone() const override;

private:
    Int128 eval_with_lookup(const Lookup& lookup) const override;
    void print(std::ostream& out) const override;

    std::string name_;
};

class UnaryExpression : public Expression {
public:
    explicit UnaryExpression(const Expression& operand);
    UnaryExpression(const UnaryExpression& other);
    UnaryExpression(UnaryExpression&&) noexcept;
    UnaryExpression& operator=(const UnaryExpression& other);
    UnaryExpression& operator=(UnaryExpression&&) noexcept;
    ~UnaryExpression() override;

protected:
    const Expression& operand() const;

private:
    std::unique_ptr<Expression> operand_;
};

class BinaryExpression : public Expression {
public:
    BinaryExpression(const Expression& left, const Expression& right);
    BinaryExpression(const BinaryExpression& other);
    BinaryExpression(BinaryExpression&&) noexcept;
    BinaryExpression& operator=(const BinaryExpression& other);
    BinaryExpression& operator=(BinaryExpression&&) noexcept;
    ~BinaryExpression() override;

protected:
    const Expression& left() const;
    const Expression& right() const;

private:
    std::unique_ptr<Expression> left_;
    std::unique_ptr<Expression> right_;
};

class Negate final : public UnaryExpression {
public:
    explicit Negate(const Expression& operand);

    Negate* clone() const override;

private:
    Int128 eval_with_lookup(const Lookup& lookup) const override;
    void print(std::ostream& out) const override;
};

class Add final : public BinaryExpression {
public:
    Add(const Expression& left, const Expression& right);

    Add* clone() const override;

private:
    Int128 eval_with_lookup(const Lookup& lookup) const override;
    void print(std::ostream& out) const override;
};

class Subtract final : public BinaryExpression {
public:
    Subtract(const Expression& left, const Expression& right);

    Subtract* clone() const override;

private:
    Int128 eval_with_lookup(const Lookup& lookup) const override;
    void print(std::ostream& out) const override;
};

class Multiply final : public BinaryExpression {
public:
    Multiply(const Expression& left, const Expression& right);

    Multiply* clone() const override;

private:
    Int128 eval_with_lookup(const Lookup& lookup) const override;
    void print(std::ostream& out) const override;
};

class Divide final : public BinaryExpression {
public:
    Divide(const Expression& left, const Expression& right);

    Divide* clone() const override;

private:
    Int128 eval_with_lookup(const Lookup& lookup) const override;
    void print(std::ostream& out) const override;
};

Negate operator-(const Expression& expression);
Add operator+(const Expression& left, const Expression& right);
Subtract operator-(const Expression& left, const Expression& right);
Multiply operator*(const Expression& left, const Expression& right);
Divide operator/(const Expression& left, const Expression& right);

#endif
