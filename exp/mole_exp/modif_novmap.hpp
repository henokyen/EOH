#include <iostream>
#include <ssrc/spatial/kd_tree.h>
#include <boost/multi_array.hpp>
#include <boost/dynamic_bitset.hpp>
#include <unordered_set>


#define MOLE_FITNESS(Name) SFERES_FITNESS(Name, sferes::fit::FitMole)

namespace sferes
{
  namespace fit
  {
    SFERES_FITNESS(FitMole, sferes::fit::Fitness)
    {
    public:
      FitMole() : _desc(2) { this->_objs.resize(3); }
      const std::vector<float>& desc() const { return _desc; }
      void set_desc(float x1, float x2)
      {
	assert(x1 >= 0);
	assert(x2 >= 0);
	assert(x1 < 1);
	assert(x2 < 1);
	assert(_desc.size() >= 2);
	_desc[0] = x1; 
	_desc[1] = x2;
      }
    protected:
      std::vector<float> _desc;
    };
  }

  namespace modif
  {
    template<typename Phen, typename Params>
    class Novmap
    {
    public:
      typedef std::array<float, 2> point_t;
      typedef boost::shared_ptr<Phen> phen_t;
      typedef ssrc::spatial::kd_tree<point_t, phen_t> tree_t;
      typedef typename tree_t::knn_iterator knn_iterator_t;
      typedef boost::multi_array<phen_t, 2> array_t;
      typedef boost::multi_array<int, 2> hits_t;
      typedef std::set<boost::dynamic_bitset<> > sbset_t; 

      static const size_t nb_neigh = 8;
      static const size_t res_x = 200;
      static const size_t res_y = 200;

      template<typename I>
      point_t get_point(const I& indiv)
      {
	point_t p;
	p[0] = std::min(1.0f, indiv->gen().data(0));
	p[1] = std::min(1.0f, indiv->gen().data(1));

	return p;
      }
      Novmap() :
	_array(boost::extents[res_x][res_y]),
	_hits(boost::extents[res_x][res_y])
      {
	for (size_t i = 0; i < res_x; ++i)
	  for (size_t j = 0; j < res_y; ++j)
	    _hits[i][j] = 0;
      }

      template<typename Ea> void apply(Ea& ea)
      {

	tree_t apop = _archive;
	//	for (size_t i = 0; i < ea.pop().size(); ++i)
	//	  apop.insert(get_point(ea.pop()[i]), ea.pop()[i]);
;
	for (size_t i = 0; i < ea.pop().size(); ++i)
          {
	    // compute the novelty score
            point_t p = get_point(ea.pop()[i]);

            std::pair<knn_iterator_t, knn_iterator_t>
	      range = apop.find_nearest_neighbors(p, nb_neigh, true);
            float nov = 0;
            float fit = 0;
	    float local_div = 0;
	    float nb_hits = 0;
            for (knn_iterator_t it = range.first, end = range.second;
                 it != end; ++it)
	      {
		float x = it->first[0] - p[0];
		float y = it->first[1] - p[1];
		size_t xr = round(p[0] * res_x);
		size_t yr = round(p[1] * res_y);
		nb_hits += _hits[xr][yr];
		nov += sqrtf(x * x + y * y);
	      }
	    nb_hits /= nb_neigh;
            nov /= nb_neigh;
	    local_div /= nb_neigh;
	    fit = ea.pop()[i]->fit().value();
	    float div = 0;
	    for (size_t j = 0; j < ea.pop().size(); ++j)
	      {
		point_t p2 = get_point(ea.pop()[j]);
		float x = p2[0] - p[0];
		float y = p2[1] - p[1];	
		div += sqrtf(x * x + y * y);
	      }
	    div /= ea.pop().size();
            size_t nb_objs = ea.pop()[i]->fit().objs().size();
            ea.pop()[i]->fit().set_obj(0, fit);
            ea.pop()[i]->fit().set_obj(1, nov/nb_hits);
	    ea.pop()[i]->fit().set_obj(2, div);
	    // add the point if the position is free or  if it's better
            size_t x = round(p[0] * res_x);
            size_t y = round(p[1] * res_y);
            assert(x < res_x);
            assert(y < res_y);
            x = std::min(x, res_x - 1);
            y = std::min(y, res_y - 1);
	    _hits[x][y]++;
            if (!_array[x][y] 
		|| ea.pop()[i]->fit().value() > _array[x][y]->fit().value())
	      {
		_array[x][y] = ea.pop()[i];
		_archive.insert(p, ea.pop()[i]);
	      }
          }
	if (ea.gen() % 10 == 0)
          {
            std::cout << "writing..." << ea.gen() << std::endl;
            std::ofstream ofs("/project/evolutionaryRobotics/hmengist/mole_exp/archive.dat");
            for (size_t i = 0; i < res_x; ++i)
              for (size_t j = 0; j < res_y; ++j)
                if (_array[i][j])
                  ofs << i / (float) res_x
                      << " " << j / (float) res_y
                      << " " << _array[i][j]->fit().value() << std::endl;
            std::ofstream ofs2("pop.dat");
	    for (size_t i = 0; i < ea.pop().size(); ++i)
	      for (size_t j = 0; 
		     j < ea.pop()[i]->fit().objs().size(); 
		   ++j)
		{
		  point_t p = get_point(ea.pop()[i]);
		  ofs2 << p[0]<<" "<<p[1] << std::endl;
		}	    
          }
      }

      const array_t& archive() const { return _array; }
    protected:
      tree_t _archive;
      array_t _array;
      hits_t _hits;
    };
  }
}
