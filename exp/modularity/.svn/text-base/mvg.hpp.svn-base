#ifndef MVG_HPP_
#define MVG_HPP_

namespace sferes
{
  namespace modif
  {
    SFERES_CLASS(Mvg)
    {
      public:
         // period of change, define P20 or P50 etc. in the variants (wscript)
#ifdef PP10
        static const size_t period = 10;
#elif defined(P20)
        static const size_t period = 20;
#elif defined(P50)
        static const size_t period = 50;
#elif defined(P100)
        static const size_t period = 100;
#elif defined(P200)
        static const size_t period = 100;
#elif defined(P500)
        static const size_t period = 500;
#elif defined(P1000)
        static const size_t period = 1000;
#elif defined(P2000)
        static const size_t period = 2000;
#elif defined(P5000)
        static const size_t period = 5000;
#elif defined(P10000)
        static const size_t period = 10000;
#else
        static const size_t period = 20;
#endif
         // we start at -1 to allow a dump _before_ the switch
        Mvg() : _obj(0) {
        }
        template<typename Ea>
        void apply(Ea& ea)
        {
          size_t nb_values = ea.pop()[0]->fit().nb_values();
          if (ea.gen() != 1 && (ea.gen() - 1) % period == 0)
          {
            _obj = (_obj + 1) % nb_values;
            std::cout << ea.gen() << " => switch to obj:" << _obj << std::endl;
          }
          int obj = _obj;
          float prob = 0.0;

#ifdef GRAD
          if ((ea.gen() % period) < period * 0.1)
            prob = 1.0f - (ea.gen() % period) / (period * 0.1f);
#endif
          for (size_t i = 0; i < ea.pop().size(); ++i)
          {
            if (misc::rand<float>() < prob)
              obj = std::max(0, (int)_obj - 1);
            else
              obj = _obj;
            assert(_obj < ea.pop()[i]->fit().nb_values());
            ea.pop()[i]->fit().set_obj(0, ea.pop()[i]->fit().values(obj));
            ea.pop()[i]->fit().set_value(ea.pop()[i]->fit().values(obj));
          }

#ifdef DIV
          _mod_div.apply(ea);
#endif
        }

      protected:
        size_t _obj;
#ifdef DIV
        BehaviorDiv<Params> _mod_div;
#endif
    };
  }
}
#endif
