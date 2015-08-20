//#define NO_PARALLEL
#ifdef AGE
#define TRACK_FIT
#endif

#include <iostream>
#include <bitset>
#include <boost/assign/list_of.hpp>
#include <boost/random.hpp>

#include <sferes/phen/parameters.hpp>
#include <sferes/gen/evo_float.hpp>
#include <sferes/gen/sampled.hpp>
#include <sferes/ea/nsga2.hpp>
#include <sferes/eval/eval.hpp>
#include <sferes/stat/pareto_front.hpp>
#include <sferes/modif/dummy.hpp>
#include <sferes/run.hpp>
#include <modules/nn2/nn.hpp>
#include <modules/nn2/gen_dnn_ff.hpp>
#include <modules/nn2/phen_dnn.hpp>

#include <modules/modularity/modularity.hpp>

#include "behavior_div.hpp"
#include "gen_spatial.hpp"
#include "stat_modularity.hpp"
#include "mvg.hpp"
#include "modularity.hpp"
#include "mod2.hpp"

#include "stat_logall.hpp"

#include "gen_hnn.hpp"
#include "phen_hnn.hpp"


#ifdef AGE
# include "modif_age.hpp"
#endif

#ifdef PNSGA
# include "pnsga.hpp"
#endif

#ifdef NOVMAP
# include "modif_novmap.hpp"
#endif

#include <boost/algorithm/string.hpp>
#define SFERES_PARAMS(P)                                                    \
  struct Ps__ { Ps__() {                                                    \
                  static std::string __params = # P;                        \
                  boost::replace_all(__params, ";", ";\n");                 \
                  boost::replace_all(__params, "{", "{\n");                 \
                  boost::replace_all(__params, "static const", "");         \
                  std::cout << "Parameters:" << __params << std::endl; } }; \
  P;                                                                        \
  static Ps__ ____p;

using namespace sferes;
using namespace nn;
using namespace sferes::gen::evo_float;
using namespace sferes::gen::dnn;


// -1 to +1 sigmoid
template<typename P>
struct AfRetina : public Af<P> {
	typedef P params_t;
	static const float lambda = 20.0f;
	AfRetina() {
		assert(trait<P>::size(this->_params) == 1);
	}
	float operator()(float p) const {
		return tanh(lambda * (p + trait<P>::single_value(this->_params)));
	}
protected:
};


