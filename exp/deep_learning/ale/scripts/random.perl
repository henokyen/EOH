$| = 1;

# read resolution
$l = <STDIN>;

print STDOUT "1,0,0,1\n";

while (1) {
    $l = <STDIN>;
    $x = int(rand(18));
    print STDOUT "$x,18\n";
    
}
