#ifndef _ARM_ENV_HPP_
#define _ARM_ENV_HPP_

#include "arm.hpp"

class ArmEnv
{
  public:
    static const int LEFT_SMALL = 0;
    static const int LEFT_BIG = 1;
    static const int RIGHT_SMALL = 2;
    static const int RIGHT_BIG = 3;

    static const float a45 = 45.0 / 180 * M_PI;
    static const float a15 = 15.0 / 180 * M_PI;

    int ball_size() const { return _env == LEFT_BIG || _env == RIGHT_BIG; }
    int ball_side() const { return _env == LEFT_BIG || _env == LEFT_SMALL; }
    const Eigen::Vector2f& ball_pos() const { return _ball_pos; }
    float ball_radius() const { return _ball_radius; }

    ArmEnv() :
      _arm(3, 2, Eigen::Vector2f(0, 0),
           boost::assign::list_of(100) (100) (100) (50.0) (50) (50) (50))
    {}
    void set_env(int env)
    {
      _env = env;
      switch (env)
      {
      case LEFT_BIG:
      {
        Eigen::Vector2f d1 = Eigen::Vector2f(-1.0 - sqrt(2) / 2.0, -1.0 - sqrt(2) / 2.0) * 50;
        Eigen::Vector2f c1 = Eigen::Vector2f(-1.0 - sqrt(2), 0.0) * 100.0 + d1;
        _ball_radius = d1.norm();
        _ball_pos = c1;
      }
      break;
      case LEFT_SMALL:
      {
        Eigen::Vector2f p2_1 =
          Eigen::Vector2f(cos(M_PI / 2 + a45 * 2 + a15 * 2),
                          sin(M_PI / 2 + a45 * 2 + a15 * 2)) * 50
          + Eigen::Vector2f(cos(M_PI / 2 + a45 * 2 + a15),
                            sin(M_PI / 2 + a45 * 2 + a15)) * 50;
        float r = p2_1.norm() / (2 * sin((M_PI - M_PI / 4) / 2));
        Eigen::Vector2f d2 = Eigen::Vector2f(cos(M_PI + a45),
                                             sin(M_PI + a45)) * r;
        Eigen::Vector2f c2 = Eigen::Vector2f(-1.0 - sqrt(2), 0.0) * 100.0;
        _ball_pos = c2 + d2;
        _ball_radius = r;
      }
      break;
      case RIGHT_BIG:
      {
        Eigen::Vector2f d1 = Eigen::Vector2f(1.0 + sqrt(2) / 2.0,
                                             -1.0 - sqrt(2) / 2.0) * 50;
        Eigen::Vector2f c1 = Eigen::Vector2f(1.0 + sqrt(2), 0.0) * 100.0 + d1;
        _ball_radius = d1.norm();
        _ball_pos = c1;
      }
      break;
      case RIGHT_SMALL:
      {
        Eigen::Vector2f p2_1 =
          Eigen::Vector2f(cos(M_PI / 2 + a45 * 2 + a15 * 2),
                          sin(M_PI / 2 + a45 * 2 + a15 * 2)) * 50
          + Eigen::Vector2f(cos(M_PI / 2 + a45 * 2 + a15),
                            sin(M_PI / 2 + a45 * 2 + a15)) * 50;
        float r = p2_1.norm() / (2 * sin((M_PI - M_PI / 4) / 2));
        Eigen::Vector2f d2 = Eigen::Vector2f(cos(M_PI - a45),
                                             sin(M_PI - a45)) * r;
        Eigen::Vector2f c2 = Eigen::Vector2f(1.0 + sqrt(2), 0.0) * 100.0;
        _ball_pos = c2 - d2;
        _ball_radius = r;
      }
      break;
      default:
        assert(0);
      }
    }

    Arm& arm() { return _arm; }
    void save(const std::string& fname) { _arm.save(fname, _ball_pos, _ball_radius); }
  protected:
    Arm _arm;
    Eigen::Vector2f _ball_pos;
    float _ball_radius;
    int _env;
};
#endif
