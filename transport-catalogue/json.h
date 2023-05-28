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

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
    
class Node : private Value {
public:
    using variant::variant;
    using Value = variant;
    
    Node(Value value);

    bool IsInt() const;
    bool IsPureDouble() const;
    bool IsDouble() const;
    bool IsBool() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsString() const;
    bool IsDict() const;

    int AsInt() const;
    double AsDouble() const;
    bool AsBool() const;
    const Array& AsArray() const;
    const std::string& AsString() const;
    const Dict& AsDict() const;

    const Value& GetValue() const;
};
    
inline bool operator==(const Node& lhs, const Node& rhs) {
    return lhs.GetValue() == rhs.GetValue();
}
    
inline bool operator!=(const Node& lhs, const Node& rhs) {
    return !(lhs == rhs);
}
    
class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

private:
    Node root_;
};

inline bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.GetRoot() == rhs.GetRoot();
}

inline bool operator!=(const Document& lhs, const Document& rhs) {
    return !(lhs == rhs);
}
    
Document Load(std::istream& input);
    
void Print(const Document& doc, std::ostream& output);
    
}  // namespace json