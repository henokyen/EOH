#ifndef STAT_ARCHIVE_HPP_
#define STAT_ARCHIVE_HPP_

#include <numeric>

namespace sferes
{
  namespace stat
  {

    SFERES_STAT(Archive, Stat)
    {
      public:
        template<typename E>
        void refresh(const E& ea)
        {
	  typedef boost::shared_ptr<typename E::phen_t> phen_t;
	  _archive.clear();	  
	  size_t xs = 
	    boost::fusion::at_c<0>(ea.fit_modifier()).archive().shape()[0];
	  size_t ys = 
	    boost::fusion::at_c<0>(ea.fit_modifier()).archive().shape()[1];
	  
	  for (size_t i = 0; i < xs; ++i)
	    for (size_t j = 0; j < ys; ++j)
	      {
		float p = 
		  boost::fusion::at_c<0>(ea.fit_modifier()).archive()[i][j];
	//	if (p)
		  _archive.push_back(p);
	      }
        }
        void show(std::ostream& os, size_t k)
        {
          //_archive[k]->develop();
          //_archive[k]->show(os);
          //_archive[k]->fit().set_mode(fit::mode::view);
          //_archive[k]->fit().eval(*_archive[k]);
        }
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
          ar& BOOST_SERIALIZATION_NVP(_archive);
        }
      protected:
        //std::vector<boost::shared_ptr<Phen> > _archive;
        typedef boost::multi_array<float, 2> _archive;
    };
  }
}

#endif
