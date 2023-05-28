#include "json_builder.h"

namespace json {
using namespace std::literals;

Builder::Context::Context(Builder& builder) : builder_(builder) {}
    
bool Builder::IsEmpty() {
    return nodes_stack_.size() == 0;
}

Node* Builder::GetLastNode() {
    if (IsEmpty()) {
        throw std::logic_error("Nodes stack is empty"s);
    }
    return nodes_stack_.back();
}
    
Builder::KeyContext Builder::Key(std::string key) {
    if (GetLastNode()->IsDict()) {
        Dict& current_node = const_cast<Dict&>(GetLastNode()->AsDict());
        current_node.emplace(key, key);
        nodes_stack_.push_back(&current_node[key]);
    } else {
        throw std::logic_error("Calling function Key() isn't in context"s);
    }
    return {*this};
}

Node* Builder::Insert(Node node) {
    if (IsEmpty() && root_.IsNull()) {
        root_ = Node(node);
        return &root_;
    } else if (GetLastNode()->IsArray()) {
        Array& current_node = const_cast<Array&>(GetLastNode()->AsArray());
        current_node.push_back(node);
        return &current_node.back();
    } else if (GetLastNode()->IsString()) {
        const std::string key = GetLastNode()->AsString();
        nodes_stack_.pop_back();
        if (GetLastNode()->IsDict()) {
            Dict& current_node = const_cast<Dict&>(GetLastNode()->AsDict());
            current_node[key] = node;
            return &current_node[key];
        } else {
            throw std::logic_error("Inserted value isn't for this key"s);
        }
    } else {
        throw std::logic_error("Inserted value isn't in context"s);
    }
}

Builder& Builder::Value(Node::Value value) {
    Insert(value);
    return *this;
}

Builder::DictItemContext Builder::StartDict() {
    nodes_stack_.push_back(Insert(Dict{}));
    return {*this};
}

Builder::ArrayItemContext Builder::StartArray() {
    nodes_stack_.push_back(Insert(Array{}));
    return {*this};
}

Builder& Builder::EndDict() {
    if (GetLastNode()->IsDict()) {
        nodes_stack_.pop_back();
    } else {
        throw std::logic_error("Calling function EndDict() isn't in context"s);
    }
    return *this;
}

Builder& Builder::EndArray() {
    if (GetLastNode()->IsArray()) {
        nodes_stack_.pop_back();
    } else {
        throw std::logic_error("Calling function EndArray() isn't in context"s);
    }
    return *this;
}

Node Builder::Build() {
    if (root_.IsNull()) {
        throw std::logic_error("Calling function Build() is used for defined objects"s);
    } else if (!IsEmpty()) {
        throw std::logic_error("Calling function Build() is used for ended objects"s);
    }
    return root_;
}

Builder::DictItemContext Builder::Context::StartDict() {
    builder_.StartDict();
    return {builder_};
}

Builder::ArrayItemContext Builder::Context::StartArray() {
    builder_.StartArray();
    return {builder_};
}

Builder& Builder::Context::EndDict() {
    builder_.EndDict();
    return builder_;
}

Builder& Builder::Context::EndArray() {
    builder_.EndArray();
    return builder_;
}

Builder::KeyContext Builder::Context::Key(std::string key) {
    builder_.Key(key);
    return {builder_};
}

Builder::DictItemContext Builder::KeyContext::Value(Node::Value value) {
    builder_.Value(value);
    return {builder_};
}

Builder::ArrayItemContext Builder::ArrayItemContext::Value(Node::Value value) {
    builder_.Value(value);
    return {builder_};
}
} // namespace json