#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <cassert>
using namespace std;

template <typename T> class Tree;

template <typename T>
/// Node of a tree containing an integer at each node
class Tree {

    /// Node information
    T data;
    /// Sequence of children (empty if none)
    vector<Tree*> children;

public:
    /// Create a node with given information
    Tree(T d){data = d;};

    /// Destruct a node and all its descendents
    ~Tree(){
        for(int i=0; i<nbChildren(); i++){
            delete children[i];
        }
    }

    /// Return information of this node
    T getData() const{return data;};

    /// Return a reference of the node data
    T& getDataRef() {return data;};

    /// Set information of this node
    void setData(T d){data = d;};

    /// Return the number of children of this node
    int nbChildren() const{return children.size();};

    /// Return the child at pos, if any (left-most child at position 0)
    Tree* getChild(int pos) const{
        assert(pos>=0 and pos<nbChildren());
        return children[pos];
    }

    Tree* getLastChild() const{
        return children[nbChildren()-1];
    }

    /// Replace the exisiting child at pos (left-most child at position 0)
    void setChild(int pos, Tree* newChild){
        // Check for  out-of-range errors
        assert(pos>=0 and pos<nbChildren());
        delete children[pos];
        children[pos] = newChild;
    }

    /// Add newChild as supplementary right-most child of this node
    void addAsLastChild(Tree* newChild){children.push_back(newChild);}

    /// Remove right-most child of this node
    void removeLastChild(){
        assert(!isLeaf());
        delete children.back();
        children.pop_back();
    };


    /// Check if the Tree is a leaf (i.e. has no child)
    bool isLeaf(){return nbChildren()==0;};

    /// Select the children which has the biggest value in data
    Tree* max(){
        if(isLeaf()) return nullptr;
        Tree* max_child = children[0];
        for(auto it=children.begin(); it!=children.end();it++){
            if(max_child->data < (*it)->data){
                max_child = *it;
            }
        }
        return max_child;
    }
};
