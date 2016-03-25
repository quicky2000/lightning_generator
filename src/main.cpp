/*    This file is part of fractal
      Copyright (C) 2016  Julien Thevenon ( julien_thevenon at yahoo.fr )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "simple_gui.h"
#include "quicky_exception.h"
#include "parameter_manager.h"
#include <thread>
#include <iostream>
#include <chrono>
#include <cmath>
#include <random>

using namespace parameter_manager;

//------------------------------------------------------------------------------
int main(int argc,char ** argv)
{
  try
    {
      // Defining application command line parameters
      parameter_manager::parameter_manager l_param_manager("lightning_generator.exe","--",0);
      parameter_if l_size_param("size",true);

      l_param_manager.add(l_size_param);

      // Treating parameters
      l_param_manager.treat_parameters(argc,argv);
      unsigned int l_size = l_size_param.value_set() ? l_size_param.get_value<uint32_t>() : 1000;
      if(!(l_size % 2))
	{
	  ++l_size;
	}

      simple_gui::simple_gui l_gui;
      l_gui.create_window(l_size,l_size);

      unsigned int l_center_x = (l_size - 1) / 2;
      unsigned int l_center_y = l_center_x;
      unsigned int l_radix = (l_size / 2 ) * 0.95;

      l_gui.refresh();

      uint32_t l_bg_color_code = l_gui.get_color_code(0x0,0x0,0x0);
      uint32_t l_fg_color_code = l_gui.get_color_code(0xFF,0xFF,0xFF);
      
      // Draw background and foreground
      for(unsigned int l_x = 0 ; l_x < l_size ; ++l_x)
	{
	  for(unsigned int l_y = 0 ; l_y < l_size ; ++l_y)
	    {
	      unsigned int l_dist = (pow(((int)l_x) - ((int)l_center_x),2) + pow(((int)l_y) - ((int)l_center_y),2));
	      if(l_dist < pow(l_radix,2))
		{
		  l_gui.set_pixel(l_x,l_y,l_bg_color_code);
		}
	      else
		{
		  l_gui.set_pixel(l_x,l_y,l_fg_color_code);
		}
	    }
	}
      l_gui.refresh();
      std::this_thread::sleep_for(std::chrono::duration<int>(3));

      unsigned int l_x = l_center_x;
      unsigned int l_y = l_center_y;

      // Seed with a real random value, if available
      std::random_device l_random_device;
      std::uniform_int_distribution<int> l_uniform_dist(0, 7);
      std::seed_seq seed2{l_random_device(), l_random_device(), l_random_device(), l_random_device(), l_random_device(), l_random_device(), l_random_device(), l_random_device()}; 
      std::mt19937 l_random_engine(seed2);

      unsigned int l_dist[8] = {0,0,0,0,0,0,0,0};
      bool l_stuck = false;
      while(!l_stuck)
	{
	  l_stuck = (l_fg_color_code == l_gui.get_pixel(l_x - 1,l_y - 1) ||
		       l_fg_color_code == l_gui.get_pixel(l_x,l_y - 1) || 
		       l_fg_color_code == l_gui.get_pixel(l_x + 1 ,l_y - 1) || 
		       l_fg_color_code == l_gui.get_pixel(l_x - 1,l_y) ||
		       l_fg_color_code == l_gui.get_pixel(l_x + 1 ,l_y) || 
		       l_fg_color_code == l_gui.get_pixel(l_x - 1,l_y + 1) ||
		       l_fg_color_code == l_gui.get_pixel(l_x,l_y + 1) || 
		       l_fg_color_code == l_gui.get_pixel(l_x + 1 ,l_y + 1)
		       );

	  //	  l_gui.set_pixel(l_x,l_y,l_fg_color_code);
	  //	  l_gui.refresh();
	  if(!l_stuck)
	    {
	      //	      l_gui.set_pixel(l_x,l_y,l_bg_color_code);
	      //	      l_gui.refresh();
	      // Choose a random mean between 0 and 7
	      int l_rand = l_uniform_dist(l_random_engine);
	      ++l_dist[l_rand];
	      switch(l_rand)
		{
		case 0:
		  ++l_x;
		  break;
		case 1:
		  --l_x;
		  break;
		case 2:
		  ++l_y;
		  break;
		case 3:
		  --l_y;
		  break;
		case 4:
		  ++l_x;
		  ++l_y;
		  break;
		case 5:
		  --l_x;
		  --l_y;
		  break;
		case 6:
		  --l_x;
		  ++l_y;
		  break;
		case 7:
		  ++l_x;
		  --l_y;
		  break;
		default:
		  throw quicky_exception::quicky_logic_exception("Unexpected rand value : " + std::to_string(l_rand),__LINE__,__FILE__);
		}
	    }
	  else if(l_x != l_center_x || l_y != l_center_y)
	    {
	      l_gui.set_pixel(l_x,l_y,l_fg_color_code);
	      l_gui.refresh();
	      l_stuck = false;
	      l_x = l_center_x;
	      l_y = l_center_y;
	    }
	}

      for(unsigned int l_index = 0 ; l_index < 8 ; ++l_index)
	{
	  std::cout << "[" << l_index << "] : " << l_dist[l_index] << std::endl ; 
	}

      std::cout << "Finished" << std::endl ;
      std::this_thread::sleep_for(std::chrono::duration<int>(3));
    }
  catch(quicky_exception::quicky_runtime_exception & e)
    {
      std::cout << "ERROR : " << e.what() << std::endl ;
      return(-1);
    }
  catch(quicky_exception::quicky_logic_exception & e)
    {
      std::cout << "ERROR : " << e.what() << std::endl ;
      return(-1);
    }
  return 0;
}
//EOF
