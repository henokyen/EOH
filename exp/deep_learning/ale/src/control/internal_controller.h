
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
 *  internal_controller.h
 *
 *  The internal controller acts as an environment wrapper for a C++ agent.
 **************************************************************************** */

#ifndef __INTERNAL_CONTROLLER_H__
#define __INTERNAL_CONTROLLER_H__

#include "Constants.h"
#include "game_controller.h"
#include "PlayerAgent.hpp"
#include "RomSettings.hpp"


class InternalController : public GameController {
    public:
        /* *********************************************************************
            Constructor
         ******************************************************************** */
        InternalController(OSystem* _osystem);
        
        /* *********************************************************************
            Destructor
         ******************************************************************** */
        virtual ~InternalController();
        
        /* *********************************************************************
            This is called on every iteration of the main loop. It is responsible 
            passing the framebuffer and the RAM content to whatever AI module we 
            are using, and applying the returned actions.
         * ****************************************************************** */
        virtual void update();

        /** Returns true if we should end the emulation */
        virtual bool has_terminated();
	
	reward_t getReward() const { return _reward; }
	void resetReward() { _reward = 0; }
	// JBM
	void set_player_agent(PlayerAgent* p) { p_player_agent_left = p; }
	void end_episode();
    protected:	
        PlayerAgent* p_player_agent_right;	// The PlayerAgent object responsible  
        PlayerAgent* p_player_agent_left;	// for controlling right/left player
	bool first_step;
	reward_t _reward;
};

#endif // __INTERNAL_CONTROLLER_H__
