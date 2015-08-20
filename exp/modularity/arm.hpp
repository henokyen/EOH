#ifndef _ARM_HPP_
#define _ARM_HPP_

#include <vector>
#include <fstream>
#include <Eigen/Core>
#include <boost/assign/list_of.hpp> // for 'list_of()'
#include <boost/assert.hpp>

class Arm
{
  public:
    Arm(size_t nb_dofs_arm, size_t nb_dofs_hand,
        const Eigen::Vector2f & base_pos,
        const std::vector<float> lengths)
      : _nb_dofs_arm(nb_dofs_arm),
        _nb_dofs_hand(nb_dofs_hand),
        _base_pos(base_pos),
        _apos(nb_dofs_arm + nb_dofs_hand * 2),
        _lengths(lengths),
        _pos(nb_dofs_arm + nb_dofs_hand * 2 + 2)
    {
      assert(_lengths.size() + 2 == _pos.size());
    }

    void set_apos(const std::vector<float>& apos)
    {
      assert(_apos.size() == _nb_dofs_hand * 2 + _nb_dofs_arm);
      _compute_apos(apos);
      _compute_pos();
    }
    void save(const std::string& fname,
              const Eigen::Vector2f& ball_pos,
              float ball_size) const
    {
      std::ofstream ofs(fname.c_str());
      save(ofs, ball_pos, ball_size);
    }
    void save(std::ofstream& ofs,
              const Eigen::Vector2f& ball_pos,
              float ball_size) const;

    const std::vector<Eigen::Vector2f>& pos() const { return _pos; }

    const Eigen::Vector2f& finger1() const
    { return _pos[_nb_dofs_arm + _nb_dofs_hand]; }

    const Eigen::Vector2f& finger2() const
    { return _pos[_nb_dofs_arm + _nb_dofs_hand * 2 + 1]; }

  protected:
    size_t _nb_dofs_arm;
    size_t _nb_dofs_hand;
    Eigen::Vector2f _base_pos;
    std::vector<float> _apos;
    std::vector<float> _lengths;
    std::vector<Eigen::Vector2f> _pos;
     // ---
     // compute_apos must be called BEFORE
    void _compute_pos();
    void _compute_apos(const std::vector<float>& apos);
};
#endif
