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




#ifndef AGENT_HPP
#define AGENT_HPP

#include <sferes/dbg/dbg.hpp>
#include <sferes/stc.hpp>

namespace sferes
{
  namespace ctrl
  { 
    
    size_t in(int i) { return i; }

    template<typename T>
    float out(const T& t) { return t.output(); }
    
    template<typename T>
    float out(const T& t, size_t i) { return t.output(i); }
    

    SFERES_CLASS(Agent)
    {
    public:
      Agent() : _first(true) {}
      template<typename Indiv>
	void init(const Indiv& indiv) { stc::exact(this)->init(); }

      template<typename Simu, typename Individual>
	void refresh(Simu& s, Individual& indiv)
      {
	if (_first)
	  {
	    refresh_params(indiv);
	    _first = false;
	  }
	refresh_sensors(s);
	refresh_arch(s);
	refresh_env(s);
      }
      template<typename Indiv>
	void refresh_params(Indiv& i) 
      {
	dbg::out(dbg::info, "agent")<<"refresh_param"<<std::endl;
	stc::exact(this)->refresh_params(i); 
      }

      template<typename Simu>
	void refresh_sensors(const Simu& s) 
      { 
	dbg::out(dbg::info, "agent")<<"refresh_sensors"<<std::endl;
	stc::exact(this)->refresh_sensors(s); 
      }

      template<typename Simu>
	void refresh_arch(const Simu& s) 
      { 
	dbg::out(dbg::info, "agent")<<"refresh_arch"<<std::endl;
	stc::exact(this)->refresh_arch(s); 
      }

      template<typename Simu>
	void refresh_env(Simu& s) 
      {
	dbg::out(dbg::info, "agent")<<"refresh_env"<<std::endl; 
	stc::exact(this)->refresh_env(s); 
      }

    protected:
      bool _first;
    };

    SFERES_CLASS_D(AgentDummy, Agent)
    {
    public:
      template<typename Indiv>
      void init(const Indiv&) {}
      template<typename Indiv>
	void refresh_params(const Indiv& i) {}
      template<typename Simu>
	void refresh_sensors(const Simu& s) {}
      template<typename Simu>
	void refresh_arch(const Simu& s) {}
      template<typename Simu>
	void refresh_env(Simu& s) {}
    };
  }
}

#define SFERES_AGENT SFERES_CLASS_D 
#endif
