#!/usr/bin/perl

foreach $a (@ARGV) {
    process($a);
}

sub splitlonglines {
  local($_) = shift;
  my $w = "";
  while (s/^.{1,80}//) {
    $w = $1 . "\"\n\"";
  } 
  return $w;
}
   
sub process {
    my $lineno = 0;
    my $id = "sc00000";
    my $filename = shift;
    my @ids = (); # (lineno, id, text) triples
    my @code = (); # source code of the file

    my $outfilename = outputExtension($filename);

    open INPUT, "<$filename" or 
      die "Failed to open $filename for reading: $!";
    @lines = <INPUT>;
    close INPUT;

    # Remove comments, glue together multiline strings
    $text = join("", @lines);
    $text =~ s%//.*%%gm;
    while ($text =~ s/\"(([^\n\"\\]|\\.)*)\"([ \t\n\r]*\n[ \t\n\r]*)\"(([^\n\"\\]|\\.)*)\"/\"$1$4\"$3/) {
	#print STDERR "1=$1\n3=$3\n4=$4\n";
    }
    
    @lines = split(/\n/,$text);
    @lines = map { s/$/\n/; $_ } @lines;

    # Processes each string literal, substitutes the identifier
    for (@lines) {
	$lineno++;

	unless (/^\s*\#/) { # skip preprocessor directive

	    while (s/\"(([^\n\"\\]|\\.)*)\"/scDecode($id)/) {
		push(@ids, [$lineno, $id, $1]);
		$id++;
	    }

	}

	push @code, $_;
    }

    # Generate output

    open OUTPUT, ">$outfilename" or 
      die "Failed to open $outfilename for writing: $!";

    print OUTPUT "#ifdef __cplusplus\n";
    print OUTPUT "extern \"C\" {\n";
    print OUTPUT "#endif\n";

    print OUTPUT "char *scDecode( char * );\n";

    print OUTPUT "#ifdef __cplusplus\n";
    print OUTPUT "}\n";
    print OUTPUT "#endif\n";

    foreach $k (@ids) {
	my ($lineno, $id, $text) = @$k;
	my @encoded = encode(unquote($text));
	my $enctext = quote(@encoded);
	my $lines = splitlonglines($encodeded);
	print OUTPUT "static char $id"."[".(length($enctext)+2).
	  "] = \"X$lines\";\n";
    }

    print OUTPUT "\n#line 1 \"$filename\"\n";
    foreach $l (@code) {
	print OUTPUT $l;
    }
    
    close OUTPUT;
}

sub unquote {
    local $_ = shift;
    s/\\n/\n/;
    s/\\r/\r/;
    s/\\t/\t/;
    s/\\f/\f/;
    s/\\b/\b/;
    s/\\a/\a/;
    return $_;
};

sub quote {
    my $s = "";
    foreach $ch (@_) {
 	if (chr($ch) !~ /[\w\s\d]/) {
	    $s .= "\\x". (sprintf "%x", $ch);
 	}
 	else {
 	    $s .= chr($ch);
 	}
    }
    $s;
};

sub encode {
    my @c = unpack("C*",shift);
    my @cc = ();

    pack("C*", @c);
    foreach $ch (@c) {
	unless ($ch == 128 || $ch == 0) {
	    $ch += 128;
	    $ch %= 256;
	}
	push @cc, $ch;
    }

    @cc;
} 

sub outputExtension {
    local $_ = shift;
    my $filename = $_;
    return $_ if (s/.(cpp|cxx|cc|c)$/-sc.$1/i) > 0;
    die "$filename must have .cpp, .cc or .C, .c extension";
};
