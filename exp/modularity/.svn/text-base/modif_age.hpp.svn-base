#ifndef MODIFIER_AGE_HPP
#define MODIFIER_AGE_HPP

#include <sferes/stc.hpp>

#ifndef TRACK_FIT
#warning TRACK_FIT should be defined to use the age modifier
#endif

namespace sferes
{
  namespace modif
  {
    SFERES_CLASS(Age)
    {
      public:
        template<typename Ea>
        void apply(Ea& ea)
        {
           // add a new random individual (which can replace any individual)
          typedef boost::shared_ptr<typename Ea::phen_t> indiv_t;
          size_t k = misc::rand(ea.pop().size() / 2) + ea.pop().size() / 2;
          ea.pop()[k] = indiv_t(new typename Ea::indiv_t::element_type());
          ea.pop()[k]->random();
          ea.pop()[k]->develop();
          ea.pop()[k]->fit().eval(*ea.pop()[k]);
          ea.pop()[k]->fit().set_age(0);

           // minimize age
          for (size_t i = 0; i < ea.pop().size(); ++i)
          {
            int l = ea.pop()[i]->fit().objs().size() - 1;
            assert(l > 0);
            int age = ea.pop()[i]->fit().age();
            ea.pop()[k]->fit().set_age(age + 1);
            ea.pop()[i]->fit().set_obj(l, -ea.pop()[i]->fit().age());
          }
        }
    };
  }
}

#endif
