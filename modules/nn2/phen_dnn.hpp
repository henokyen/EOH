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




#ifndef PHEN_DNN_HPP
#define PHEN_DNN_HPP

#include <map>
#include <bitset>
#include <sferes/phen/indiv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include "gen_dnn.hpp"
#include <bitset>
#include <vector>
#include <sstream>

namespace sferes
{
  namespace phen
  {

    SFERES_INDIV(Dnn, Indiv)
    {
    public:
    static const size_t DELTA = 25;
    static const size_t b_size = 256;
    static const size_t Mutation = 200;
    Dnn (){}
    template  <typename T>
    std::string NumberToString ( T Number )
      {
         std::ostringstream ss;
         ss << Number;
         return ss.str();
      }
    Dnn* clone (const Dnn& i)
    {
    	//std::cout<< "copying ..?? \n"<<std::endl;
    	//Dnn *temp = new Indiv();
    	this->_fit=i._fit;
    	this->_gen=i._gen;
        return this;
      }
    	//*this = i;}
    ~ Dnn (){};
	void develop()
	{
	  //std::cout<< "develop the parameters \n"<<std::endl;
	  BGL_FORALL_VERTICES_T(v, this->gen().get_graph(), 
				typename nn_t::graph_t)
	    {
	      this->gen().get_graph()[v].get_afparams().develop();
	      this->gen().get_graph()[v].get_pfparams().develop();
	    }
	  BGL_FORALL_EDGES_T(e, this->gen().get_graph(), 
			     typename nn_t::graph_t)
	    {
	      this->gen().get_graph()[e].get_weight().develop();
	    }
	  // init everything
	  this->_gen.init();	  
	}
void evolvbility(Dnn& ind)
	{
		std::vector<std::bitset<b_size> > offspring;
		std::ofstream log_file("/home/henok/workspace/parent.dat");
		ind.develop();
		ind.show(log_file);

		//ind.fit().offspring_behavoir(ind,behavior);
		for (int i= 0;i<10;i++)
		{
		//copy that individual
		Dnn *copyp= clone (ind);
		//mutate it
		copyp->gen().mutate();
		//develope it. That is how sferes work, mutate and then develope
		copyp->develop();

		//compute the behavior of one of the offspring
		//boost::dynamic_bitset<> behavior(b_size);
		std::string fname ="/home/henok/workspace/offspring"+copyp->NumberToString(i)+".dat";
		std::ofstream log_file2(fname.c_str());
		copyp->show(log_file2);
		std::bitset <b_size> behavior;
		copyp->fit().offspring_behavoir(copyp,behavior);
		//copyp->fit().offspring_behavoir(copyp);
		offspring.push_back(behavior); //accumulating all offspring of an individual
		//for (boost::dynamic_bitset<>::size_type i = 0;i < behavior.size(); ++i)
		  //        std::cout << behavior[i];
		//std::cout<<"\n $$$$$\n";
		}
		distnicts(offspring, Mutation);
//		int size=0;
//		for(int i=0;i<10;i++)
//		 {
//			  int d=0;
//			  bool pass=true;
//			  for(int j=0;j<size;j++) {
//		        d += offspring[i]->fit().be_dist(offspring[j]);
//			    if(d<DELTA) {
//			     pass=false;
//			     break;
//			  }
//			 }
//
//			 if(pass)
//			   size++;
//		 }
//	std::cout<<"\n Evolvability value: "<<size;
//	std::cout<<"\n";
	}
// compute the distinctness among offspring
template <size_t nb_bits>
int distnicts(std::vector<std::bitset <nb_bits> >& offspring, int len)
{
	int size=0;
	for(int i=0;i<len;i++)
	 {
		  int d=0;
		  bool pass=true;
		  for(int j=0;j<size;j++) {
	        d = (offspring[i]^ offspring[j]).count();
		    if(d<=DELTA) {
		     pass=false;
		     break;
		  }
		 }

		 if(pass)
		   ++size;
	 }
std::cout<<"Evolvability value: "<<size<<"\n";
return size;
}
//int dist(boost::dynamic_bitset<> offp1, boost::dynamic_bitset<> offp2) const
//{
//  return (offp1 ^ offp2).count();
//}
void show(std::ostream& os) {  this->gen().write(os);  }
	typedef typename Gen::nn_t nn_t;
nn_t& nn() { return this->gen(); }
	const nn_t& nn() const { return this->gen(); }
      protected:
      };
  }
}


#endif
