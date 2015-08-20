#ifndef MODIFIER_BEHAVIOR_DIV_HPP
#define MODIFIER_BEHAVIOR_DIV_HPP

#include <sferes/stc.hpp>

namespace sferes
{
  namespace modif
  {
    namespace modifier_div
    {
      struct Dist
      {
        Dist(float d, int i) : dist(d), index(i) {}
        float dist;
        int index;
        bool operator < (const Dist &disto) const
        {
          return dist < disto.dist;
        };
      };


      template<typename Phen>
       struct _parallel_behavior_div
      {
	typedef std::vector<boost::shared_ptr<Phen> > pop_t;
	pop_t _pop;

	_parallel_behavior_div(pop_t& pop) : _pop(pop) {}
	_parallel_behavior_div(const _parallel_behavior_div& ev) : _pop(ev._pop) {}
	void operator() (const parallel::range_t& r) const
	{
	  for (size_t i = r.begin(); i != r.end(); ++i)
	    {
	      
	      std::vector<Dist> v_dist;
	      for (size_t j = 0; j < _pop.size(); ++j)
		{
		  float hd = _pop[i]->fit().dist(*_pop[j]);
		  v_dist.push_back(Dist(hd, j));
		}
	      std::sort(v_dist.begin(), v_dist.end());

	      float d = 0.0f;
	      for (size_t j = 0; j < 8; ++j)
		d += v_dist[j].dist;
	      d /= 8.0;
	      std::cout<<"DIV:"<<d<<std::endl;
	      int l =  _pop[i]->fit().objs().size() - 1;
	      assert(l > 0);
	      _pop[i]->fit().set_obj(l, d);
	    }
	}
      };
      template<typename T>
      struct is_in
      {
	is_in(const T& s) : _to_remove(s) {}
	bool operator()(const typename T::value_type& x)
	{
	  return _to_remove.find(x) == _to_remove.end();
	}
	const T& _to_remove;
      };
    }

    // ADD the mean distance to the population to the last objective (it
    // DOESN'T add the objective automatically)
    // you HAVE to initialize this value to a "good" one (depending on
    // your constraints scheme; for instance, 0)
    // you phenotype/individual class must have a float dist(const
    // Phen& o) method (the dist method must be thread-safe)
    SFERES_CLASS(BehaviorDiv)
    {
    public:
      template<typename Ea>
	void apply(Ea& ea)
      {
	// parallel compute
	parallel::init();
	parallel::p_for(parallel::range_t(0, ea.pop().size()),
			modifier_div::_parallel_behavior_div<typename Ea::phen_t>(ea.pop()));
      }
    };
  }
}

#endif
