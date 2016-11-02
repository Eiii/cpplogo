#include "cpplogo/node.h"

namespace cpplogo {

/***********************************************************
* Node constructor
***********************************************************/
Node::Node(vectord edges, vectord sizes, int depth) :
  edges_(edges), sizes_(sizes), depth_(depth), value_(), has_value_(false), 
  is_fake_value_(false)
{
} /* Node() */

/***********************************************************
* SetValue
* Sets the real value of the node.
***********************************************************/
void Node::SetValue(double value)
{
  value_ = value;
  has_value_ = true;
  is_fake_value_ = false;
} /* SetValue() */

/***********************************************************
* SetFakeValue
* Sets a fake value for the node.
***********************************************************/
void Node::SetFakeValue(double value)
{
  value_ = value;
  has_value_ = true;
  is_fake_value_ = true;
} /* SetFakeValue() */

/***********************************************************
* Center
* Returns the center point of the node (where observations
* should happen).
***********************************************************/
vectord Node::Center() const
{
  vectord center = edges_;
  assert(center.size() == sizes_.size());
  for (size_t i = 0; i < center.size(); i++) {
    center[i] += sizes_[i]/2.0; 
  }
  return center;
} /* Center() */

/***********************************************************
* Node pretty printer
***********************************************************/
std::ostream& operator<<(std::ostream& os, const Node& n)
{
  os << "Node(" << n.Center() << ", " << n.sizes();
  if (n.has_value()) {
    os << ", " << n.value();
  }
  os << ")";
  return os;
}

}
