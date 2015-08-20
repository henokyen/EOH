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
 *  fifo_controller.h
 *
 *  The implementation of the FIFOController class, which is a subclass of
 * GameController, and is resposible for sending the Screen/RAM/RL content to
 * whatever external program we are using through FIFO pipes, and apply the
 * actions that are sent back
 **************************************************************************** */

#ifndef __FIFO_CONTROLLER_H__
#define __FIFO_CONTROLLER_H__

#include "Constants.h"
#include "game_controller.h"

class RomSettings;

class FIFOController : public GameController {

    public:

        FIFOController(OSystem* _osystem, bool named_pipes = false);
        virtual ~FIFOController();


        // This is called on every iteration of the main loop. It is responsible
        // passing the framebuffer and the RAM content to whatever AI module we
        // are using, and applying the returned actions.
        virtual void update();

        virtual bool has_terminated();

    protected:

    protected:

        int i_max_num_frames;
        int i_current_frame_number;

        bool b_run_length_encoding;

        FILE* p_fout;               // Output Pipe
        FILE* p_fin;                // Input Pipe
};

#endif  // __FIFO_CONTROLLER_H__