SFERES_PARAMS(
struct Params {
	static long seed;
	struct pop
	{
		static const unsigned size = 1000;
#ifdef NOVMAP
		static const unsigned nb_gen = 10001;
#else
		static const unsigned nb_gen = 25001;
#endif
		static const int dump_period = nb_gen - 1;
		static const int initial_aleat = 1;
	};
	struct ea
	{
#ifdef PR10
		SFERES_ARRAY(float, obj_pressure, 1.0, 0.1, 1.0);
#elif defined(PR25)
		SFERES_ARRAY(float, obj_pressure, 1.0, 0.25, 1.0);
#elif defined(PR50)
		SFERES_ARRAY(float, obj_pressure, 1.0, 0.5, 1.0);
#elif defined(PR75)
		SFERES_ARRAY(float, obj_pressure, 1.0, 0.75, 1.0);
#elif defined(PR95)
		SFERES_ARRAY(float, obj_pressure, 1.0, 0.95, 1.0);
#else
		SFERES_ARRAY(float, obj_pressure, 1.0, 1.0, 1.0);
#endif
	};
	struct rand_obj
	{
		static const float std_dev = 0.1;
	};
	struct dnn
	{
		// for weights
		struct weights
		{
			struct evo_float
			{
				static const float cross_rate = 0.5f;
				//1.0 because dnn handle this globally
				static const float mutation_rate = 1.0f;
				static const float eta_m = 10.0f;
				static const float eta_c = 10.0f;
				static const mutation_t mutation_type = polynomial;
				static const cross_over_t cross_over_type = sbx;
			};
			struct sampled
			{
				// no 0 !
				SFERES_ARRAY(float, values, 2, -1, 1, 2); //???-2??
				static const float mutation_rate = 1.0f;
				static const float cross_rate = 0.5f;
				static const bool ordered = true;
			};
		};
		struct bias
		{
			struct evo_float
			{
				static const float cross_rate = 0.5f;
				static const float mutation_rate = 1.0f / 24.0f;
				static const float eta_m = 10.0f;
				static const float eta_c = 10.0f;
				static const mutation_t mutation_type = polynomial;
				static const cross_over_t cross_over_type = sbx;
			};
			struct sampled
			{
				SFERES_ARRAY(float, values, -2, -1, 0, 1, 2);
				static const float mutation_rate = 1.0 / 24.0f;
				static const float cross_rate = 0.0f;
				static const bool ordered = false;
			};
		};

		static const size_t nb_inputs = 8;
# ifdef SEASONS
		static const size_t nb_outputs = 2;
# else
		static const size_t nb_outputs = 1;
# endif

		// slope of the sigmoid
		static const size_t lambda = 20;

		static const size_t min_nb_neurons = 10; //useless
		static const size_t max_nb_neurons = 30; //useless
		static const size_t min_nb_conns = 0; // ?? TODO
		static const size_t max_nb_conns = 120; // ?? TODO

#ifdef MDEL
		static const float m_rate_add_conn = 0.1f;
		static const float m_rate_del_conn = 0.2f;
#elif defined(MDEL2)
		static const float m_rate_add_conn = 0.15f;
		static const float m_rate_del_conn = 0.2f;
#elif defined(MDEL3)
		static const float m_rate_add_conn = 0.02f;
		static const float m_rate_del_conn = 0.2f;
#else
		static const float m_rate_add_conn = 0.2f;
		static const float m_rate_del_conn = 0.2f;
#endif

		//static const float m_rate_change_conn = 0.15; //old default
		static const float m_rate_change_conn = 0.0; //new default?
		static const float m_rate_add_neuron = 0.0f;
		static const float m_rate_del_neuron = 0.0f;
		// average number of weigth changes
		static const float m_avg_weight = 2.0f;

		static const int io_param_evolving = true;
		static const init_t init = ff;
		struct spatial
		{
			static const float min_coord = -4.0f;
			static const float max_coord = 4.0f;
#ifdef RANDCOORDS
			static const bool rand_coords = true;
#else
			static const bool rand_coords = false;
#endif

#ifdef SEASONS
			SFERES_ARRAY(int, layers, 8, 8, 4, 2, 2);
			SFERES_ARRAY(float, x,
			-3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5,
			-3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5,
			-2, -1, 1, 2,
			-1, 1,
			-1, 1);
			SFERES_ARRAY(float, y,
			0, 0, 0, 0, 0, 0, 0, 0,
			1, 1, 1, 1, 1, 1, 1, 1,
			2, 2, 2, 2,
			3, 3,
			4, 4);
			SFERES_ARRAY(float, z,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0,
			0, 0);
#else
			//wider separation (best with tlength of 3.5) // new default
			SFERES_ARRAY(int, layers, 8, 8, 4, 2, 1);
			/*       SFERES_ARRAY(float, x,
			                    -5, -4, -3, -2, 2, 3, 4, 5,
			                    -5, -4, -3, -2, 2, 3, 4, 5,
			                    -3, -2, 2, 3,
			                    -2, 2,
			                    0);
			       SFERES_ARRAY(float, y,
			                    0, 0, 0, 0, 0, 0, 0, 0,
			                    1, 1, 1, 1, 1, 1, 1, 1,
			                    2, 2, 2, 2,
			                    3, 3,
			                    4);
			       SFERES_ARRAY(float, z,
			                    0, 0, 0, 0, 0, 0, 0, 0,
			                    0, 0, 0, 0, 0, 0, 0, 0,
			                    0, 0, 0, 0,
			                    0, 0,
			                    0);
			 */
			//used in original experiments
			// SFERES_ARRAY(int, layers, 8, 8, 4, 2, 1);
			SFERES_ARRAY(float, x,
			-3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5,
			-3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5,
			-2, -1, 1, 2,
			-1, 1,
			0);
			SFERES_ARRAY(float, y,
			0, 0, 0, 0, 0, 0, 0, 0,
			1, 1, 1, 1, 1, 1, 1, 1,
			2, 2, 2, 2,
			3, 3,
			4);
			SFERES_ARRAY(float, z,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0,
			0);

#endif
		};
	};

	struct parameters
	{
#ifdef FLOAT
		// maximum value of parameters (weights & bias)
		static const float min = -2;
		// minimum value
		static const float max = 2;
#else
		// maximum value of parameters (weights & bias)
		static const float min = 0;
		// minimum value
		static const float max = 1;
#endif
	};
	struct stats
	{
		//static const size_t period = 1; //print stats every N generations
		static const size_t period = 1; //print stats every N generations
		static const size_t logall_period = 100;
	};
};
)


