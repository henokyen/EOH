#ifndef MODIFIER_KMEDOIDS_HPP
#define MODIFIER_KMEDOIDS_HPP

#include <sferes/stc.hpp>

#include "kmedoids.hpp"
namespace sferes
{
  namespace modif
  {    
    namespace kmedoids_details
    {
     
      struct dist_phen
      {
	template<typename P>
	float operator()(const P& p1, const P& p2) const
	{
	  return p1->fit().dist(*p2);
	}
      };
    }
    SFERES_CLASS(Kmedoids)
    {
    public:
      template<typename Ea>
	void apply(Ea& ea)
      {
	kmedoids::clusters_t clusters = 
	  kmedoids::kmedoids(ea.pop(), kmedoids_details::dist_phen(), 
			     ea.pop().size() / 4);

	int l =  ea.pop()[0]->fit().objs().size() - 1;
	assert(l > 0);
	// div = 1 for medoids, 0 otherwise
	for (size_t i = 0; i < ea.pop().size(); ++i)
	  ea.pop()[i]->fit().set_obj(l, 0);

	for (size_t i = 0; i < clusters.first.size(); ++i)
	  ea.pop()[clusters.first[i]]->fit().set_obj(l, 1);
       	  
      }
    };
  }
}

#endif
