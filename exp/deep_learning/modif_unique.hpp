#ifndef MODIFIER_UNIQUE_HPP
#define MODIFIER_UNIQUE_HPP

#include <sferes/stc.hpp>

namespace sferes
{
  namespace modif
  {
    namespace div
    {
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
	      float d = 0.0f;
	      for (size_t j = 0; j < _pop.size(); ++j)
		d += _pop[i]->fit().dist(*_pop[j]);
	      d /= _pop.size();
	      int l =  _pop[i]->fit().objs().size() - 1;
	      assert(l > 0);
	      _pop[i]->fit().set_obj(l, d);
	    }
	}
      };
    }

    // ADD the mean distance to the population to the last objective (it
    // DOESN'T add the objective automatically)
    // you HAVE to initialize this value to a "good" one (depending on
    // your constraints scheme; for instance, 0)
    // you phenotype/individual class must have a float dist(const
    // Phen& o) method (the dist method must be thread-safe)
    SFERES_CLASS(Unique)
    {
    public:
      template<typename Ea>
	void apply(Ea& ea)
      {
	// parallel compute
	parallel::init();
	parallel::p_for(parallel::range_t(0, ea.pop().size()),
			div::_parallel_behavior_div<typename Ea::phen_t>(ea.pop()));
      
	// now the uniqueness score
	std::list<int> 
	  pop_ids(boost::counting_iterator<int>(0),
		  boost::counting_iterator<int>(ea.pop().size()));
	pop_ids.reverse();
	int l =  ea.pop()[0]->fit().objs().size() - 1;
	while (!pop_ids.empty())
	  {
	    std::vector<int> cluster;
	    std::list<int>::iterator it = pop_ids.begin();
	    int id = pop_ids.front();
	    while (it != pop_ids.end())
	      {
		if (ea.pop()[id]->fit().behavior() == ea.pop()[*it]->fit().behavior())
		  {
		    cluster.push_back(*it);
		    it = pop_ids.erase(it);
		  }
		else
		  ++it;
	      }
	    for (size_t i = 0; i < cluster.size(); ++i)
	      {
		float score = (int)ea.pop().size() - i 
		  + ea.pop()[cluster[i]]->fit().obj(l);
		ea.pop()[cluster[i]]->fit().set_obj(l, score);
	      }	      
	  }
	std::ofstream ofs_pop("pop.dat");
	for(size_t i = 0; i < ea.pop().size(); ++i)
	  ofs_pop << ea.pop()[i]->fit().obj(0) << " "
		  << ea.pop()[i]->fit().obj(1) << std::endl;
      }
    };
  }
}

#endif



	    
