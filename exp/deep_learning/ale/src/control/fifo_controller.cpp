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
 *  fifo_controller.cpp
 *
 * The implementation of the FIFOController class, which is a subclass of
 * GameController, and is resposible for sending the Screens/RAM content to
 * whatever external program we are using through FIFO pipes, and apply the
 * actions that are sent back
 **************************************************************************** */

#include <string.h>

#include "fifo_controller.h"
#include "Serializer.hxx"
#include "Deserializer.hxx"

#include "Roms.hpp"
#include "RomSettings.hpp"

#define MAX_RUN_LENGTH (0xFF)

static const char hexval[] = { 
    '0', '1', '2', '3', '4', '5', '6', '7', 
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' 
};

/* appends a pixels value to the string buffer, returning the number of characters written */
inline void appendByte(char *buf, uInt8 v) {
    *buf++ = hexval[(v >> 4)];
    *buf++ = hexval[v & 0xF];
}

/* interface constructor */
FIFOController::FIFOController(OSystem* _osystem, bool named_pipes) :
    GameController(_osystem) {
    // for backward compatibility, allow named pipes
    if (named_pipes) {
        p_fout = fopen("ale_fifo_out", "w");
        p_fin = fopen("ale_fifo_in", "r");
    } else {
        p_fout = stdout;
        p_fin = stdin;
    }
    if (p_fout == NULL || p_fin == NULL) {
        cerr << "A.L.E expects two pipes to exist:\n";
        exit(1);
    }

    i_max_num_frames = p_osystem->settings().getInt("max_num_frames_per_episode");
    b_run_length_encoding = p_osystem->settings().getBool("run_length_encoding");
    // send the width and height of the screen through the pipe
    char out_buffer [50];
    cerr << "i_screen_width = " << i_screen_width << " - i_screen_height =" <<   i_screen_height << endl;
    sprintf (out_buffer, "%d-%d\n", i_screen_width, i_screen_height);
    fputs(out_buffer, p_fout);
    fflush (p_fout);

    // get confirmation that the values were sent
    char in_buffer [50];
    cerr<< "A.L.E: waiting for a reply ..." << endl;
    fgets (in_buffer, 50, p_fin);
    char * token = strtok (in_buffer,",\n");
    b_send_screen_matrix = atoi(token);
    token = strtok (NULL,",\n");
    b_send_console_ram = atoi(token);
    token = strtok (NULL,",\n");
    i_skip_frames_num = atoi(token);
    i_skip_frames_counter = i_skip_frames_num;
    token = strtok(NULL, ",\n");
    b_send_rewards = atoi(token);

    cerr << "A.L.E: send_screen_matrix is: " << b_send_screen_matrix << endl;
    cerr << "A.L.E: send_console_ram is: " << b_send_console_ram << endl;
    cerr << "A.L.E: i_skip_frames_num is: " << i_skip_frames_num    << endl;
    cerr << "A.L.E: reinforcement learning mode: " << b_send_rewards << endl;

    
    i_current_frame_number = 0;
}


/* destructor */
FIFOController::~FIFOController() {

    if (p_fout != NULL) fclose(p_fout);
    if (p_fin != NULL)  fclose(p_fin);
}


/* This is called on every iteration of the main loop. It is responsible
   for passing the framebuffer and the RAM content to whatever AI module we
   are using, and applying the returned actions. */