struct ParamsHnn {
	struct cppn {
		// params of the CPPN
		struct sampled {
			SFERES_ARRAY(float, values, 0, 1, 2);
			static const float mutation_rate = 0.1f;
			static const float cross_rate = 0.25f;
			static const bool ordered = false;
		};
		struct evo_float {
			static const float mutation_rate = 0.1f;
			static const float cross_rate = 0.1f;
			static const mutation_t mutation_type = polynomial;
			static const cross_over_t cross_over_type = sbx;
			static const float eta_m = 10.0f;
			static const float eta_c = 10.0f;
		};
		struct dnn {
			static const size_t nb_inputs = 7;
			static const size_t nb_outputs = 2;
			static const init_t init = ff;
			static const float m_rate_add_conn = 0.03f;
			static const float m_rate_del_conn = 0.02f;
			static const float m_rate_change_conn = 0.05f;
			static const float m_rate_add_neuron = 0.03f;
			static const float m_rate_del_neuron = 0.02f;

			static const size_t min_nb_neurons = 6;
			static const size_t max_nb_neurons = 20;
			static const size_t min_nb_conns = 30;
			static const size_t max_nb_conns = 150;
		};

	};

	struct hnn {
		SFERES_ARRAY(int, layers, 8, 8, 4, 2, 1);
		SFERES_ARRAY(float, x,
		        -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5,
		        -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5,
		        -2, -1, 1, 2,
		        -1, 1,
		        0);
		SFERES_ARRAY(float, y,
		        0, 0, 0, 0, 0, 0, 0, 0,
		        1, 1, 1, 1, 1, 1, 1, 1,
		        2, 2, 2, 2,
		        3, 3,
		        4);
		SFERES_ARRAY(float, z,
		        0, 0, 0, 0, 0, 0, 0, 0,
		        0, 0, 0, 0, 0, 0, 0, 0,
		        0, 0, 0, 0,
		        0, 0,
		        0);
		static const size_t nb_pfparams = 0;
		static const size_t nb_afparams = 0;
		typedef AfRetina<nn::params::Vectorf<1> > af_t;
		typedef nn::PfWSum<> pf_t;
		typedef nn::Neuron<pf_t, af_t> neuron_t;
		typedef nn::Connection<> connection_t;
	};
};

