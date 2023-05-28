#pragma once
#include "json.h"

#include <string>
#include <vector>

namespace json {

class Builder {
public:
    class DictItemContext;
    class ArrayItemContext;
    class KeyContext;

    class Context {
    public:
        Context(Builder& builder);
        
        DictItemContext StartDict();
        
        ArrayItemContext StartArray();
        
        Builder& EndDict();
        
        Builder& EndArray();
        
        KeyContext Key(std::string key);
        
    protected:
        Builder& builder_;
    };
    
    class DictItemContext : public Context {
    public:
        DictItemContext StartDict() = delete;
        
        ArrayItemContext StartArray() = delete;
        
        Builder& EndArray() = delete;
    };
    
    class ArrayItemContext : public Context {
    public:
        ArrayItemContext Value(Node::Value value);
        
        Builder& EndDict() = delete;
        
        KeyContext Key(std::string key) = delete;
    };
    
    class KeyContext : public Context {
    public:
        DictItemContext Value(Node::Value value);
        
        Builder& EndDict() = delete;
        
        Builder& EndArray() = delete;
        
        KeyContext Key(std::string key) = delete;
    };
    
    KeyContext Key(std::string key);
    
    Builder& Value(Node::Value value);
    
    DictItemContext StartDict();
    
    ArrayItemContext StartArray();
    
    Builder& EndDict();
    
    Builder& EndArray();
    
    Node Build();

private:
    bool IsEmpty();
    
    Node* GetLastNode();
    
    Node* Insert(Node node);
    
    Node root_;
    std::vector<Node*> nodes_stack_;
};
} // namespace json