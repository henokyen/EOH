#ifndef GRADUAL_CONNECTIONS_HPP_
#define GRADUAL_CONNECTIONS_HPP_

namespace sferes
{
namespace modif
{
SFERES_CLASS(Gradual){
public:

	// we start at -1 to allow a dump _before_ the switch
	Gradual(){
	}

	template<typename Ea>
	void apply(Ea& ea)
	{
		switch(ea.gen()){
		case 5000:  Params::ea::pressure = 0.9; break;
		case 10000: Params::ea::pressure = 0.8; break;
		case 15000: Params::ea::pressure = 0.7; break;
		case 20000: Params::ea::pressure = 0.6; break;
		case 25000: Params::ea::pressure = 0.5; break;            
                case 30000: Params::ea::pressure = 0.4; break;
                case 35000: Params::ea::pressure = 0.3; break;
                case 40000: Params::ea::pressure = 0.2; break;
                case 45000: Params::ea::pressure = 0.1; break;
//		default:
			//Do nothing
		}

#ifdef DIV
          _mod_div.apply(ea);
#endif
	}

#ifdef DIV
        BehaviorDiv<Params> _mod_div;
#endif
};
}
}
#endif //GRADUAL_CONNECTIONS_HPP_
