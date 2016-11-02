/*********************************************************************
* INCLUDES
*********************************************************************/
#include <map>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <random>

/*********************************************************************
* NAMESPACE
*********************************************************************/
namespace cpplogo {

/*********************************************************************
* TYPES
*********************************************************************/
using matrixd = boost::numeric::ublas::matrix<double>;
using vectord = boost::numeric::ublas::vector<double>;

using RandomEngine = std::mt19937;

using ObjectiveFn = std::function<double(const vectord&)>;
  
/*********************************************************************
* END NAMESPACE
*********************************************************************/
}
