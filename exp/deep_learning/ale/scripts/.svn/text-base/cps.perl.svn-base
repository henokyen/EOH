
# -----------------------------------------
#   roms that are known to have problems
# -----------------------------------------
@skiplist = (
    "elevator_action",
    "fishing_derby",
    "skiing",
    "venture"
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
#   report the average cps for each rom
# ----------------------------------------
$repeats = 10;

for ($j=0; $j <= $#files; $j++) {
    $rom = $files[$j];
    $cmd = "perl evaluate.perl $rom $repeats 2> dump";
    $output = `$cmd`;
    if (!($output =~ /error/)) {
    	@f = split /\n/, $output;
        @g = split /:/, $f[$#f];
        print "$rom".":".$g[$#g]."\n";
    } else {
        print "$rom: no result\n";
    }
}