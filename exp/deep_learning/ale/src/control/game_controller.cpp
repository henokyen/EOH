/* *****************************************************************************
 * A.L.E (Arcade Learning Environment)
 * Copyright (c) 2009-2012 by Yavar Naddaf, Joel Veness, Marc G. Bellemare and 
 *   the Reinforcement Learning and Artificial Intelligence Laboratory
 * Released under the GNU General Public License; see License.txt for details. 
 *
 * Based on: Stella  --  "An Atari 2600 VCS Emulator"
 * Copyright (c) 1995-2007 by Bradford W. Mott and the Stella team
 *
 * *****************************************************************************
 *  game_controller.cpp
 *
 *  The implementation of the GameController class, which is the superclass for
 *  other controller classes. A controller class sits between Stella, and 
 *  whatever mechanism we are using to control Stella, i.e. FIFO pipes and
 *  external code, or internal player_agent classes.
 **************************************************************************** */

#include "game_controller.h"
#include "Roms.hpp"


/* *********************************************************************
   Constructor
   ******************************************************************** */
GameController::GameController(OSystem* _osystem): state(_osystem) {
  p_osystem = _osystem;
  p_global_event_obj = p_osystem->event();
  p_console = &(p_osystem->console());
  MediaSource& mediasrc = p_console->mediaSource();
  p_emulator_system = &(p_console->system());
  i_screen_width  = mediasrc.width();
  i_screen_height = mediasrc.height();
  b_send_screen_matrix    = true;
  b_send_console_ram      = true;
  i_skip_frames_num       = 0;
  i_skip_frames_counter   = 0;
  e_previous_a_action     = PLAYER_A_NOOP;
  e_previous_b_action     = PLAYER_B_NOOP;

  // load the RL wrapper for the ROM
  string rom_file = p_osystem->settings().getString("rom_file");
  m_rom_settings = buildRomRLWrapper(rom_file);
  if (m_rom_settings == NULL) {
    std::cerr << "Unsupported ROM file" << std::endl;
    exit(1);
  }

  state.setSettings(m_rom_settings);
  // MGB
  p_num_system_reset_steps = 2;//atoi(_osystem->settings().getString("system_reset_steps").c_str());

  for (int i = 0; i < i_screen_height; i++) {
    IntVect row;
    for (int j = 0; j < i_screen_width; j++)
      row.push_back(-1);    
    _screen_matrix.push_back(row);
  }


  // Initialize our copy of frame_buffer
    pi_old_frame_buffer = new uInt32[i_screen_width * i_screen_height];
    // this frame buffer contains the phosphor blended frame 
    pi_curr_frame_buffer = new uInt32[i_screen_width * i_screen_height];
    for (int i = 0; i < i_screen_width * i_screen_height; i++) {
        pi_old_frame_buffer[i] = -1;
    }

    // MGB @phosphor taken from default Stella settings
    phosphor_blend_ratio   = 77;


  systemReset();

}

void GameController :: update()
{
    static bool first_step = true;
    if (first_step) {
        makeAveragePalette();
        first_step = false;
    }
    copy_framebuffer();
}
        
/* *********************************************************************
    Destructor
 ******************************************************************** */
GameController::~GameController() {
    if (m_rom_settings) {
        delete m_rom_settings;
        m_rom_settings = NULL;
    }
    delete [] pi_old_frame_buffer;
    delete [] pi_curr_frame_buffer;
}

void GameController::saveState() {
  state.save();
}

void GameController::loadState() {
  state.load();
}

/* reset the interface and emulator */
void GameController::systemReset() {
  state.reset(p_num_system_reset_steps);
}

/* ***************************************************************************
 *  Function read_ram
 *  Reads a byte from console ram
 *  
 *  The code is mainly based on RamDebug.cxx
 * ***************************************************************************/
int GameController::read_ram(int offset) {
    offset &= 0x7f; // there are only 128 bytes
    return p_emulator_system->peek(offset + 0x80);
}


