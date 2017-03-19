#include "cpplogo/randomimgpo.h"
#include "cpplogo/logging.h"

namespace cpplogo {

namespace { 
//Helper function to test if two nodes are the same
//size and in the same place
bool node_pos_eq(const Node& n1, const Node& n2) {
  size_t dim = n1.sizes().size();
  if (n1.depth() != n2.depth()) return false;
  for (size_t i = 0; i < dim; i++) {
    if (n1.edges()[i] != n2.edges()[i]) return false;
    if (n1.sizes()[i] != n2.sizes()[i]) return false;
  }
  return true;
}
}

/***********************************************************
* ChooseSplitDimension
* This is a questionable hack-- Since ChooseSplitDimension
* is used both when constructing IMGPO's secondary tree
* and when expanding real nodes, if we randomly choose
* split dimensions the secondary tree can end up mismatched
* with the nodes that would actually be created if the
* node being split was expanded to that depth.
* So, whenever we make a random splitting choice we just
* remember the position of the node we split and which 
* dimension we split along.
* Then, any time we need to split a node we check our
* past splits first to ensure we make the same random
* decision we did previously.
* In practice this didn't seem to significantly affect the
* algorithm's performance, but I'm keeping it in anyway 
* because the alternative feels bad.
***********************************************************/
size_t RandomIMGPO::ChooseSplitDimension(const Node* node) const
{
  size_t dim;
  bool match_found = false;
  for (const auto& pair : pre_splits_) {
    Node n = std::get<0>(pair);
    size_t d = std::get<1>(pair);
    if (node_pos_eq(n, *node)) {
      dim = d; 
      match_found = true;
      break;
    }
  }

  if (!match_found) {
    dim = RandomSOO::ChooseSplitDimension(node);
    pre_splits_.emplace(pre_splits_.begin(), *node, dim);
  }
  return dim;
} /* ChooseSplitDimension() */

}
