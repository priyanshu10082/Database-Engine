#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

template <typename K, typename V>
class BPlusTree {
public:
    class Node {
    public:
        bool isLeaf;
        std::vector<K> keys;
        Node(bool leaf) : isLeaf(leaf) {}
        virtual ~Node() = default;
    };

    class InternalNode : public Node {
    public:
        std::vector<Node*> children;
        InternalNode() : Node(false) {}
    };

    class LeafNode : public Node {
    public:
        std::vector<V> values;
        LeafNode* next;
        LeafNode() : Node(true), next(nullptr) {}
    };

private:
    Node* root;
    int order;

    void insertInternal(K key, Node* cursor, Node* child) {
        InternalNode* internalNode = static_cast<InternalNode*>(cursor);
        int insertPos = std::upper_bound(internalNode->keys.begin(), internalNode->keys.end(), key) - internalNode->keys.begin();
        internalNode->keys.insert(internalNode->keys.begin() + insertPos, key);
        internalNode->children.insert(internalNode->children.begin() + insertPos + 1, child);
        
        if (internalNode->keys.size() == order) {
            InternalNode* newInternal = new InternalNode();
            int splitIndex = order / 2;
            K upKey = internalNode->keys[splitIndex];
            
            for (size_t i = splitIndex + 1; i < internalNode->keys.size(); ++i) {
                newInternal->keys.push_back(internalNode->keys[i]);
            }
            for (size_t i = splitIndex + 1; i < internalNode->children.size(); ++i) {
                newInternal->children.push_back(internalNode->children[i]);
            }
            
            internalNode->keys.resize(splitIndex);
            internalNode->children.resize(splitIndex + 1);
            
            if (cursor == root) {
                InternalNode* newRoot = new InternalNode();
                newRoot->keys.push_back(upKey);
                newRoot->children.push_back(internalNode);
                newRoot->children.push_back(newInternal);
                root = newRoot;
            } else {
                insertInternal(upKey, findParent(root, cursor), newInternal);
            }
        }
    }

    Node* findParent(Node* cursor, Node* child) {
        if (cursor == nullptr || cursor->isLeaf) return nullptr;
        InternalNode* internalNode = static_cast<InternalNode*>(cursor);
        if (internalNode->children[0]->isLeaf) {
            for (size_t i = 0; i < internalNode->children.size(); ++i) {
                if (internalNode->children[i] == child) return cursor;
            }
            return nullptr;
        } else {
            for (size_t i = 0; i < internalNode->children.size(); ++i) {
                if (internalNode->children[i] == child) return cursor;
                Node* parent = findParent(internalNode->children[i], child);
                if (parent != nullptr) return parent;
            }
            return nullptr;
        }
    }
    
    void cleanUp(Node* cursor) {
        if (cursor != nullptr) {
            if (!cursor->isLeaf) {
                InternalNode* internalNode = static_cast<InternalNode*>(cursor);
                for (Node* child : internalNode->children) {
                    cleanUp(child);
                }
            }
            delete cursor;
        }
    }

public:
    BPlusTree(int _order) : root(nullptr), order(_order) {}
    ~BPlusTree() { if (root) cleanUp(root); }

    void insert(K key, const V& value) {
        if (root == nullptr) {
            root = new LeafNode();
            LeafNode* leafNode = static_cast<LeafNode*>(root);
            leafNode->keys.push_back(key);
            leafNode->values.push_back(value);
        } else {
            Node* cursor = root;
            Node* parent = nullptr;
            while (!cursor->isLeaf) {
                parent = cursor;
                InternalNode* internalNode = static_cast<InternalNode*>(cursor);
                int idx = std::upper_bound(internalNode->keys.begin(), internalNode->keys.end(), key) - internalNode->keys.begin();
                cursor = internalNode->children[idx];
            }
            LeafNode* leafNode = static_cast<LeafNode*>(cursor);
            int insertPos = std::upper_bound(leafNode->keys.begin(), leafNode->keys.end(), key) - leafNode->keys.begin();
            leafNode->keys.insert(leafNode->keys.begin() + insertPos, key);
            leafNode->values.insert(leafNode->values.begin() + insertPos, value);
            
            if (leafNode->keys.size() == order) {
                LeafNode* newLeaf = new LeafNode();
                int splitIndex = (order + 1) / 2;
                
                for (size_t i = splitIndex; i < leafNode->keys.size(); ++i) {
                    newLeaf->keys.push_back(leafNode->keys[i]);
                    newLeaf->values.push_back(leafNode->values[i]);
                }
                
                leafNode->keys.resize(splitIndex);
                leafNode->values.resize(splitIndex);
                
                newLeaf->next = leafNode->next;
                leafNode->next = newLeaf;
                
                if (cursor == root) {
                    InternalNode* newRoot = new InternalNode();
                    newRoot->keys.push_back(newLeaf->keys[0]);
                    newRoot->children.push_back(leafNode);
                    newRoot->children.push_back(newLeaf);
                    root = newRoot;
                } else {
                    insertInternal(newLeaf->keys[0], parent, newLeaf);
                }
            }
        }
    }

    bool search(K key, V& outValue) {
        if (root == nullptr) return false;
        Node* cursor = root;
        while (!cursor->isLeaf) {
            InternalNode* internalNode = static_cast<InternalNode*>(cursor);
            int idx = std::upper_bound(internalNode->keys.begin(), internalNode->keys.end(), key) - internalNode->keys.begin();
            cursor = internalNode->children[idx];
        }
        LeafNode* leafNode = static_cast<LeafNode*>(cursor);
        for (size_t i = 0; i < leafNode->keys.size(); ++i) {
            if (leafNode->keys[i] == key) {
                outValue = leafNode->values[i];
                return true;
            }
        }
        return false;
    }
};
