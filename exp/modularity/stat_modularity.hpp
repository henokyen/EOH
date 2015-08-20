#ifndef STAT_MODULARITY_HPP_
#define STAT_MODULARITY_HPP_

#include <numeric>
#include <stdio.h>

namespace sferes
{
  namespace stat
  {
     // lexical order
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
     // use fitness::values()
    struct compare_values
    {
      compare_values(size_t k) : _k(k) {
      }
      template<typename I>
      bool operator() (const boost::shared_ptr<I>&i1,
                       const boost::shared_ptr<I>&i2) const
      {
        return i1->fit().values(_k) > i2->fit().values(_k);
      }
      size_t _k;
    };
   struct compare_hier_mod
    {
      compare_hier_mod(){
      }
      template<typename I>
      bool operator() (const boost::shared_ptr<I>&i1,const boost::shared_ptr<I>&i2) const
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

SFERES_STAT(Mod, Stat)
    {
      public:
        template<typename E>
        void refresh(const E& ea)
        {
          //this->_create_log_file(ea, "mod_hier_diff_st_point_.dat");
        	this->_create_log_file(ea, "fixed_no_skip_3outputs.dat");
          if (ea.gen() % Params::stats::period != 0)
            return;
          typedef std::vector<boost::shared_ptr<Phen> > pareto_t;
          pareto_t p = ea.pareto_front();
          std::sort(p.begin(), p.end(), compare_val_objs());
          //pareto_t p_v1 = ea.pop();
          //pareto_t p_v2 = ea.pop();
         // std::sort(p_v1.begin(), p_v1.end(), compare_values(0));
          //std::sort(p_v2.begin(), p_v2.end(), compare_values(1));

          std::vector<float> mod, length, optlength,hierar;
          _best.clear();
          mod.clear();
          length.clear();
          optlength.clear();
          hierar.clear();

          boost::shared_ptr<Phen> b = p[0];
          //std::cout<<"Preto size:"<<p.size()<<std::endl;
          for (size_t i = 0; i < p.size() &&
               p[i]->fit().obj(0) == p[0]->fit().obj(0);
               i=p.size())
          {
	    b = p[i];
            _best.push_back(b);
            mod.push_back(b->fit().mod());
            length.push_back(b->fit().length());
            optlength.push_back(b->fit().optlength());
	    hierar.push_back(b->fit().get_grc());
          }
          float avg_mod = std::accumulate(mod.begin(), mod.end(), 0.0f) / mod.size();
          float avg_length = std::accumulate(length.begin(), length.end(), 0.0f) / length.size();
          float avg_optlength =
          std::accumulate(optlength.begin(), optlength.end(), 0.0f) / optlength.size();
	  float avg_hierar=std::accumulate(hierar.begin(),hierar.end(),0.0f)/hierar.size();
          std::sort(mod.begin(), mod.end());
          std::sort(length.begin(), length.end());
          std::sort(optlength.begin(), optlength.end());
	  std::sort(hierar.begin(),hierar.end());
     //std::cout<<_best[0]->fit().value()<<":"<<_best[0]->fit().get_grc()<<std::endl;
          (*this->_log_file) <<std::setprecision(5) 
                             << ea.gen() << " "                 // generation
                             << _best[0]->fit().value() << " "  // best fitnesss (value)
			     << _best[0]->fit().mod() << " " // mod of the first indiv (ild report)
		    	     << _best[0]->fit().get_grc()<<" "// hierarchy of the first indiv
			//<<_best[0]->fit().get_grc()<<" "	
                           //  << avg_mod << " "                  // average modularity (among optimal indivs)
                             //<< avg_length << " "               // average length
                             //<< avg_optlength << " "            // average optimized length
	  		 //    <<avg_hierar<<" "			//average hierarchy (among optimal indivs)
                            // << p_v1[0]->fit().values(0) << " " // best indiv for objective 0, value of objective 2
                             //<< p_v2[0]->fit().values(1) << " " // best indiv for objectiv 1, value of objective 1
                            // << mod.size() << " "               // number of optimal indivs
                             << mod[mod.size() / 2] << " "      // median modularity
                             << length[length.size() / 2] << " " // median length
                             << optlength[optlength.size() / 2] << " " // median opt length
		             <<hierar[hierar.size()/2]<<" "  		// median hierarchy
                             << *std::max_element(mod.begin(), mod.end()) << " "
                             << *std::min_element(mod.begin(), mod.end()) << " "
			     << *std::max_element(hierar.begin(), hierar.end()) << " "
                             << *std::min_element(hierar.begin(), hierar.end()) << " "
<<"\n";
                             //<< *std::max_element(length.begin(), length.end()) << " "
                             //<< *std::min_element(length.begin(), length.end()) << " "
                             //<< *std::max_element(optlength.begin(), optlength.end()) << " "
                            // << *std::min_element(optlength.begin(), optlength.end()) << " ";
         /* for (size_t i = 0; i < _best[0]->fit().nb_values(); ++i)
          (*this->_log_file) << _best[0]->fit().values(i) << " ";
         // (*this->_log_file) << std::endl;*/
        /*if (stop_criterion(ea)) {
        	std::cout << "Stop criterion reached" << std::endl;
		ea.write();
                (*this->_log_file).close();
	         exit(0);
		 }*/
	}
template<typename EA>
void reset (EA& ea)
{
      this->delete_log_file(ea, "mod_hierievo.dat");     
}
template<typename EA>
bool stop_criterion(const EA& ea) const
  {
    for (size_t i = 0; i < ea.pop().size(); ++i)
       if (ea.pop()[i]->fit().value() == 1.0)
        	return true;
    return false;
  }
  // void show(std::ostream& os, size_t k)
    // {
      //  _best[k]->develop();
        // _best[k]->show(os);
          //_best[k]->fit().set_mode(fit::mode::view);
          //_best[k]->fit().eval(*_best[k]);
        //}
      void show(std::ostream& os, size_t k = 0)
        {
 /*         _best[k]->develop();
 *          _best[k]->show(os);
 *          _best[k]->fit().set_mode(fit::mode::view);
 *          _best[k]->fit().eval(*_best[k]);*/
        std::sort(_best.begin(),_best.end(),compare_hier_mod());
        if(k < _best.size()){
        _best[k]->develop();
        //_best[k]->show(os);
        _best[k]->fit().set_mode(fit::mode::view);
        _best[k]->fit().eval(*_best[k]);
         os<<" "   <<_best[0]->fit().value();
         os << " " << _best[k]->fit().mod();
         os << " " << _best[k]->fit().get_grc();
         os<<  " " <<_best[k]->fit().optlength();
         os << "\n";
        
        } else {
        for(int i=0; i<_best.size(); i++){
            _best[i]->develop();
            _best[i]->fit().set_mode(fit::mode::view);
            std::ostringstream ss;
            ss << std::setw( 4 ) << std::setfill( '0' ) << i;
           _best[i]->fit().setPostFix(ss.str());
           _best[i]->fit().eval(*_best[i]);
            os << i;
            for(int j=0; j < _best[i]->fit().objs().size(); j++){
            os << " " << _best[i]->fit().obj(j);
            }
            os<<" "  <<_best[0]->fit().value();
            os << " " << _best[i]->fit().mod();
            os << " " << _best[i]->fit().get_grc();
            os<<  " " <<_best[k]->fit().optlength();
            os << "\n";
        }
      }
   }

        const boost::shared_ptr<Phen> best() const { return _best[0]; }
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
          ar& BOOST_SERIALIZATION_NVP(_best);
        }
      protected:
        std::vector<boost::shared_ptr<Phen> > _best;
    };
  }
}

#endif
