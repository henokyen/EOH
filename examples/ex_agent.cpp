#include <iostream>
#include <boost/foreach.hpp>
#include "sferes/ctrl.hpp"


namespace sferes
{
  namespace ctrl
  {    

    SFERES_SENSOR(Sensor1, Sensor)
    {
    public:
      Sensor1() { this->_output = 1; }
      template<typename T>
      void refresh(const T& env) { std::cout<<"sensor 1"<<std::endl;}
    };

    SFERES_SENSOR(Sensor2, Sensor1)
    {
    public:
      Sensor2() { this->_output = 2; }
      template<typename T>
      void refresh(const T& env) { std::cout<<"sensor2"<<std::endl; } 
    };


    SFERES_AGENT(AgentTest, Agent)
    {
    public:
      AgentTest() : 
	arch1(2, 2), 
	arch2(1, 1)
	  {}
      
      template<typename Indiv>
	void refresh_params(const Indiv& indiv)
      {	
      }
      
      template<typename Simu>
      void refresh_sensors(const Simu& s)
      {
	s1.refresh(s);
	s2.refresh(s);
      }

      template<typename Simu>
      void refresh_arch(const Simu& s)
      {
	arch1 << in(0) << out(s1)
 	      << in(1) << out(s2)
	      << end;
 	arch2 << out(arch1) << end;
 	a1 << out(arch2) << end;
 	a2 << out(s1) << end;
      }

      template<typename Simu>
      void refresh_env(Simu& s)
      {}
      
    protected:

      Sensor1<> s1;
      Sensor2<> s2;
      ArchSum<> arch1;
      ArchSum<> arch2;
      ActDummy<> a1, a2;
    };

  }
}
 
int main()
{
  using namespace sferes::ctrl;

  sferes::ctrl::AgentTest<> agent;
  agent.refresh(agent, agent);

  return 0;
}
