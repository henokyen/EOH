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




/*
** sdl.hh
** Login : <mouret@asuncion.lip6.fr>
** Started on  Mon Jan 14 16:42:14 2008 Jean-Baptiste MOURET
** $Id$
** 
** Copyright (C) 2008 Jean-Baptiste MOURET
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef   	SDL_HH_
# define   	SDL_HH_

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <valarray>
#include <stdio.h>
#include <unistd.h>
#include <SDL.h>

namespace sdl
{
  class Exception
  {
  public:
    Exception(const std::string& msg) : _msg(msg) {}
    const std::string& msg() const { return _msg; }
  protected:
    std::string _msg;
  };
  class Display
  {

  public:
    Display(int w, int h);
    ~Display()
    {
      SDL_Quit();
    }
    void update();
    void fill_black() { SDL_FillRect(_screen, 0, 0); }
    void line(float x0, float y0, float x1, float y1,
	      Uint8 r, Uint8 g, Uint8 b)
    {
      line((int) roundf(x0),
	   (int) roundf(y0),
	   (int) roundf(x1),
	   (int) roundf(y1),
	   r, g, b);
    }
    void line(int x0, int y0,
	      int x1, int y1,
	      Uint8 r, Uint8 g, Uint8 b)
    {
      Uint32 color = SDL_MapRGB(_screen->format, r, g, b);
      _line(_screen, x0, y0, x1, y1, color);
    }    
    void circle(float x_center, float y_center, float radius,
		Uint8 r, Uint8 g, Uint8 b)
    {
      circle((int) roundf(x_center),
	     (int) roundf(y_center),
	     (int) roundf(radius),
	     r, g, b);
    }
    void circle(int x_center, int y_center, int radius,
		Uint8 r, Uint8 g, Uint8 b)
    {
      _circle(_screen, x_center, y_center, radius, r, g, b);
    }
    unsigned get_h() const { return _h; }
    unsigned get_w() const { return _w; }
  protected:
    void _events();

    void _quit()
    {
      SDL_Quit();
      exit(0);
    }
    void _put_pixel(SDL_Surface* surf,
		    Uint32 color, int x, int y)
    {
      if (x < 0) x = 0;
      if (x > _w) x = _w - 1;
      if (y < 0) y = 0;
      if (y > _h) y = _h - 1;
      Uint32 *bufp = (Uint32*)surf->pixels  + y * surf->pitch / 4 + x;
      *bufp = color;
    }
    void _put_pixel(SDL_Surface* surf,
		    unsigned x, unsigned y,
		    Uint8 r, Uint8 g, Uint8 b)
    {
      Uint32 color = SDL_MapRGB(surf->format, r, g, b);
      _put_pixel(surf, color, x, y);
    }
    void _circle_points(SDL_Surface* surf,
			int cx, int cy, int x, int y, Uint32 color);
    void _circle(SDL_Surface* surf,
		 int x_center, int y_center, int radius,
		 Uint8 r, Uint8 g, Uint8 b);
    void _line(SDL_Surface* surf, int x0, int y0, int x1, int y1, Uint32 color);
    // 
    int _w, _h;
    SDL_Surface* _screen;
  };

}
#endif	    /* !SDL_HH_ */