void FIFOController::update() {

    static bool first_step = true;
    if (first_step) {
        makeAveragePalette();
        first_step = false;
    }

    static char final_str[256000];
    int final_str_n = 0;
    
    Action player_a_action, player_b_action;

    // 0- See if we are skipping this frame
    if (i_skip_frames_counter < i_skip_frames_num) {
        // skip this frame
        i_skip_frames_counter++;
        if (e_previous_a_action == RESET || e_previous_b_action == RESET ) {
            player_a_action = PLAYER_A_NOOP;
            player_b_action = PLAYER_B_NOOP;
            e_previous_a_action = PLAYER_A_NOOP;
            e_previous_b_action = PLAYER_B_NOOP;
        } else {
            player_a_action = e_previous_a_action;
            player_b_action = e_previous_b_action;
        }
    } else {
        // don't skip this frame
        i_skip_frames_counter = 0;
        // 1- Send the updated pixels in the screen through the pipe

        if (b_send_console_ram) {
            // 1.1 - Get the ram content (128 bytes)
            for(int i = 0; i < 128; i++) {
                int ram_byte = read_ram(i);
                final_str_n += sprintf(final_str + final_str_n, "%02x", ram_byte);
            }
            final_str_n += sprintf(final_str + final_str_n, ":");
        }
        if (b_send_screen_matrix) {
            
            i_current_frame_number++;

            // MGB @phosphor
            phosphorBlend();
            // The next section is taken from FrameBufferSoft
            bool has_change = false;

            int ind_j = 0, ind_i = 0;

            int currentColor = -1;
            int runLength = 0;

            for (int i = 0; i < i_screen_width * i_screen_height; i++) {
                uInt32 rgb = pi_curr_frame_buffer[i];
                uInt8 col = rgbToNTSC(rgb);

                if (b_run_length_encoding) {
                  // Lengthen this run
                  if (col == currentColor && runLength < MAX_RUN_LENGTH)
                    runLength++;
                  else {
                    // Output it
                    appendByte(final_str + final_str_n, currentColor);
                    appendByte(final_str + final_str_n + 2, runLength);
                    final_str_n += 4;

                    // Switch to the new color
                    currentColor = col;
                    runLength = 1;
                  }
                }

                else { // Output full screen
                    appendByte(final_str + final_str_n, col); 
                    final_str_n += 2;
                }

                pi_old_frame_buffer[i] = col;
                has_change = true;
                ind_i++;
                if (ind_i == i_screen_width) { ind_j++; ind_i = 0; }
                
            }
            if (b_run_length_encoding && currentColor != -1) {
              appendByte(final_str + final_str_n, currentColor);
              appendByte(final_str + final_str_n + 2, runLength);
              final_str_n += 4;
            }

            // MGB - if no changes, we will send NADA
            if (!has_change) final_str_n += sprintf(final_str + final_str_n, "NADA");
            final_str_n += sprintf(final_str + final_str_n, ":");
        } // To be consistent, do not send anything when the screen is not requested

        // Send the RL data
        if (b_send_rewards) {
            bool max_frames_lim = i_max_num_frames > 0 && i_current_frame_number >= i_max_num_frames;
            m_rom_settings->step(*p_emulator_system);

            int r = m_rom_settings->getReward();
            int t = m_rom_settings->isTerminal();
            final_str_n += sprintf(final_str + final_str_n, "%d,%d:", t, r);
        }

        final_str_n += sprintf(final_str + final_str_n, "\n");
        fputs(final_str, p_fout);
        fflush(p_fout);

        // 2- Read the new action from the pipe
        // the action is sent as player_a_action,player_b_action
        char in_buffer[50];
        fgets (in_buffer, 50, p_fin);
        char * token = strtok (in_buffer,",\n");
        player_a_action = (Action)atoi(token);
        token = strtok (NULL,",\n");
        player_b_action = (Action)atoi(token);
    }

    // MGB Handle special actions (no Atari action is actually taken)
    switch (player_a_action) {
        // Load system state
        case LOAD_STATE:
            // Note - this does not reset the game screen; so that the subsequent screen
            //  is incorrect (in fact, two screens, due to colour averaging)
            loadState();
            p_osystem->skipEmulation();
            return;
        // Save system state (the emulator will be in the same state on the next time step)
        case SAVE_STATE:
            saveState();
            p_osystem->skipEmulation();
            return;
        case SYSTEM_RESET:
            systemReset();
            p_osystem->skipEmulation();
            return;
        default:
            // Ignore all other actions; handle them as normal
            break;
    }

    e_previous_a_action = player_a_action;
    e_previous_b_action = player_b_action;
    state.apply_action(player_a_action, player_b_action);
}


bool FIFOController::has_terminated() {
  // I am not convinced that we necessarily get an EOF on the input stream,
  //  especially if using named pipes; but this might be the best we can do
  return feof(p_fin);
}
