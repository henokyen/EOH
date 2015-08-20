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




#ifndef SENSOR_HPP
#define SENSOR_HPP

#include "sferes/stc.hpp"

namespace sferes
{
  namespace ctrl
  {
    SFERES_CLASS(Sensor)
    {
    public:
      Sensor() : _output(0.0f) {}
      Sensor(float v) : _output(v) {}
      
      float output() const { return _output; }
      
      template<typename T>
	void refresh(const T& env) { stc::exact(this)->refresh(env); }

    protected:
      float _output;
    };

    SFERES_CLASS_D(SensorDummy, Sensor)
    {
    public:
      SensorDummy(float v) : SFERES_PARENT(SensorDummy, Sensor)(v) {}
      
      template<typename T>
	void refresh(const T& env) 
      {}
    };

  }
}

#define SFERES_SENSOR SFERES_CLASS_D
#endif
