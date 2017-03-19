#include "cpplogo/soo.h"

#include "cpplogo/logging.h"
#include <algorithm>

using std::vector;

namespace cpplogo {

/***********************************************************
* SOO constructor
***********************************************************/
SOO::SOO(const Options& opt) :
  OptIntf(opt),
  num_children_(opt.num_children),
  vmax_(),
  num_expansions_(1),
  num_node_evals_(1),
  space_(),
  step_observed_nodes_()
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
} /* SOO() */

/***********************************************************
* BeginStep
* Called at the beginning of each optimization step.
***********************************************************/
void SOO::BeginStep() 
{
  LOG(trace) << "Beginning step";
  vmax_ = -std::numeric_limits<double>::infinity();
  step_observed_nodes_.clear();
} /* BeginStep() */

/***********************************************************
* EndStep
* Called at the end of each optimization step.
***********************************************************/
void SOO::EndStep()
{
  LOG(trace) << "Ending step";
} /* EndStep() */

/***********************************************************
* CalculateMaxDepth
***********************************************************/
size_t SOO::CalculateMaxDepth() const
{
  size_t max_depth = std::sqrt(num_expansions_);
  max_depth = std::min(max_depth, space_.size());
  LOG(trace) << "Max depth = " << max_depth;
  return max_depth;
} /* CalculateMaxDepth() */

/***********************************************************
* StepExpandNodes
***********************************************************/
void SOO::StepExpandNodes()
{
  // For each 'depth level', expand the best node at that
  // depth.
  size_t dmax = CalculateMaxDepth();
  for (size_t depth = 0; depth <= dmax; depth++) {
    ExpandBestAtDepth(depth);
  }
} /* StepExpandNodes() */

/***********************************************************
* ExpandBestAtDepth
* Expand the node with the best value at the specified
* depth.
***********************************************************/
void SOO::ExpandBestAtDepth(size_t depth)
{
  LOG(trace) << "Expanding depth " << depth;
  // Find the best node
  Node* best_node = BestNodeAtDepth(depth);
  if (!best_node) {
    LOG(trace) << "Depth empty";
    return;
  }
  LOG(trace) << "Best node = " << *best_node;

  // Expand the node and evaluate its children if the best node is good enough
  if (NodeShouldBeExpanded(best_node)) {
    // Get the resultant children from the expansion
    vector<Node> children = ExpandNode(best_node);
    num_expansions_ += 1; 
    // Make observations for each un-observed node among the children
    ObserveNodes(&children);

    // Make sure we have a place to put the next level of nodes
    int next_depth = best_node->depth()+1;
    if (space_.size() <= static_cast<size_t>(next_depth)) {
      space_.resize(next_depth+1);
    }

    // Put the expanded nodes into the appropriate level in the space
    auto& next_level = space_[next_depth];
    next_level.insert(next_level.end(), children.begin(), children.end());

    // Delete the node that was expanded
    RemoveNode(best_node);
  }
  num_node_evals_ += 1;
} /* ExpandBestAtDepth() */

/***********************************************************
* NodeShouldBeExpanded
***********************************************************/
bool SOO::NodeShouldBeExpanded(Node* node)
{
  if (node->value() > vmax_) {
    vmax_ = node->value();
    LOG(trace) << "New vmax = " << vmax_;
    return true;
  } else {
    return false;
  }
} /* NodeShouldBeExpanded() */

/***********************************************************
* BestNodeAtDepth
* Find the node with the best value at the specified depth
* level.
* Returns a pointer to that node. Returns a null pointer
* if no nodes exist at the requested depth.
***********************************************************/
const Node* SOO::BestNodeAtDepth(size_t depth) const
{
  // If the depth doesn't exist, it can't have a best node
  if (depth >= space_.size()) return nullptr;
  const vector<Node>& nodes = space_.at(depth);
  const Node* best_node = nullptr;

  // Node comparison lambda function
  // See the max_element STL documentation
  auto node_comp = [](const Node& a, const Node& b) { 
    return a.value() < b.value(); 
  };
  auto best_it = std::max_element(nodes.begin(), nodes.end(), node_comp);

  // If we found a node, set the best_node pointer to the address of
  // the best node found
  // Otherwise, no nodes are at this depth and best_node will be null
  if (best_it != nodes.end()) {
    best_node = &(*best_it);
  }

  return best_node;
} /* BestNodeAtDepth() */

/***********************************************************
* BestNodeAtDepth
* Do some fancy casting to return a non-const pointer to the
* best node at the requested depth.
***********************************************************/
Node* SOO::BestNodeAtDepth(size_t depth) 
{
  const auto cthis = const_cast<const SOO*>(this);
  return const_cast<Node*>(cthis->BestNodeAtDepth(depth));
} /* BestNodeAtDepth() */

/***********************************************************
* BestNode
* Returns a pointer to the best node in the entire space.
* TODO: This should probably leverage BestNodeAtDepth
***********************************************************/
const Node* SOO::BestNode() const
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
void SOO::RemoveNode(Node* node)
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
vector<Node> SOO::ExpandNode(Node* node)
{
  LOG(trace) << "Expanding " << *node;
  vector<Node> children;

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
* ChooseSplitDimension
* Return which dimension the specified node should be split
* along.
* Effectively returns the first dimension with the maximum
* size.
***********************************************************/
size_t SOO::ChooseSplitDimension(const Node* node) const
{
  size_t max_dim = 0;
  for (size_t d = 0; d < static_cast<size_t>(dim_); d++) {
    if (node->sizes()[d] > node->sizes()[max_dim]) {
      max_dim = d;
    }
  }
  LOG(trace) << "Splitting along " << max_dim;
  return max_dim;
} /* ChooseSplitDimension() */

/***********************************************************
* ObserveNodes
* Observe values for each node in a list of nodes.
***********************************************************/
void SOO::ObserveNodes(vector<Node>* nodes)
{
  for (auto& node : *nodes) {
    ObserveNode(&node);
  }
} /* ObserveNodes() */

/***********************************************************
* ObserveNode
* Observe a value for the specified node.
***********************************************************/
bool SOO::ObserveNode(Node* node)
{
  // If the node already has a value, then we don't have to 
  // do anything.
  if (node->has_value() && !node->is_fake_value()) {
    LOG(trace) << "Already had: " << *node;
    return false;
  }

  // Observe the value and set it
  double value = fn_(node->Center());
  num_observations_++;
  node->SetValue(value);

  // We need to copy the node here, otherwise it might
  // get deleted in a future expansion during this step!
  step_observed_nodes_.push_back(*node);
  LOG(trace) << "Observed: " << *node;
  return true;
} /* ObserveNode() */

}
