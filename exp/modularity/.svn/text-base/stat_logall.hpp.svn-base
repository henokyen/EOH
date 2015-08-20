#ifndef STAT_LOGALL_HPP_
#define STAT_LOGALL_HPP_

#include <numeric>

namespace sferes
{
  namespace stat
  {
    SFERES_STAT(LogAll, Stat)
    {
      public:
        template<typename E>
        void refresh(const E& ea)
        {
          this->_create_log_file(ea, "all_pop.dat");
          if (ea.gen() % Params::stats::logall_period != 0)
            return;
          typedef std::vector<boost::shared_ptr<Phen> > pareto_t;
          pareto_t p = ea.pop();
	  for (size_t i = 0; i < p.size(); ++i)
	    {
	      (*this->_log_file) << ea.gen() << " ";
	      for (size_t j = 0; j < p[i]->fit().objs().size(); ++j)
		(*this->_log_file) << p[i]->fit().obj(j) << " ";
	      (*this->_log_file) << p[i]->fit().mod() << " "
				 << p[i]->fit().optlength();
	      (*this->_log_file) << std::endl;
	    }
        }

        void show(std::ostream& os, size_t k) const {}
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version){ }
    };
  }
}

#endif
