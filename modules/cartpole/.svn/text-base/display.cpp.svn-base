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
** sdl.cc
** Login : <mouret@mexico>
** Started on  Sat Jan 12 20:40:12 2008 Jeanbaptiste MOURET
** $Id$
** 
** Copyright (C) 2008 Jeanbaptiste MOURET
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
#include "display.hpp"

namespace sdl
{

  void Display :: _events()
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
      {
	switch (event.type) 
	  {
	  case SDL_QUIT:
	    _quit();
	  case SDL_KEYDOWN:
	    if (event.key.keysym.sym == SDLK_ESCAPE)
	      _quit();
	    break;
	  }
      }
  }

  Display :: Display(int w, int h) : _w(w), _h(w) 
  {
    if (SDL_Init(SDL_INIT_VIDEO) == -1)
      throw Exception(SDL_GetError());

    _screen = SDL_SetVideoMode(_w, _h, 
			       32, SDL_SWSURFACE);
    if (!_screen)
      throw Exception(SDL_GetError());
  }
  
  void Display :: _line(SDL_Surface* surf,
			int x0, int y0, int x1, int y1, Uint32 color)
  {
    int dy = y1 - y0;
    int dx = x1 - x0;
    int stepx, stepy;

    if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
    if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
    dy <<= 1; // dy is now 2*dy
    dx <<= 1; // dx is now 2*dx

    _put_pixel(surf, color, x0, y0);
    if (dx > dy) 
      {
	int fraction = dy - (dx >> 1); // same as 2*dy - dx
	while (x0 != x1)
	  {
	    if (fraction >= 0)
	      {
		y0 += stepy;
		fraction -= dx; // -= 2*dx
	      }
	    x0 += stepx;
	    fraction += dy; //  -= 2*dy
	    _put_pixel(surf, color, x0, y0);
	  }
      } 
    else 
      {
	int fraction = dx - (dy >> 1);
	while (y0 != y1)
	  {
	    if (fraction >= 0)
	      {
		x0 += stepx;
		fraction -= dy;
	      }
	    y0 += stepy;
	    fraction += dx;
	    _put_pixel(surf, color, x0, y0);
	  }
      }
  }

  void Display :: _circle_points(SDL_Surface* surf,
				 int cx, int cy, int x, int y, Uint32 color)
  {

    if (x == 0) {
      _put_pixel(surf, color, cx, cy + y);
      _put_pixel(surf, color, cx, cy - y);
      _put_pixel(surf, color, cx + y, cy);
      _put_pixel(surf, color, cx - y, cy);
    }
    else if (x == y) {
      _put_pixel(surf, color, cx + x, cy + y);
      _put_pixel(surf, color, cx - x, cy + y);
      _put_pixel(surf, color, cx + x, cy - y);
      _put_pixel(surf, color, cx - x, cy - y);
    } else if (x < y) {
      _put_pixel(surf, color, cx + x, cy + y);
      _put_pixel(surf, color, cx - x, cy + y);
      _put_pixel(surf, color, cx + x, cy - y);
      _put_pixel(surf, color, cx - x, cy - y);
      _put_pixel(surf, color, cx + y, cy + x);
      _put_pixel(surf, color, cx - y, cy + x);
      _put_pixel(surf, color, cx + y, cy - x);
      _put_pixel(surf, color, cx - y, cy - x);
    }
  }

  void Display :: _circle(SDL_Surface *surf,
			  int x_center, int y_center, int radius,
			  Uint8 r, Uint8 g, Uint8 b)
  {
    
    Uint32 color = SDL_MapRGB(_screen->format, r, g, b);
    int x = 0;
    int y = radius;
    int p = (5 - radius * 4) / 4;
    
    _circle_points(surf, x_center, y_center, x, y, color);
    while (x < y) 
      {
	x++;
	if (p < 0) 
	  p += 2*x+1;
	else
	  {
	    y--;
	    p += 2 * (x - y) + 1;
	  }
	_circle_points(surf, x_center, y_center, x, y, color);
      }
  }


  void Display::update()
  {
    _events();
    SDL_Flip(_screen);
  }

}

