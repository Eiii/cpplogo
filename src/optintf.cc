#include "cpplogo/optintf.h"
#include "cpplogo/logging.h"

namespace cpplogo {

/***********************************************************
* OptIntf constructor
***********************************************************/
OptIntf::OptIntf(const Options& opt) :
  fn_(opt.fn), dim_(opt.dim), num_children_(opt.num_children), 
  max_observations_(opt.max_observations), num_observations_(0),
  space_()
{
  // Create top-level node and observe its value
  vectord edges(dim_), sizes(dim_);
  for (int i = 0; i < dim_; i++) {
    edges[i] = 0.0;
    sizes[i] = 1.0;
  }
  space_.emplace_back();
  space_[0].emplace_back(edges, sizes, 0);
  ObserveNode(&space_[0].back());
} /* OptIntf() */

/***********************************************************
* Optimize
* Keep executing optimization steps until finished.
***********************************************************/
void OptIntf::Optimize() 
{
  while (!IsFinished()) {
    Step();
  }
} /* Optimize() */

/***********************************************************
* Step
* Execute a single step of the optimization procudure.
***********************************************************/
void OptIntf::Step() {
  BeginStep();

  StepExpandNodes();

  EndStep();
} /* Step() */

/***********************************************************
* IsFinished
* Returns true if the optiization is finished.
***********************************************************/
bool OptIntf::IsFinished() const {
  return num_observations_ >= max_observations_;
}

/***********************************************************
* BestNode
* Returns a pointer to the best node in the entire space.
***********************************************************/
const Node* OptIntf::BestNode() const
{
  const Node* best_node = nullptr;
  for (const auto& nodes : space_) {
    for (const auto& n : nodes) {
      if (best_node == nullptr || n.value() > best_node->value()) {
        best_node = &n;
      }
    }
  }
  return best_node;
} /* BestNode() */

/***********************************************************
* RemoveNode
* Remove the specified node from the node space.
***********************************************************/
void OptIntf::RemoveNode(Node* node)
{
  // Get a reference to the depth the node is at
  auto& depth_list = space_[node->depth()];

  // Use fancy STL stuff to find the iterator to the node in its depth
  auto pred = [node](const Node& n) { return &n == node; };
  auto it = std::find_if(depth_list.begin(), depth_list.end(), pred);
  assert(it != depth_list.end());

  depth_list.erase(it);
} /* RemoveNode() */

/***********************************************************
* ExpandNode
* Expand the specified node.
* Returns a list of the resultant child nodes.
***********************************************************/
std::vector<Node> OptIntf::ExpandNode(Node* node)
{
  LOG(trace) << "Expanding " << *node;
  std::vector<Node> children;

  //Choose which dimension to split along
  size_t split_dim = ChooseSplitDimension(node);

  //Calculate new values for child nodes
  vectord edges = node->edges();
  vectord sizes = node->sizes();
  int depth = node->depth()+1;
  double new_size = sizes[split_dim]/num_children_;
  sizes[split_dim] = new_size;

  //Create child nodes
  for (int i = 0; i < num_children_; i++) {
    children.emplace_back(edges, sizes, depth);
    edges[split_dim] += new_size;

    // ...the center child node can just take its value from its parent!
    assert(num_children_ % 2 == 1);
    if (i == num_children_/2 && !node->is_fake_value()) {
      children.back().SetValue(node->value());
    }
  }
  return children;
} /* ExpandNode() */

/***********************************************************
* ObserveNode
* Observe a value for the specified node.
***********************************************************/
bool OptIntf::ObserveNode(Node* node)
{
  // If the node already has a value, then we don't have to 
  // do anything.
  if (node->has_value() && !node->is_fake_value()) {
    LOG(trace) << "Already had: " << *node;
    return false;
  }

  // Observe the value and set it
  double value = fn_(node->Center());
  node->SetValue(value);
  num_observations_++;

  LOG(trace) << "Observed: " << *node;
  return true;
} /* ObserveNode() */

/***********************************************************
* ObserveNodes
* Observe values for each node in a list of nodes.
***********************************************************/
void OptIntf::ObserveNodes(std::vector<Node>* nodes)
{
  for (auto& node : *nodes) {
    ObserveNode(&node);
  }
} /* ObserveNodes() */

/***********************************************************
* AddNode
* Add a node to the space.
***********************************************************/
void OptIntf::AddNode(const Node& node)
{
  size_t depth = static_cast<size_t>(node.depth());
  if (space_.size() <= depth) {
    space_.resize(depth+1);
  }
  auto& level = space_.at(depth);
  level.push_back(node);
} /* AddNode() */

/***********************************************************
* AddNode
* Add a node to the space.
***********************************************************/
void OptIntf::AddNodes(const std::vector<Node>& nodes)
{
  for (const auto& node : nodes) {
    AddNode(node);
  }
} /* AddNodes() */

}
