#pragma once

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json {

class Node;

using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
public:
    using Value = std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict>;
    
    Node () = default;
     Node (std::nullptr_t null);
     Node (int value);
     Node (double value);
     Node (std::string value);
     Node (bool value);
     Node (Array value);
     Node (Dict value);

    // проверка значения внутри 
    [[nodiscard]] [[maybe_unused]] bool IsNull()       const;
    [[nodiscard]] bool IsInt()        const;
    [[nodiscard]] bool IsDouble()     const;
    [[nodiscard]] bool IsPureDouble() const;
    [[nodiscard]] bool IsString()     const;
    [[nodiscard]] bool IsBool()       const;
    [[nodiscard]] bool IsArray()      const;
    [[nodiscard]] bool IsMap()        const;

    // возвращает значение из variant
    [[nodiscard]] int     AsInt()                  const;
    [[nodiscard]] double  AsDouble()               const;
    [[nodiscard]] const   std::string& AsString()  const;
    [[nodiscard]] bool    AsBool()                 const;
    [[nodiscard]] const   Array& AsArray()         const;
    [[nodiscard]] const   Dict& AsMap()            const;

    [[nodiscard]] const Value& GetValue() const;

private:
    Value value_;
};

bool operator== (const Node& lhs, const Node& rhs);
bool operator!= (const Node& lhs, const Node& rhs);

// -----------Document---------------

class Document {
public:
    explicit Document(Node root);
    [[nodiscard]] const Node& GetRoot() const;

private:
    Node root_;
};

bool operator==(const Document& lhs, const Document& rhs);
bool operator!=(const Document& lhs, const Document& rhs);

Document Load(std::istream& input);

// -----------Print---------------

struct PrintContext {
    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void PrintIndent() const;

    // Возвращает новый контекст вывода с увеличенным смещением
    [[nodiscard]] PrintContext Indented() const;
};

    [[maybe_unused]] void PrintValue (std::nullptr_t, const PrintContext& ctx);

// PrintValue для int и double
template <typename Value>
[[maybe_unused]] void PrintValue (const Value& value, const PrintContext& ctx) {
    ctx.out << value;
}

void PrintValue (std::string value, const PrintContext& ctx);
[[maybe_unused]] void PrintValue (bool value,        const PrintContext& ctx);
[[maybe_unused]] void PrintValue (Array values,      const PrintContext& ctx);
void PrintValue (Dict values,       const PrintContext& ctx);

void PrintNode (const Node& node, const PrintContext& ctx);
void Print(const Document& doc, std::ostream& out);

}  // namespace json