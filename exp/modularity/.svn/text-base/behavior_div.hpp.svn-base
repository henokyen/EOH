#ifndef MODIFIER_BEHAVIOR_DIV_HPP
#define MODIFIER_BEHAVIOR_DIV_HPP

#include <sferes/stc.hpp>

namespace sferes
{

  template <int nb_bits>
  float hamming_dist(const ::std::vector< std::bitset<nb_bits> > &d1,
		     const ::std::vector< std::bitset<nb_bits> > &d2)
  {
    float ham = 0;
    assert(d1.size() == d2.size());
    for (size_t i = 0; i < d1.size(); i++)
      ham += (float) (d1[i] ^ d2[i]).count() / (float) d1[i].size();
    return ham / (float)d1.size();
  }

  namespace modif
  {
    namespace modifier_div
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
	      //d += _pop[i]->fit().obj(l);
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
#ifdef RMDUP
	std::set<std::vector<float> > fits;
	typedef boost::shared_ptr<typename Ea::phen_t> indiv_t;
	typedef std::set<indiv_t> ind_set_t;
	typedef std::vector<indiv_t> pop_t;
	ind_set_t to_remove;
	for (size_t i = 0; i < ea.pop().size(); ++i)
	  {
	    if (fits.find(ea.pop()[i]->fit().objs()) != fits.end())
	      to_remove.insert(ea.pop()[i]);
	    else
	      fits.insert(ea.pop()[i]->fit().objs());
	  }
	typename pop_t::iterator it =
	  std::remove_if(ea.pop().begin(), ea.pop().end(),
			 modifier_div::is_in<ind_set_t>(to_remove));
	std::cout<<"removing:"<<to_remove.size()<<std::endl;
	for (; it != ea.pop().end(); ++it)
	  {
	    (*it) = indiv_t(new typename Ea::indiv_t::element_type());
	    (*it)->random();
	    (*it)->develop();
	    (*it)->fit().eval(*(*it));
	  }
	std::cout<<"done"<<std::endl;
#endif
      }
    };
  }
}

#endif
