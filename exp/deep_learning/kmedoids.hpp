#include <vector>
#include <algorithm>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/swap.hpp>
#include <boost/multi_array.hpp>

namespace kmedoids
{

  static const int nb_rand = 25;  
  static const size_t total_iterations = 200;
  
  // first : id of the clusters
  // second : cluster id for each data point
  typedef std::pair<std::vector<int>, std::vector<int> > clusters_t;
  typedef boost::multi_array<double, 2> dist_t;
  
  double _total_cost(const std::vector<int>& clusters,
		    const dist_t& dists)
  {
    double cost = 0;
    for (size_t i = 0; i < clusters.size(); ++i)
      cost += dists[clusters[i]][i];
    return cost;
  }

  // private function (core of the algorithm)
  double _kmedoids(const dist_t& dists, std::vector<int>& medoids, 
		   std::vector<int>& clusters)

  {
    // initialization
    std::vector<int> r(boost::counting_iterator<int>(0),
		       boost::counting_iterator<int>(clusters.size()));
    assert(r.size() == clusters.size());
    std::random_shuffle(r.begin(), r.end());
    for (size_t i = 0; i < medoids.size(); ++i)
      medoids[i] = r[i];
    std::vector<bool> is_medoids(clusters.size());
    bool done = false;
    for (size_t nb_it = 0; nb_it < total_iterations && !done; ++nb_it)
      {
	int changes = 0;
	for (size_t i = 0; i < clusters.size(); ++i)
	  {
	    std::vector<double> d(medoids.size());
	    for (size_t n = 0; n < medoids.size(); ++n)
	      d[n] = dists[i][medoids[n]];
	    int m = std::min_element(d.begin(), d.end()) - d.begin();
	    clusters[i] = medoids[m];
	  }
	// update cache
	std::fill(is_medoids.begin(), is_medoids.end(), false);
	for (size_t i = 0; i < medoids.size(); ++i)
	  is_medoids[medoids[i]] = true;
	// find new medoids
	std::pair<int, int> best_swap = std::make_pair(-1, -1);
	double current_cost = _total_cost(clusters, dists);	    
	for (size_t n = 0; n < medoids.size(); ++n)
	  {
	    for (size_t i = 0; i < clusters.size(); ++i)
	      if (!is_medoids[i])
		{
		  std::vector<int> n_clusters = clusters;
		  std::replace(n_clusters.begin(), n_clusters.end(), 
			       medoids[n], (int)i);
		  double cost = _total_cost(n_clusters, dists);
		  if (cost < current_cost)// epsilon : good improvement
		    {
		      best_swap = std::make_pair(n, i);
		      current_cost = cost;
		    }
		}
	  }
	if (best_swap.first != -1)
	  {
	    std::replace(clusters.begin(), clusters.end(), 
			 medoids[best_swap.first], best_swap.second);
	    medoids[best_swap.first] = best_swap.second;
	  }
	else
	  done = true;
      }
    return _total_cost(clusters, dists);
  }

  // main algorithm
  template<typename T, typename D>
  clusters_t kmedoids(const std::vector<T>& data, const D& dist_f, size_t k)
  {

    // distances
    dist_t dists(boost::extents[data.size()][data.size()]);
    for (size_t i = 0; i < data.size(); ++i)
      for (size_t j = 0; j <= i; ++j)
	{
	  dists[i][j] = dist_f(data[i], data[j]);
	  dists[j][i] = dists[i][j];
	}
    // run N times and take the best one
    double best_cost = 1e10;
    std::vector<int> best_medoids(k);
    std::vector<int> best_clusters(k);
    for (size_t i = 0; i < nb_rand; ++i)
      {
	std::vector<int> medoids(k);
	std::vector<int> clusters(data.size());
	double cost = _kmedoids(dists, medoids, clusters);
	if (cost < best_cost)
	  {
	    best_medoids.swap(medoids);
	    best_clusters.swap(clusters);
	    best_cost = cost;
	    std::cout<<"cost:"<<cost<<std::endl;
	  }
      }
    return std::make_pair(best_medoids, best_clusters);
  }


}
