#!/usr/bin/perl
use IPC::Open2;

# -----------------------------------
#          test configuration
# -----------------------------------

# disables buffered IO
$| = 1;

$OS = $^O;
$numArgs = $#ARGV + 1;
die "Usage: $0 <rom> <sample size>" if ($numArgs != 2);

$debug_mode = 2; # 0 off, 1 some information, 2 verbose, 3 avalanche
$rom = $ARGV[0];
$sample_size = $ARGV[1];


$env_cmd = "./ale.exe -game_controller fifo -export_frame_frq 1 ../../../roms/$rom.bin";
$agent_cmd   = "perl random.perl";
#$agent_cmd    = "java -Xmx1024M -jar ALEJavaAgent.jar";


if ($OS eq "linux" or $OS eq "darwin") {
  # platform specific code
}

$start_time = time;

# -----------------------------------
#            main loop
# -----------------------------------

for ($i=1; $i <= $sample_size; $i++) {

    local (*AGENT_READ, *AGENT_WRITE, *AGENT_ERR);
    local (*ENV_READ, *ENV_WRITE, *ENV_ERR);
    
    my $pid_env    = open2(\*ENV_READ, \*ENV_WRITE, $env_cmd);
    my $pid_agent  = open2(\*AGENT_READ, \*AGENT_WRITE, $agent_cmd);
    
    print STDERR "Started Agent with PID: $pid_agent\n" if $debug_mode > 1;
    print STDERR "Started Environment with PID:   $pid_env\n" if $debug_mode > 1;
    
    $total_reward = 0;
    $c = 1;
    $ep_start_time = time;

    EVALUATE: {
    do {        
        
        # read from environment
        if (!kill(0, $pid_env)) {
            print "error: environment terminated unexpectedly";
            exit(1)
        }
        $l = <ENV_READ>;
        if ($l eq "") {
            print "error: environment terminated unexpectedly\n";
            exit(1);
        }
        
	# extract reward and terminal status, skip during handshaking
	if ($c > 1) {
	    @f = split /:/, $l; 
  	    $tok = $f[$#f-1];
	    @g = split /,/, $tok;
 	    $terminate = $g[0];	    
	    $total_reward += $g[1];
	    print STDERR "Time: $c, Reward: $g[1],\n" if $debug_mode >= 2 && $g[1] != 0;
	    print STDERR "Time: $c, Reward: $g[1],\n" if $debug_mode >= 3;
	    if ($terminate == 1) {
	     	$ep_end_time = time;
	        last EVALUATE;
	    }
	}
	
	# send to agent
	print AGENT_WRITE $l;
        
        # read from agent, send to environment
        die "agent terminated unexpectedly" unless kill(0, $pid_agent);
        $a = <AGENT_READ>; 
        print ENV_WRITE $a;
        print STDERR "Action: $a\n" if $debug_mode >= 3;
        
        $c++;
        $cycles++;
        
    } while (1);
    }
        
    # terminate agent/environment
    
    #sleep 1;
    close ENV_READ; close ENV_WRITE; close AGENT_READ; close AGENT_WRITE;
    kill $pid_env, 9; kill $pid_agent, 9;
    waitpid($pid_agent, 0); waitpid($pid_env, 0);
    
    # display episode information
    $ave_rew = $total_reward / $c;
    $ep_time = $ep_end_time - $ep_start_time;
    $cps = $c / $ep_time;
    printf STDERR (
      "Episode Length: %8d,". 
      "Episode Return: %12.2f,". 
      "Episode Avg Reward: %8.3f,". 
      "Episode Time: %8d"."s,".
      "Cycles Per Second: %12.2f\n",
       $c, 
       $total_reward, 
       $ave_rew, 
       $ep_time,
       $cps
    ) if $debug_mode;

    # update statistics
    $mu_old = $mu;
    $mu = $mu + (1.0 / $i) * ($total_reward - $mu);
    if ($i > 1) {
        $s = ($i - 2.0) * $s + ($total_reward - $mu) * ($total_reward - $mu_old);
        $s /= ($i - 1.0);
    }
}

$end_time = time;


# -----------------------------------
#       report statistics
# -----------------------------------
$ave_cps = $cycles / ($end_time - $start_time);
$est_std_err = sqrt($s / $sample_size);
$lmu = $mu - 1.96 * $est_std_err;
$umu = $mu + 1.96 * $est_std_err;
if ($sample_size == 1) {
    # sample variance based statistics undefined in this case
    $lmu = $umu = $s = "N/A";
}

print "$rom Runs: $sample_size, Est. Avg Return = $mu, Est. Var Return: $s, ".
      "Approx. 95% C.I. for Avg Return: [$lmu,$umu], Average CPS: $ave_cps\n";

