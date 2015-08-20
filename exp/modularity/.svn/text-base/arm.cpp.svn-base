#include <iostream>
#include "arm.hpp"
#include "arm_env.hpp"

void Arm::save(std::ofstream& ofs, const Eigen::Vector2f& ball_pos, float ball_size) const
{
  float xoffset = 500;
  float yoffset = 500;

  ofs << "<?xml version=\"1.0\" standalone=\"no\"?>" << std::endl;
  ofs << "<svg width=\"1000\" height=\"1000\" version=\"1.1\""
      << " xmlns=\"http://www.w3.org/2000/svg\">" << std::endl;

  if (ball_size > 0)
    ofs << "<circle cx=\"" << ball_pos.x() + xoffset
        << "\" cy=\"" << -ball_pos.y() + yoffset
        << "\" r=\"" << ball_size
        << "\" stroke=\"black\" stroke-width=\"1\" fill=\"white\"/>"
        << std::endl;

  for (size_t i = 0; i < _pos.size() - 1; ++i)
  {
    if (i == _nb_dofs_arm + _nb_dofs_hand)
      continue;
    ofs << "<line x1=\"" << _pos[i].x() + xoffset
        << "\" y1=\"" << -_pos[i].y() + yoffset << "\" "
        << "x2=\"" << _pos[i + 1].x() + xoffset
        << "\" y2=\"" << -_pos[i + 1].y() + yoffset << "\""
        << " style=\"stroke:rgb("
        << "50,50,50"
        << ");stroke-width:" << 3
        << "\"/>"
        << std::endl;
  }

  for (size_t i = 0; i < _pos.size(); ++i)
    ofs << "<circle cx=\"" << _pos[i].x() + xoffset
        << "\" cy=\"" << -_pos[i].y() + yoffset
        << "\" r=\"5\" stroke=\"black\" stroke-width=\"1\" fill=\"white\"/>"
        << std::endl;
  ofs << "</svg>" << std::endl;
}

void Arm::_compute_pos()
{
  _pos[0] = _base_pos;
  for (size_t i = 1; i < _nb_dofs_arm + 1; ++i)
    _pos[i] = _pos[i - 1]
              + Eigen::Vector2f(cos(M_PI / 2 + _apos[i - 1]),
                                sin(M_PI / 2 + _apos[i - 1])) * _lengths[i - 1];
  size_t k = _nb_dofs_arm;
  _pos[k] = _pos[_nb_dofs_arm];
  for (size_t i = 1; i < _nb_dofs_hand + 1; ++i)
    _pos[i + k] = _pos[i - 1 + k]
                  + Eigen::Vector2f(cos(M_PI / 2 + _apos[i - 1 + k]),
                                    sin(M_PI / 2 + _apos[i - 1 + k])) *
                  _lengths[i + k - 1];

  k = _nb_dofs_arm + _nb_dofs_hand + 1;
  _pos[k] = _pos[_nb_dofs_arm];
  for (size_t i = 1; i < _nb_dofs_hand + 1; ++i)
    _pos[i + k] = _pos[i - 1 + k]
                  + Eigen::Vector2f(cos(M_PI / 2 + _apos[i - 2 + k]),
                                    sin(M_PI / 2 + _apos[i - 2 + k])) *
                  _lengths[i + k - 2];
}

void Arm::_compute_apos(const std::vector<float>& apos)
{
   // find the real angle
  _apos[0] = apos[0];
  for (size_t i = 1; i < _nb_dofs_arm; ++i)
    _apos[i] = apos[i] + _apos[i - 1];
  for (size_t i = 0; i < 2; ++i)
  {
    size_t k = i * _nb_dofs_hand + _nb_dofs_arm;
    _apos[k] = _apos[_nb_dofs_arm - 1] + apos[k];
    for (size_t j = 1; j < _nb_dofs_hand; ++j)
      _apos[k + j] = _apos[k + j - 1] + apos[k + j];
  }
}

#ifdef TEST_ARM
int main(int argc, char**argvs)
{
  float a45 = 45.0 / 180 * M_PI;
  float a15 = 15.0 / 180 * M_PI;

  std::vector<float> lengths = boost::assign::list_of(100) (100) (100) (50.0) (50) (50) (50);

  ArmEnv left_small(ArmEnv::LEFT_SMALL);
  left_small.arm().set_apos(boost::assign::list_of(a45) (a45) (a45) (a45) (a45) (-a45) (-a45));
  left_small.save("env1.svg");

  ArmEnv left_big(ArmEnv::LEFT_BIG);
  left_big.arm().set_apos(boost::assign::list_of(a45) (a45) (a45) (a15) (a15) (-a15) (-a15));
  left_big.save("env2.svg");


  ArmEnv right_big(ArmEnv::RIGHT_BIG);
  right_big.arm().set_apos(boost::assign::list_of(-a45) (-a45) (-a45) (a45) (a45) (-a45) (-a45));
  right_big.save("env3.svg");

  ArmEnv right_small(ArmEnv::RIGHT_SMALL);
  right_small.arm().set_apos(boost::assign::list_of(-a45) (-a45) (-a45) (a15) (a15) (-a15) (-a15));
  right_small.save("env4.svg");



  return 0;
}
#endif
