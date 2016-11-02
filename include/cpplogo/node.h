/***********************************************************
* node.h
* Class that represents a single node in the function space.
***********************************************************/
#pragma once
#include "cpplogo/types.h"

namespace cpplogo {

class Node {
  public:
    Node(vectord edges, vectord sizes, int depth);
    Node(const Node& rhs) = default;
    Node& operator=(const Node& rhs) = default;
    Node(Node&& rhs) = default;
    Node& operator=(Node&& rhs) = default;
    virtual ~Node() = default;

  public:
    void SetValue(double value);
    void SetFakeValue(double value);
    vectord Center() const;

  public:
    const vectord& edges() const { return edges_; }
    const vectord& sizes() const { return sizes_; }
    int depth() const { return depth_; }
    double value() const { return value_; }
    bool has_value() const { return has_value_; }
    bool is_fake_value() const { return is_fake_value_; }

  protected:
    vectord edges_;      // Edge of the node for each dimension
    vectord sizes_;      // Size of the node in each dimension
    int depth_;          // Depth of the node in the node space
    double value_;       // Value of the function at the node's center
    bool has_value_;     // Does the node have a value?
    bool is_fake_value_; // Is the node's vaule fake? (for BaMSOO)
};

std::ostream& operator<<(std::ostream& os, const Node& n);

}
