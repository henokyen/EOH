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




#ifndef BEST_FIT_
#define BEST_FIT_

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <sferes/stat/stat.hpp>
#include <numeric>
namespace sferes
{
  namespace stat
  {
    
  struct compare_val_objs
    {
      compare_val_objs(){
      }
      template<typename I>
      bool operator() (const boost::shared_ptr<I> i1, const boost::shared_ptr<I> i2) const
      {
        assert(i1->fit().objs().size() == i2->fit().objs().size());
        assert(i1->fit().objs().size());
        assert(i2->fit().objs().size());
          //first, compare the main objective
        if (i1->fit().value() > i2->fit().value())
          return true;
        if (i1->fit().value() < i2->fit().value())
          return false;
          //equality : compare other objectives
        for (size_t i = 0; i < i1->fit().objs().size(); ++i)
          if (i1->fit().obj(i) > i2->fit().obj(i))
            return true;
          else
          if (i1->fit().obj(i) < i2->fit().obj(i))
            return false;
        return false;
      }
    };
	  struct compare_hier_mod
    {
      compare_hier_mod(){
      }
      template<typename I>
      bool operator() (const boost::shared_ptr<I>i1,const boost::shared_ptr<I>i2) const
      {
            //first, compare based on fitness
        if (i1->fit().value() > i2->fit().value())
           return true;
        else if (i1->fit().value() == i2->fit().value())
        {
             //then compare based on hierarchy
          if (i1->fit().get_grc() > i2->fit().get_grc()) 
              return true;
          else if (i1->fit().get_grc() == i2->fit().get_grc())
              //then based on modularity
              return (i1->fit().mod() > i2->fit().mod());
          else 
             return false; 
        }
      return false;

      }
      
    };
	  // assume that the population is sorted !
    SFERES_STAT(BestFit, Stat)
    {
    public:
      template<typename E>
	void refresh(const E& ea)
      {
	assert(!ea.pop().empty());
	typedef std::vector<boost::shared_ptr<Phen> > pareto_t;
    pareto_t p = ea.pareto_front();
    std::sort(p.begin(), p.end(), compare_val_objs());
    //_best = *ea.pop().begin();
	_best[0]=p.begin();
	this->_create_log_file(ea, "bestfit.dat");
	if (ea.dump_enabled())
	  (*this->_log_file) << ea.gen() << " " << _best[0]->fit().value() << std::endl;
      }
      void show(std::ostream& os, size_t k)
      {
	_best->develop();
	_best->show(os);
	_best->fit().set_mode(fit::mode::view);
	_best->fit().eval(*_best);


      }
      const boost::shared_ptr<Phen> best() const { return _best; }
      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
        ar & BOOST_SERIALIZATION_NVP(_best);
      }
    protected:
//boost::shared_ptr<Phen> _best;
  std::vector<boost::shared_ptr<Phen> > _best;
    };
  }
}
#endif