SFERES_FITNESS(FitRetina, sferes::fit::Fitness) {
public:
	FitRetina() : _age(0) {
	}
	int age() const { return _age; }
	void set_age(int a) { _age = a; }
	template<typename Indiv>
	float dist(const Indiv& o) const {
		return (o.fit()._behavior ^ _behavior).count() / (float) _behavior.size();
	}
	// size of the behavior vector
#ifdef SEASONS
	static const size_t b_size = 512;
#else
	static const size_t b_size = 256;
#endif

	template<typename Indiv>
	void eval(Indiv& ind) {
#ifdef TESTSUB
		ind.gen().mutate();
		ind.develop();
#endif
		this->_objs.clear();
		this->_age++;
		_create_objects();
		float err1 = 0, err2 = 0;
		// for seasons
		std::vector<float> errs_o1(4), errs_o2(4);
		std::fill(errs_o1.begin(), errs_o1.end(), 0.0f);
		std::fill(errs_o2.begin(), errs_o2.end(), 0.0f);

		float ex1 = 0, ex2 = 0;

		std::vector<float> inputs(8);
		size_t k = 0;
		for (size_t s = 0; s < 256; ++s) {
			std::bitset<8> in(s);
			for (size_t i = 0; i < in.size(); ++i)
				inputs[i] = in[i] ? 1 : -1;
			bool left = std::find(_left_objects.begin(),
			        _left_objects.end(),
			        (s & 0xF))
			        != _left_objects.end();
			bool right = std::find(_right_objects.begin(),
			        _right_objects.end(),
			        (s & 0xF0))
			        != _right_objects.end();

			// step
#ifdef HNN
			for (size_t i = 0; i < 4; ++i) //TODO HNN
				ind.nn().step(inputs);
#else
			for (size_t i = 0; i < ind.gen().get_depth(); ++i)
				ind.nn().step(inputs);
#endif
#ifdef SEASONS
			float o1 = ind.nn().get_outf(0);
			float o2 = ind.nn().get_outf(1);

			_seasons(left, right, o1, errs_o1);
			_seasons(left, right, o2, errs_o2);

			_behavior[k] = o1 > 0 ? 1 : 0;
			_behavior[k + 1] = o2 > 0 ? 1 : 0;

			k += 2;
#else
			float out = ind.nn().get_outf(0);


#ifdef ROBJECTS
			size_t nb_white = in.count();
			if (nb_white == 4)
				err1 += (out > 0 ? 0 : 1);
			else
				err1 += (out < 0 ? 0 : 1);
#else
			// left & right
			if (left && right)
				err1 += (out > 0 ? 0 : 1);
			else
				err1 += (out < 0 ? 0 : 1);
#endif


			if (left || right)
				err2 += (out > 0 ? 0 : 1);
			else
				err2 += (out < 0 ? 0 : 1);

			_behavior[s] = out > 0 ? 1 : 0;
#ifdef TESTSUB
			// test if there are two xor modules
			size_t k = 0;
			BGL_FORALL_VERTICES_T(v, ind.nn().get_graph(), typename Indiv::nn_t::graph_t)
			std::cout << "nn " << ind.nn().get_graph()[v].get_id() << " "
			        << s << " " << k++ << " "
			        << ind.nn().get_graph()[v].get_afparams().data(0) << " "
			        << ind.nn().get_graph()[v].get_current_output() << std::endl;
#endif
#endif
		}

		this->_objs.clear();
#if defined(MVG) && !defined(SEASONS)
		this->_objs.push_back(-42);
#endif



#ifdef SEASONS

		if (this->mode() == fit::mode::view) {
			std::cout << "o1:" << std::endl;
			for (size_t i = 0; i < errs_o1.size(); ++i)
				std::cout << errs_o1[i] << " ";
			std::cout << std::endl;

			std::cout << "o2:" << std::endl;
			for (size_t i = 0; i < errs_o2.size(); ++i)
				std::cout << errs_o2[i] << " ";
			std::cout << std::endl;
		}

		this->_objs.push_back(-43);
		this->_values.resize(2);
		assert(errs_o1.size() == errs_o2.size());
		for (size_t i = 0; i < errs_o1.size(); ++i)
			errs_o1[i] = 0.5 - errs_o1[i] / 512.0f;
		for (size_t i = 0; i < errs_o2.size(); ++i)
			errs_o2[i] = 0.5 - errs_o2[i] / 512.0f;

		// L&R : 0
		// L|R : 1
		// L : 2
		// R : 3
		// SI1 : L&R + L&R
		// SI2 : L|R + L|R
		// SI3 : L|R + L&R
		// SI4 : L&R + R
		// SI5 : L|R + R
		// SI6 : L + R

#ifndef HFS
		this->_values.resize(2);
#if   defined(SI1) // L&R + L&R
		this->_values[0] = errs_o1[0] + errs_o2[0];
#elif defined(SI2) // L|R + L|R
		this->_values[0] = errs_o1[1] + errs_o2[1];
#elif defined(SI3) // L|R + L&R
		this->_values[0] = errs_o1[0] + errs_o2[1];
#elif defined(SI4) // L&R + R
		this->_values[0] = errs_o1[0] + errs_o2[3];
#elif defined(SI5) // L|R + R
		this->_values[0] = errs_o1[1] + errs_o2[3];
#elif defined(SI6) // L + R
		this->_values[0] = errs_o1[2] + errs_o2[3];
#endif

#if   defined(SP1) // L&R + L&R
		this->_values[1] = errs_o1[0] + errs_o2[0];
#elif defined(SP2) // L|R + L|R
		this->_values[1] = errs_o1[1] + errs_o2[1];
#elif defined(SP3) // L|R + L&R
		this->_values[1] = errs_o1[0] + errs_o2[1];
#elif defined(SP4) // L&R + R
		this->_values[1] = errs_o1[0] + errs_o2[3];
#elif defined(SP5) // L|R + R
		this->_values[1] = errs_o1[1] + errs_o2[3];
#elif defined(SP6) // L + R
		this->_values[1] = errs_o1[2] + errs_o2[3];
#endif
#else
		this->_values.resize(6);
		this->_values[0] = errs_o1[0] + errs_o2[0];
		this->_values[1] = errs_o1[1] + errs_o2[1];
		this->_values[2] = errs_o1[0] + errs_o2[1];
		this->_values[3] = errs_o1[0] + errs_o2[3];
		this->_values[4] = errs_o1[1] + errs_o2[3];
		this->_values[5] = errs_o1[2] + errs_o2[3];
#endif


		if (this->mode() == fit::mode::view)
			for (size_t i = 0; i < this->_values.size(); ++i)
				std::cout << "season " << i << " " << this->_values[i] << std::endl;

#else
		this->_values.resize(2);
		this->_values[0] = 1 - err1 / 256.0f;
		this->_values[1] = 1 - err2 / 256.0f;
#endif

		this->_value = this->_values[0];

		// compute the modularity
#ifndef HNN
		typename Indiv::nn_t::graph_t gsimp = ind.gen().simplified_nn().get_graph();
#else
		typename Indiv::nn_t::graph_t gsimp = ind.simplified_nn().get_graph();
#endif
		if (boost::num_edges(gsimp) == 0 || boost::num_vertices(gsimp) == 0)
			_mod = 0;
		else
			_mod = mod::modularity(gsimp);
		//compute_mod_simplified(ind.gen());


#if defined(FG_AND)  || defined(ROBJECTS)
		this->_objs.push_back(1 - err1 / 256.0f);
		this->_value = this->_values[0];
#endif
#if defined(FG_OR)
		this->_objs.push_back(1 - err2 / 256.0f);
		this->_value = this->_values[1];
#endif

		this->_nb_conns = boost::num_edges(gsimp);
#ifdef NCONNS
		this->_objs.push_back(-(int)boost::num_edges(gsimp));
#endif

		this->_nb_nodes = boost::num_vertices(gsimp);
#ifdef NNODES
		this->_objs.push_back(-(int)boost::num_vertices(gsimp));
#endif

#ifdef NCNODES
		this->_objs.push_back(-(int)boost::num_edges(gsimp) - (int)boost::num_vertices(gsimp));
#endif



#ifdef MOD
		this->_objs.push_back(_mod);
#endif

#ifdef HNN
		_length = ind.compute_length();
		_optlength = _length; //ind.gen().compute_optimal_length();
#else
		_length = ind.gen().compute_length();
		//	_optlength = ind.gen().compute_optimal_length(0, false);
#endif

#ifdef MAX_LENGTH
		this->_objs.push_back(-ind.gen().max_length());
#endif

#if defined(TLENGTH) && !defined(ONP)
#  ifdef HNN
		this->_objs.push_back(-ind.compute_length(3.5)); //use with wider geometric separation, which is the new default
#  else
		this->_objs.push_back(-ind.gen().compute_length(3.5)); //use with wider geometric separation, which is the new default
		//this->_objs.push_back(-ind.gen().compute_length(1.5)); //used in original runs
#  endif
#endif


#if defined(TLENGTH) && defined(ONP)
		bool write_svg = (this->mode() == fit::mode::view);
		this->_objs.push_back(-ind.gen().compute_optimal_length(3.5, write_svg));
#endif

#if defined(LENGTH) && !defined(ONP)
		this->_objs.push_back(-_length);
#endif

#if defined(LENGTH) && defined(ONP)
		bool write_svg = (this->mode() == fit::mode::view);
		this->_objs.push_back(-ind.gen().compute_optimal_length(0, write_svg));
#endif

#if defined(SLENGTH) && defined(ONP)
		bool write_svg = (this->mode() == fit::mode::view);
		float l = ind.gen().compute_optimal_length(0, write_svg);
		this->_objs[0] = ((1 - err1 / 256.0f) - l / 2000.0f);
#endif


#if defined(DIV) || defined(AGE) || defined(NOVMAP)
		this->_objs.push_back(0);
#endif

#ifdef RANDOBJ
		this->_objs.push_back(ind.gen().rand_obj());
#endif

		if (this->mode() == sferes::fit::mode::view) {
			std::cout << "behavior:" << _behavior << std::endl;
			std::ofstream ofs("/tmp/nn.dot");
			ind.nn().write(ofs);
			std::cout << "- AND: " << 1 - err1 / 256.0f << std::endl;
			std::cout << "- OR: " << 1 - err2 / 256.0f << std::endl;
			std::cout << "- MOD: " << _mod << std::endl;
			std::cout << "- length: " << _length << std::endl;
			std::cout << "- nb neurons: " << ind.nn().get_nb_neurons() << std::endl;
			std::cout << "- nb conns: " << ind.nn().get_nb_connections() << std::endl;

			// dump in nn.svg
			std::ofstream ofs2("nn.svg");
#ifdef HNN
			//    ind.write_svg(ofs2);
#else
			ind.gen().write_svg(ofs2);
#endif
			// dump in xml
			typedef boost::archive::xml_oarchive oa_t;
			std::ofstream ofs_xml("nn.xml");
			oa_t oa(ofs_xml);
			oa << boost::serialization::make_nvp("phen", ind);
			// analysis
			analysis_left_right(ind);
			analysis_io_mod(ind);
			analysis_dyn_mod(ind);
		}
	}
	float values(size_t k) const { return _values[k]; }
	size_t nb_values() const { return _values.size(); }
	void set_value(float v) { this->_value = v; }
	const std::bitset<b_size>& behavior() const { return _behavior; }
	float mod() const { return _mod; }
	float length() const { return _length; }
	float optlength() const { return _optlength; }
	int nb_nodes() const { return _nb_nodes; }
	int nb_conns() const { return _nb_conns; }
protected:
	std::vector<float> _values;
	std::vector<size_t> _left_objects;
	std::vector<size_t> _right_objects;
	std::vector<size_t> _rand_objects;
	std::bitset<b_size> _behavior;
	int _age;

	float _mod, _mod2;
	float _length, _optlength;
	int _nb_nodes, _nb_conns;
	template<typename I>
	void analysis_left_right(I& indiv) {
		float best_right = 0, best_left = 0;
		for (size_t i = 0; i < 4; ++i)
			_analysis_left_right_cases(i, indiv, &best_left, &best_right);
		std::cout << "- BLN: " << best_left / 256.0f << std::endl
		        << "- BRN: " << best_right / 256.0f << std::endl;
		std::cout << "- A: "
		        << (best_left + best_right) / 512.0f
		        << std::endl;
	}
	template<typename I>
	void _analysis_left_right_cases(int c, I& indiv,
	        float*best_left, float *best_right) {
		assert(c < 4);
		std::vector<int> left_score(indiv.nn().get_nb_neurons());
		std::vector<int> right_score(indiv.nn().get_nb_neurons());
		std::fill(left_score.begin(), left_score.end(), 0);
		std::fill(right_score.begin(), right_score.end(), 0);

		std::vector<float> inputs(8);
		for (size_t s = 0; s < 256; ++s) {
			std::bitset<8> in(s);
			for (size_t i = 0; i < in.size(); ++i)
				inputs[i] = in[i] ? 1 : -1;
			bool left = std::find(_left_objects.begin(),
			        _left_objects.end(),
			        (s & 0xF))
			        != _left_objects.end();
			bool right = std::find(_right_objects.begin(),
			        _right_objects.end(),
			        (s & 0xF0))
			        != _right_objects.end();
			// step
#ifndef HNN
			for (size_t i = 0; i < indiv.gen().get_depth(); ++i)
				indiv.nn().step(inputs);
#else
			for (size_t i = 0; i < indiv.get_depth(); ++i)
				indiv.nn().step(inputs);
#endif
			size_t k = 0;
			BGL_FORALL_VERTICES_T(v, indiv.nn().get_graph(), typename I::nn_t::graph_t) {
				bool x = false;
				if (c == 0)
					x = indiv.nn().get_graph()[v].get_current_output() > 0;
				else if (c == 1)
					x = indiv.nn().get_graph()[v].get_current_output() >= 0;
				else if (c == 2)
					x = indiv.nn().get_graph()[v].get_current_output() < 0;
				else if (c == 3)
					x = indiv.nn().get_graph()[v].get_current_output() <= 0;

				if (left && x)
					left_score[k]++;
				if (!left && !x)
					left_score[k]++;
				if (right && x)
					right_score[k]++;
				if (!right && !x)
					right_score[k]++;

				++k;
			}
		}
		size_t k = 0;
		float best_left_score = -1e10, best_right_score = -1e10;
		BGL_FORALL_VERTICES_T(v, indiv.nn().get_graph(), typename I::nn_t::graph_t) {
			if (left_score[k] > best_left_score)
				best_left_score = left_score[k];
			if (right_score[k] > best_right_score)
				best_right_score = right_score[k];
			++k;
		}
		*best_left = std::max(best_left_score, *best_left);
		*best_right = std::max(best_right_score, *best_right);
	}
	template<typename I>
	void analysis_io_mod(I& indiv) {
		std::vector<std::set<typename I::nn_t::graph_t::vertex_descriptor> > mods;
#ifndef HNN
		typename I::nn_t nn = indiv.gen().simplified_nn();
#else
		typename I::nn_t nn = indiv.simplified_nn();
#endif
		float q = mod2::split(nn.get_graph(), mods);
		assert(mods.size() == 2);
		using namespace boost::assign;
		std::set<int> left = list_of(0)(1)(2)(3);
		std::set<int> right = list_of(4)(5)(6)(7);
		float s = 0;
		for (size_t i = 0; i < 2; ++i) {
			int m_left = 0, m_right = 0;
			BOOST_FOREACH(typename I::nn_t::graph_t::vertex_descriptor v, mods[i]) {
				//          std::cout << nn.get_graph()[v].get_in() << std::endl;
				if (left.find(nn.get_graph()[v].get_in()) != left.end())
					m_left++;
				if (right.find(nn.get_graph()[v].get_in()) != right.end())
					m_right++;
			}
			s += abs(m_left - m_right);
		}
#ifndef HNN
		float qs = compute_mod_simplified(indiv.gen());
#else
		float qs = compute_mod_simplified(indiv.nn());
#endif
		std::cout << "- IOM: " << s << std::endl;
		std::cout << "- Q2: " << q << std::endl;
		std::cout << "- QS: " << qs << std::endl;

	}

	template<typename I>
	void analysis_dyn_mod(I& indiv) {
		std::vector<float> inputs(8);
		std::vector<std::vector<bool> > changed(2);
		changed[0].resize(indiv.nn().get_nb_neurons());
		changed[1].resize(indiv.nn().get_nb_neurons());
		for (size_t c = 0; c < changed.size(); ++c) {
			std::fill(changed[c].begin(), changed[c].end(), false);
			for (size_t i = 0; i < 16; ++i) {
				std::bitset<8> obj(i);
				std::vector<bool> prev_state(indiv.nn().get_nb_neurons());
				for (size_t s = 0; s < 16; ++s) {
					std::bitset<8> in(s);
					if (c == 0)
						in = (in << 4) | obj;
					else
						in = (obj << 4) | in;
					for (size_t j = 0; j < in.size(); ++j)
						inputs[j] = in[j] ? 1 : -1;
#ifndef HNN
					for (size_t j = 0; j < indiv.gen().get_depth(); ++j)
						indiv.nn().step(inputs);
#else
					for (size_t j = 0; j < indiv.get_depth(); ++j)
						indiv.nn().step(inputs);
#endif
					size_t k = 0;
					std::vector<bool> state(indiv.nn().get_nb_neurons());
					BGL_FORALL_VERTICES_T(v, indiv.nn().get_graph(),
					        typename I::nn_t::graph_t)
					state[k++] = indiv.nn().get_graph()[v].get_current_output() > 0;
					if (s != 0)
						for (size_t j = 0; j < state.size(); ++j)
							if (state[j] != prev_state[j])
								changed[c][j] = true;
					prev_state = state;
				}
			}
		}

		float diffs = 0.0f;
		for (size_t j = 0; j < changed[0].size(); ++j)
			if (changed[0][j] != changed[1][j])
				diffs += 1;
		float score = diffs / changed[0].size();
		std::cout << "- DM: " << score << std::endl;
	}


	void _create_objects() {
// #ifdef ROBJECTS
//       typedef boost::mt19937 base_generator_t;
//       typedef boost::variate_generator<base_generator_t, boost::uniform_int<> > rand_t;
//       rand_t rand_gen(base_generator_t(42u), boost::uniform_int<>(0, 255));
//       for (size_t i = 0; i < 8; ++i)
// 	_left_objects.push_back(rand_gen());
//       for (size_t i = 0; i < 8; ++i)
// 	_right_objects.push_back(rand_gen());

// #else
		_left_objects.push_back(15);
		_left_objects.push_back(7);
		_left_objects.push_back(5);
		_left_objects.push_back(13);
		_left_objects.push_back(1);
		_left_objects.push_back(11);
		_left_objects.push_back(4);
		_left_objects.push_back(14);
		//
		_right_objects.push_back(112);
		_right_objects.push_back(240);
		_right_objects.push_back(208);
		_right_objects.push_back(160);
		_right_objects.push_back(176);
		_right_objects.push_back(32);
		_right_objects.push_back(224);
		_right_objects.push_back(128);
		//#endif

	}
	void _seasons(bool left, bool right, float out, std::vector<float>& errs) {
		assert(errs.size() == 4);
		if (left && right)
			errs[0] += (out > 0 ? 0 : 1);
		else
			errs[0] += (out < 0 ? 0 : 1);

		if (left || right)
			errs[1] += (out > 0 ? 0 : 1);
		else
			errs[1] += (out < 0 ? 0 : 1);

		if (left)
			errs[2] += (out > 0 ? 0 : 1);
		else
			errs[2] += (out < 0 ? 0 : 1);

		if (right)
			errs[3] += (out > 0 ? 0 : 1);
		else
			errs[3] += (out < 0 ? 0 : 1);

	}
};

