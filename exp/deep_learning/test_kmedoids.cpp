#include "kmedoids.hpp"

struct Point
{
  Point(float xx, float yy) : x(xx), y(yy) {}
  float x, y;
};

float dist(const Point& p1, const Point &p2)
{
  float x = p1.x - p2.x;
  float y = p1.y - p2.y;
  return sqrtf(x * x + y * y);
}


template<typename T> 
inline T rand(T max = 1.0)
{ 
  assert(max > 0);
  T v;
  do
    v = T(((double)max * ::rand())/(RAND_MAX + 1.0));
  while(v >= max); // this strange case happened... precision problem?
  assert(v < max);
  return v;
}

template<typename T>
inline T rand(T min, T max)
{
  assert(max != min);
  assert(max > min);
  T res = T(rand<double>() * ((long int) max - (long int) min) + min);
  assert(res >= min);
  assert(res < max);
  return res;
}


void gen(std::vector<Point>& data, const Point& p, int k)
{
  for (size_t i = 0; i < k; ++i)
    data.push_back(Point(p.x + rand<float>(-1.5f, 1.5f),
			 p.y + rand<float>(-1.5f, 1.5f)));
}
int main()
{
  srand(time(NULL));
  std::vector<Point> data;
  gen(data, Point(10, 10), 30);
  gen(data, Point(1, 1), 50);
  gen(data, Point(10, 5), 70);
  gen(data, Point(0, 8), 34);


  kmedoids::clusters_t r = kmedoids::kmedoids(data, dist, 80);
  for (size_t i = 0; i < data.size(); ++i)
    std::cout << data[i].x << " " << data[i].y << " " << r.second[i] << std::endl;
  for (size_t i = 0; i < r.first.size(); ++i)
    std::cout << data[r.first[i]].x << " " << data[r.first[i]].y << " " << 500 << std::endl;

  return 0;
}
