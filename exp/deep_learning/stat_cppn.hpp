//| This file is a part of the sferes2 framework.
//| Copyright 2009, ISIR / Universite Pierre et Marie Curie (UPMC)
//| Main contributor(s): Jean-Baptiste Mouret, mouret@isir.fr
//|
//| This software is a computer program whose purpose is to facilitate
//| experiments in evolutionary computation and evolutionary robotics.
//| 
//| This software is governed by the CeCILL license under French law
//| and abiding by the rules of distribution of free software.  You
//| can use, modify and/ or redistribute the software under the terms
//| of the CeCILL license as circulated by CEA, CNRS and INRIA at the
//| following URL "http://www.cecill.info".
//| 
//| As a counterpart to the access to the source code and rights to
//| copy, modify and redistribute granted by the license, users are
//| provided only with a limited warranty and the software's author,
//| the holder of the economic rights, and the successive licensors
//| have only limited liability.
//|
//| In this respect, the user's attention is drawn to the risks
//| associated with loading, using, modifying and/or developing or
//| reproducing the software by the user in light of its specific
//| status of free software, that may mean that it is complicated to
//| manipulate, and that also therefore means that it is reserved for
//| developers and experienced professionals having in-depth computer
//| knowledge. Users are therefore encouraged to load and test the
//| software's suitability as regards their requirements in conditions
//| enabling the security of their systems and/or data to be ensured
//| and, more generally, to use and operate it in the same conditions
//| as regards security.
//|
//| The fact that you are presently reading this means that you have
//| had knowledge of the CeCILL license and that you accept its terms.


#ifndef STAT_CPPN_HPP_
#define STAT_CPPN_HPP_

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/iterator/counting_iterator.hpp>

#include <sferes/stc.hpp>
#include <sferes/parallel.hpp>
#include <sferes/fit/fitness.hpp>
#include <sferes/stat/stat.hpp>

namespace sferes
{
  namespace stat
  {
    SFERES_STAT(Cppn, Stat)
    {
    public:     
      template<typename E>
	void refresh(const E& ea)
      {
	this->_create_log_file(ea, "cppn.dat");
	if (ea.dump_enabled())
	  {
	    float size_cppn = 0;
	    float size_cppn_max = 0;
	    float fit = 0;
	    float fit_max = 0;
	    float connections = 0;
	    float connections_max = 0;
	    float cost = 0;
	    float cost_max = -1e15;
	    for (size_t i = 0; i < ea.pop().size(); ++i)
	      {
		size_cppn += ea.pop()[i]->fit().cppn_nodes();
		size_cppn_max = 
		  std::max(size_cppn_max, 
			   (float)ea.pop()[i]->fit().cppn_nodes());
		fit += ea.pop()[i]->fit().obj(0);
		fit_max = std::max(fit_max, ea.pop()[i]->fit().obj(0));
		connections += ea.pop()[i]->fit().nn_conns();
		connections_max =
		  std::max(connections_max,
			   (float) ea.pop()[i]->fit().nn_conns());
		cost += ea.pop()[i]->fit().obj(1);
		cost_max = std::max(cost_max, ea.pop()[i]->fit().obj(1));
	      }
	    size_cppn /= ea.pop().size();
	    fit /= ea.pop().size();
	    connections /= ea.pop().size();
	    cost /= ea.pop().size();

	    // unique behaviors
	    std::list<int> 
	      pop_ids(boost::counting_iterator<int>(0),
		      boost::counting_iterator<int>(ea.pop().size()));
	    int nb_unique = 0;
	    while (!pop_ids.empty())
	      {
		std::vector<int> cluster;
		std::list<int>::iterator it = pop_ids.begin();
		int id = pop_ids.front();
		while (it != pop_ids.end())
		  if (ea.pop()[id]->fit().behavior() == ea.pop()[*it]->fit().behavior())	
		    it = pop_ids.erase(it);		      
		  else
		    ++it;
		++nb_unique;
	      }


	    *(this->_log_file) << ea.gen() << " "
			       << size_cppn << " " << size_cppn_max << " "
			       << fit << " " << fit_max << " "
			       << connections << " " << connections_max << " "
			       << nb_unique << " " //# unique behaviors
			       << ea.pareto_front().size() << " "
			       << cost << " " << cost_max
			       << std::endl;
	  }
      }          

    protected:
    };
  }
}
#endif