namespace sferes {
namespace stat {
SFERES_CLASS(RetinaStop) {
public:
	template<typename E>
	void refresh(const E& ea) const {
		if (stop_criterion(ea)) {
			std::cout << "Stop criterion reached" << std::endl;
			ea.write();
			exit(0);
		}
	}
	void show(std::ostream& os, size_t k)
	{}
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{}

	template<typename EA>
	bool stop_criterion(const EA& ea) const {
		for (size_t i = 0; i < ea.pop().size(); ++i)
			if (ea.pop()[i]->fit().value() == 1.0)
				return true;
		return false;

	}
};
}
}


long Params::seed = -1;

int main(int argc, char **argv) {
	typedef FitRetina<Params> fit_t;
#ifdef HNN
	typedef phen::Parameters<gen::EvoFloat<1, ParamsHnn::cppn>,
	        fit::FitDummy<>, ParamsHnn::cppn> weight_t;
	typedef gen::Hnn<weight_t, ParamsHnn, ParamsHnn::cppn> gen_t;
	typedef phen::Hnn<gen_t, fit_t, ParamsHnn> phen_t;
#else
#ifdef FLOAT
	typedef phen::Parameters<gen::EvoFloat<1, Params::dnn::weights>,
	        fit::FitDummy<>, Params> weight_t;
	typedef phen::Parameters<gen::EvoFloat<1, Params::dnn::bias>,
	        fit::FitDummy<>, Params> bias_t;
#else
	typedef phen::Parameters<gen::Sampled<1, Params::dnn::weights>,
	        fit::FitDummy<>, Params> weight_t;
	typedef phen::Parameters<gen::Sampled<1, Params::dnn::bias>,
	        fit::FitDummy<>, Params> bias_t;

#endif
	typedef PfWSum<weight_t> pf_t;
	typedef AfRetina<bias_t> af_t;
	typedef sferes::gen::DnnSpatial<Neuron<pf_t, af_t>, Connection<weight_t>, Params> gen_t;
	typedef phen::Dnn<gen_t, fit_t, Params> phen_t;
#endif
	typedef eval::Parallel<Params> eval_t;
	typedef boost::fusion::vector
	<
	//    stat::ParetoFront<phen_t, Params>, // comment this line to remove pareto.dat
	stat::Mod<phen_t, Params>
#ifdef LOGALL
	, stat::LogAll<phen_t, Params>
#endif
#ifdef STOP
	, stat::RetinaStop<Params>
#endif
	>  stat_t;

	// MVG cannot be used with DIV !
#if defined(DIV)and !defined(MVG)
	typedef modif::BehaviorDiv<> modifier_t;
#elif defined(MVG)
	typedef modif::Mvg<> modifier_t;
#elif defined(AGE)
	typedef modif::Age<> modifier_t;
#elif defined(NOVMAP)
	typedef modif::Novmap<> modifier_t;
#else
	typedef modif::Dummy<> modifier_t;
#endif

#ifdef PNSGA
	typedef ea::Pnsga<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
#else
	typedef ea::Nsga2<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
#endif
	ea_t ea;

	if (argc == 2) { // if a number is provided on the command line
		int randomSeed = atoi(argv[1]);
		printf("randomSeed:%i\n", randomSeed);
		srand(randomSeed);  //set it as the random seed
		Params::seed = randomSeed;
		run_ea(argc, argv, ea, false);
	} else if (argc == 3) { // number + file
		int randomSeed = atoi(argv[1]);
		printf("randomSeed:%i\n", randomSeed);
		srand(randomSeed);  //set it as the random seed
		Params::seed = randomSeed;
#ifdef PNSGA
		ea.load_and_run(std::string(argv[2]));
#else
		run_ea(argc, argv, ea, false);
#endif
	} else {
#ifdef TESTSUB
		srand(atoi(argv[1]));  //set it as the random seed
		run_ea(argc, argv, ea, false);
#else
		run_ea(argc, argv, ea, true);
#endif

	}


	return 0;
}
