#ifndef STAT_MAP_HPP_
#define STAT_MAP_HPP_

#include <numeric>
#include <fstream>
#include <boost/multi_array.hpp>
#include <sferes/stat/stat.hpp>
#include "map_elite.hpp"

namespace sferes
{
  namespace stat
  {
    SFERES_STAT(Map, Stat)
    {
    public:
      typedef boost::shared_ptr<Phen> phen_t;
      typedef boost::multi_array<phen_t, 2> array_t;
      typedef boost::array<float, 2> point_t;

      Map() : _xs(0), _ys(0) {}
      template<typename E>
        void refresh(const E& ea)
      {
        this->_create_log_file(ea, "average_fitness.dat");
       // _avg_fitness.clear();
	_archive.clear();	  
	_xs = ea.archive().shape()[0];
	_ys = ea.archive().shape()[1];
	assert(_xs == Params::ea::res_x);
	assert(_ys == Params::ea::res_y);

	for (size_t i = 0; i < _xs; ++i)
	  for (size_t j = 0; j < _ys; ++j)
	    {
	      phen_t p = ea.archive()[i][j];
	      _archive.push_back(p);
            } 
       writetofile(ea.archive(),ea,this->_log_file);
	if (ea.gen() % Params::pop::dump_period == 0)
	  {
	    _write_archive(ea.archive(), std::string("archive_"), ea);
#ifdef MAP_WRITE_PARENTS
	    _write_parents(ea.archive(), ea.parents(), std::string("parents_"), ea);
#endif
	  }
      }
      void show(std::ostream& os, size_t k)
      {
        //std::cout<<"what is K :"<<k<<std::endl;
	//std::cerr << "loading "<< k / _ys << "," << k % _ys << std::endl;
        //std::cout<<"Size of _archive is :"<<_archive.size()<<std::endl;
        for (int i = k; i < _archive.size(); i++)
        {
	    if (_archive[i] && (*_archive[i]).gen().is_validnetwork())
	     {
	       _archive[i]->develop();
          //     std::cout<<"nb neurons in stat_map one:  "<<(*_archive[i]).gen().nb_neurons()<<std::endl;
	       _archive[i]->show(os);
	       _archive[i]->fit().set_mode(fit::mode::view);
               std::ostringstream ss;
               ss << std::setw( 4 ) << std::setfill( '0' ) << i;
               _archive[i]->fit().setPostFix(ss.str());
              // print the number of neurons here
            //  std::cout<<"nb neurons in stat_map "<<(*_archive[i]).gen().nb_neurons()<<std::endl;
               _archive[i]->fit().eval(*_archive[i]);
	     }
	//else
	  //std::cerr << "Warning, no point here" << std::endl;
       }
    }
      template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
      {
        //std::cout<<"does it come here ?"<<std::endl;	 
	ar & BOOST_SERIALIZATION_NVP(_archive);
	ar & BOOST_SERIALIZATION_NVP(_xs);
	ar & BOOST_SERIALIZATION_NVP(_ys);
      }
    protected:
      std::vector<float > _avg_fitness;
      std::vector<phen_t> _archive;
      int _xs, _ys;

      template<typename EA>
      void _write_parents(const array_t& array,
			  const array_t& p_array,
			  const std::string& prefix,
			  const EA& ea) const
      {
	std::cout << "writing..." << prefix << ea.gen() << std::endl;
	std::string fname =  ea.res_dir() + "/"
	  + prefix
	  + boost::lexical_cast<
	    std::string>(ea.gen())
	  + std::string(".dat");
	std::ofstream ofs(fname.c_str());
	for (size_t i = 0; i < _xs; ++i)
	  for (size_t j = 0; j < _ys; ++j)
	    if (array[i][j] && p_array[i][j])
	      {
		point_t p =ea.get_point(p_array[i][j]);
                //point_t p = _get_point(p_array[i][j]);
		size_t x = round(p[0] * _xs);
		size_t y = round(p[1] * _ys);
		ofs << i / (float) _xs
		    << " " << j / (float) _ys
		    << " " << p_array[i][j]->fit().value()
		    << " " << x / (float) _xs
		    << " " << y / (float) _ys
		    << " " << array[i][j]->fit().value()
		    << std::endl;
	      }
      }

      template<typename EA>
      void _write_archive(const array_t& array,
			  const std::string& prefix,
			  const EA& ea) const
      {
	std::cout << "writing..." << prefix << ea.gen() << std::endl;
	std::string fname = ea.res_dir() + "/"
	  + prefix
	  + boost::lexical_cast<
	    std::string>(ea.gen())
	  + std::string(".dat");
	
	std::ofstream ofs(fname.c_str());
	for (size_t i = 0; i < _xs; ++i)
	  for (size_t j = 0; j < _ys; ++j)
	    if (array[i][j])
	      {
              //std::cout<<"how did you get here?"<<std::endl; 
		ofs << i / (float) _xs
		    << " " << j / (float) _ys
		    << " " << array[i][j]->fit().value()
		    << std::endl;
	      }
      }
     template<typename EA>
     void writetofile(const array_t& array,const EA& ea, boost::shared_ptr<std::ofstream> logfile)
      {
      _avg_fitness.clear();
      //std::cout<<"what is going?"<<endl;
        for (size_t i = 0; i < _xs; ++i)
          for (size_t j = 0; j < _ys; ++j)
            if (array[i][j] )
               {
  //               std::cout<<"are u writing? if there is a point"<<endl;
                _avg_fitness.push_back(array[i][j]->fit().value());
               }
//std::cout<<_avg_fitness.size()<<std::endl;
*logfile<<std::setprecision(3)<< ea.gen() <<" "<<std::accumulate(_avg_fitness.begin(),_avg_fitness.end(),0.0f)/_avg_fitness.size()<<"\n";
      }

    };
  }
}

#endif
