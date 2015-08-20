#ifndef MODIFIER_BEHAVIOR_NOV_HPP
#define MODIFIER_BEHAVIOR_NOV_HPP

#include <sferes/stc.hpp>
#include <boost/timer.hpp>
namespace sferes
{
  namespace modif
  {
    namespace novelty
    {
      template<typename T>
      float dist(const T& v1, const T& v2)
      {
	return (v1 ^ v2).count() / (float) v1.size();   
      }

      template<typename T>
      struct compare_dist_f
      {
	compare_dist_f(const T& v) : _v(v) {}
	const T _v;
     
	bool operator()(const T& v1, const T& v2) const
	{
	  assert(v1.size() == _v.size());
	  assert(v2.size() == _v.size());
	  return dist(v1, _v) < dist(v2, _v);
	}
      };
    }


    SFERES_CLASS(BehaviorNov)
    {
    public:
      typedef typename std::bitset<Params::fitness::b_size> behavior_t;
      BehaviorNov() : _rho_min(10.0f), _not_added(0) {}
      template<typename Ea>
	void apply(Ea& ea)
      {
	static const size_t k = 15;
     
	float max_n = 0;
	int added = 0;
	std::vector<behavior_t> archive = _archive;
#ifndef NOVELTY_NO_DIV 
	for (size_t i = 0; i < ea.pop().size(); ++i)
	  archive.push_back(ea.pop()[i]->fit().behavior());
#endif
	for (size_t i = 0; i < ea.pop().size(); ++i)
	  {
	    const behavior_t& behavior = ea.pop()[i]->fit().behavior();
	    tbb::parallel_sort(archive.begin(),
			       archive.end(),
			       novelty::compare_dist_f<behavior_t>(behavior));
	    float n = 0;
	    if (archive.size() > k)
	      for (size_t j = 0; j < k; ++j)
		n += novelty::dist(archive[j], behavior);
	    else
	      n += 1;
	    n /= k;
	    max_n = std::max(n, max_n);
	    size_t ns = ea.pop()[i]->fit().objs().size() - 1;
	    ea.pop()[i]->fit().set_obj(ns, n);
	    if (_archive.size() < k || n > _rho_min || misc::rand<float>() < 0.01 / 100.0f)
	      {
		_archive.push_back(behavior);
		_not_added = 0;
		++added;
	      }
	    else
	      ++_not_added;
	  }
	if (_not_added > 2500)
	  _rho_min *= 0.95;
	if (_archive.size() > k  && added > 4)
	  _rho_min *= 1.05f;
      }
      const std::vector<behavior_t> & archive() const { return _archive; }
    protected:
      std::vector<behavior_t> _archive;
      float _rho_min;
      size_t _not_added;
   
    };

  }
}
#endif