// JBM
void GameController::copy_framebuffer() {
  phosphorBlend();
  int ind_i, ind_j;
  for (int i = 0; i < i_screen_width * i_screen_height; i++) {
    uInt8 v = pi_curr_frame_buffer[i];
    ind_i = i / i_screen_width;
    ind_j = i - (ind_i * i_screen_width);
    _screen_matrix[ind_i][ind_j] = v;
  }
}



// JBM
void GameController :: export_screen() const {
  char buffer [256] = { 0 };
  sprintf (buffer, "exported_screens/%09d.png", state.getFrameNumber());
  p_osystem->p_export_screen->save_png(&_screen_matrix, buffer);
}



// MGB @phosphor
void GameController::phosphorBlend() {
  uInt8 * current_buffer = p_console->mediaSource().currentFrameBuffer();
  uInt8 * previous_buffer = p_console->mediaSource().previousFrameBuffer();

  for (int i = 0; i < i_screen_width * i_screen_height; i++) {
    int cv = current_buffer[i];
    int pv = previous_buffer[i];
    
    // Find out the corresponding rgb color 
    uInt32 rgb = my_avg_palette[cv][pv];
    int r1, g1, b1;

    pi_curr_frame_buffer[i] = rgbToNTSC(rgb);
  }
}

// MGB @phosphor
void GameController::makeAveragePalette() {

  ExportScreen* es = p_osystem->p_export_screen;

  // Precompute the average RGB values for phosphor-averaged colors c1 and c2
  for (int c1 = 0; c1 < 256; c1++) {
    for (int c2 = 0; c2 < 256; c2++) {
      int r1, g1, b1;
      int r2, g2, b2;
      es->get_rgb_from_palette(c1, r1, g1, b1);
      es->get_rgb_from_palette(c2, r2, g2, b2);

      uInt8 r = getPhosphor(r1, r2);
      uInt8 g = getPhosphor(g1, g2);
      uInt8 b = getPhosphor(b1, b2);
      my_avg_palette[c1][c2] = makeRGB(r, g, b);
    }
  }
  
  // Also make a RGB to NTSC color map
  for (int r = 0; r < 256; r += 4) {
    for (int g = 0; g < 256; g += 4) {  
      for (int b = 0; b < 256; b += 4) {
        // For each RGB point, we find its closest NTSC match
        int minDist = 256 * 3 + 1;
        int minIndex = -1;

        for (int c1 = 0; c1 < 256; c1++) {
          // Get the RGB corresponding to c1
          int r1, g1, b1;
          es->get_rgb_from_palette(c1, r1, g1, b1);

          int dist = abs(r1 - r) + abs(g1 - g) + abs(b1 - b);
          if (dist < minDist) {
            minDist = dist;
            minIndex = c1;
          }
        }

        rgb_ntsc[r >> 2][g >> 2][b >> 2] = minIndex;
      }
    }
  }
}

// MGB @phopshor
/** Compute the phosphor blend of two color components */ 
uInt8 GameController::getPhosphor(uInt8 v1, uInt8 v2) {
  if (v1 < v2) {
    int tmp = v1;
    v1 = v2;
    v2 = tmp;
  }

  uInt32 blendedValue = ((v1 - v2) * phosphor_blend_ratio) / 100 + v2;
  if (blendedValue > 255) return 255;
  else return (uInt8) blendedValue;
}


uInt32 GameController::makeRGB(uInt8 r, uInt8 g, uInt8 b) {
  return (r << 16) | (g << 8) | b;
}


uInt8 GameController::rgbToNTSC(uInt32 rgb) {
  int r = (rgb >> 16) & 0xFF;
  int g = (rgb >> 8) & 0xFF;
  int b = rgb & 0xFF;

  return rgb_ntsc[r >> 2][g >> 2][b >> 2];
}
