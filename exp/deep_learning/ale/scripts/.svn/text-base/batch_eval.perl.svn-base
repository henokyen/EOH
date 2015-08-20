# -----------------------------------
#  Process command line arguments
# -----------------------------------
$numArgs = $#ARGV + 1;
die "Usage: $0 <sample size> <threads>" if ($numArgs != 2);

# ----------------------------------------------------------
#  Parallel Work Queue (taken from somewhere on the net...
# ----------------------------------------------------------
#!/usr/bin/perl
use Carp;
use IPC::Open3;
use vars qw( $debug );
$debug = 0;

# The first parameter is how many jobs to run at once, the remaining are
# the jobs.  Jobs may be a string, or an anonymous array of the cmd and
# args.
#
# All output from children go to your STDERR and STDOUT.  They get no
# input.  It prints fairly uninformative errors for commands with
# problems, and returns a hash of problems.
#
# The jobs SHOULD NOT depend on each other!
sub run_parallel {
  my $job_count = shift;
  unless (0 < $job_count) {
    confess("run_parallel called without a positive parallel job count!");
  }
  my @to_start = @_;
  my %running;
  my %errors;
  my $is_running = 0;
  while (@to_start or %running) {
    if (@to_start and ($is_running < $job_count)) {
      # Launch a job
      my $job = shift @to_start;
      unless (ref($job)) {
        $job = [$job];
      }
      print "Launching '$job->[0]'\n" if $debug;
      local *NULL;
      my $null_file = ($^O =~ /Win/) ? 'nul': '/dev/null';   
      open (NULL, $null_file) or confess("Cannot read from $null_file:$!");
      my $proc_id = open3("<&NULL", ">&STDOUT", ">&STDERR", @$job);
      $running{$proc_id} = $job;
      ++$is_running;
    }
    else {
      # collect a job
      my $proc_id = wait();
      if (! exists $running{$proc_id}) {
        confess("Reaped unknown process $proc_id!");
      }
      elsif ($?) {
        # Oops
        my $job = $running{$proc_id};
        my ($cmd, @args) = @$job;
        my $err = "Running '$cmd' gave return code '$?'";
        if (@args) {
          $err .= join "\n\t", "\nAdditional args:", @args;
        }
        print STDERR $err, "\n";
        $errors{$proc_id} = $err;
      }
      print "Reaped '$running{$proc_id}->[0]'\n" if $debug;
      delete $running{$proc_id};
      --$is_running;
    }
  }
  return %errors;
}

# -----------------------------------------
#   roms that are known to have problems
# -----------------------------------------
@skiplist = (
    "elevator_action",
    "fishing_derby",
    "joust",
    "canyon_bomber",
    "pitfall2",
    "frontline",
    "skiing",
    "venture",
    "miner_2049r"
);
my %skiphash;
@skiphash{@skiplist}=();

# -------------------------------
#    extract the list of roms
# -------------------------------
@f = glob "~/roms/*.bin";
@files = ();
for ($i=0; $i <= $#f; $i++) {
    @g = split /\//, $f[$i];
    @h = split /\./, $g[$#g];
    push files, $h[0] if not exists $skiphash{$h[0]};
}

# ----------------------------------------
#   run the evaluate script in parallel
# ----------------------------------------
$repeats = $ARGV[0];

my @jobs = (); 
for ($j=0; $j <= $#files; $j++) {
    $rom = $files[$j];
    $cmd = "perl evaluate.perl $rom $repeats 2> $rom.dump";
    push(@jobs, $cmd); 
}

&run_parallel($ARGV[1], @jobs);





