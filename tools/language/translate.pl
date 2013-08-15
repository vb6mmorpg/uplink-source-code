#!/usr/bin/perl
use strict;

use Getopt::Long;
use File::Find;

main();

sub main() {
    # Process command-line options
    
    my $prune;
    my $lang;
    my $help;

    my $result = GetOptions("prune" => \$prune,
			    "lang=s" => \$lang,
			    "help" => \$help);

    if (defined($lang) && defined($prune)) {
	print "Mutually exclusive options --prune and --lang\n";
	exit 1;
    }

    if (defined($lang)) {
	print "Translating source files to language \"$lang\"\n";

	my $transref = readtrans("strings.txt");
	foreach my $a (sourceFiles()) {
	    print "Translating $a\n";
	    # Translate into language
	    process($a, $lang, $transref, 1);
	}
	writetrans($transref, "strings.txt");
	print "Wrote strings.txt\n";
    }
    elsif (defined($prune)) {
	print "Scanning for unused translations.\n";
	
	my $transref = readtrans("strings.txt");
	foreach my $a (sourceFiles()) {
	    print "Scanning $a\n";
	    # Translate into german (gr)
	    process($a, "other", $transref, 0);
	}
	my @pruned = pruneunused($transref);
	if (@pruned > 0) {
	    print "\nUnused translations:\n";
	    foreach my $a (@pruned) {
		print "$a\n";
	    }
	    print "\n";
	}
	print scalar(@pruned) . " translations unused.\n";
	writetrans($transref, "strings-pruned.txt");
	print "Wrote strings-pruned.txt\n";
    }
    else {
	help();
	exit 1;
    }
}

sub pruneunused ($)
{
    my $transref = shift;
    my ($dict, $keys, $used) = @$transref;

    my @newkeys = ();
    my @pruned = ();

    foreach my $key (@$keys) {
	if ($used->{$key}) {
	    push @newkeys, $key;
	}
	else {
	    push @pruned, $key;
	    $dict->{$key} = undef;
	}
    }

    @$keys = @newkeys;
    return @pruned;
}

sub help() {
    print "usage: $0 [--prune|--lang=[gr|fr|ru]\n";
}

# return all the source files to 
# be translated
sub sourceFiles()
{
    my @files = ();

    find( sub { push @files, $File::Find::name
		    if (# files, not directories
			-f $_
			# has a C++ source extension
			&& /\.(cpp|cxx|cc|c)$/
			# not already translated files
			&& !/-trans\.(cpp|cxx|cc|c)$/
			# not an encrypted text file
			&& !/-sc\.(cpp|cxx|cc|c)$/); }, 
	  "." );

    return @files;
}

#
# Format of the translations file:
# Paragraphs of:
#
# en: english text
# gr: german text
# fr: french text

# en*: english text that does not need translation

sub testtrans () {
  print "begin\n";
  my $transref = readtrans("strings.txt");
  my ($translation, $warning) = lookuptrans($transref, "fr", "hello");
  print "$warning\n" if defined $warning;
  writetrans($transref, "strings.txt");
  print "end\n";
}

#
# Read the translations file into a hash
#   english_string -> { language -> string }
#
# and an array of keys to indicate ordering.
#
# transref = readtrans(inputfilename)
#
#
# transref is of the form [\%dict, \@keys, \%used]
#   where dict is the hash described above
#         keys are the keys of the hash
#         used is a reference to an array describing which 
#           words were looked up during a translate process.
#

sub readtrans ($) {
  my $filename = shift;
  my @keys = ();
  my %dict = ();

  # Parser states
  #  1 = Looking for english text
  #  2 = Reading translations

  my $state = 1;
  my $english = "";
  my $lineno = 0;

  open TRANS, "<$filename" or die "Failed to open $filename for reading: $!";
  while (<TRANS>) {
    $lineno++;
    chomp;
    s/^[ \t]*//;
    s/[ \t]*$//g;

    if ($state == 1) {
      # Looking for english text
      # Skip blank lines
      next if ($_ eq "");

      if (/^en:\s*\"(.*)\"$/) {
	$english = $1;
	push @keys, $english;
	$state = 2;
	$dict{$english} = {};
      }
      elsif (/^en\*:\s*\"(.*)\"$/) {
	$english = $1;
	push @keys, $english;
	$dict{$english} = {'*NT*' => 1}; # *NT* -> no translation required.
      }
      else {
	warn "$filename:$lineno: warning, expected \"en:\"";
      }
    }
    else {
      if ($_ eq "") {
	# Blank line found, move back to state 1
	# (looking for english text)
	$state = 1;
      }
      elsif (/^en\*?:/) {
	die "$filename:$lineno: need paragraph separator (blank line) before starting a new translation\n";
      }
      elsif (/^(\w+):\s*\"(.*)\"$/) {
	# We have a translation for a particular piece of english
	# Check that this is not a duplicate (probably an error)
	my $lang = $1;
	my $translation = $2;
	my $hashref = $dict{$english} || {};
	warn "$filename:$lineno: multiple definitions for $english, language $lang"
	  if (defined $hashref->{$lang});
	$hashref->{$lang} = $translation;
	$dict{$english} = $hashref;
      }
    }
  }
  close TRANS;
  my %used = ();
  return [\%dict, \@keys, \%used];
}

# Write the translation back to a file
# writetrans(transref, filename)

sub writetrans ($$) {
  my ($transref, $filename) = @_;
  my ($dict, $keys, $used) = @$transref;

  # Keep up a backup
  if (-f "$filename.bak") {
    unlink "$filename.bak" or
      warn "Could not delete \"$filename.bak\"";
    rename "$filename", "$filename.bak";
  };

  # Write it out
  open TRANS, ">$filename" or 
    die "Failed to open $filename for writing: $!\n";

  foreach my $key (@$keys) {
    unless ($dict->{$key}->{'*NT*'}) {
      # key is translatable
      print TRANS "en: \"$key\"\n";
      my $entryref = $dict->{$key};
      my @langs = keys(%$entryref);
      foreach my $lang (@langs) {
	print TRANS "$lang: \"" . $dict->{$key}->{$lang} . "\"\n";
      }
    }
    else {
      # key is untranslatable
      print TRANS "en*: \"$key\"\n";
    }
    print TRANS "\n";
  }
  close TRANS;
}

#
# Lookup a translation (and add a blank entry if no entry is found)
#
# ($translation, $warning) = lookuptrans($transref, $language, $english)
#
# if there was no translation for the requested word,
# it is added to the dictionary

sub lookuptrans($$$) {
  my ($transref, $language, $english) = @_;
  my ($dict, $keys, $used) = @$transref;

  # Keep track of which translations are referenced by 
  # a source file
  $used->{$english} = 1;

  # Look up the translation
  my $entry = $dict->{$english};
  if (!defined $entry) {
    # Add a new entry
    $dict->{$english} = {};
    push @$keys, $english;
    return ($english, "New translation: $english");
  }
  else {
    my $translation = $entry->{$language};
    if (!defined $translation) {
      if (!defined $entry->{'*NT*'}) {
	# Missing translation
	return ($english, "Translation missing for $english");
      }
      else {
	return ($english, undef);
      }
    }
    else {
      return ($translation, undef);
    }
  }
}

sub splitlonglines {
    # nm split length must be a multiple of 3
  local $_ = shift;
  my $w = "";
  if (s/^(.{1,72})//) {
      $w = $1;
  }
  while (s/^(.{1,72})//) {
    $w .= "\"\n\"$1";
  }
  return $w;
}

sub process ($$$$) {
    my ($filename, $lang, $transref, $writetranslated) = @_;
    my $lineno = 0;
    my $id = "sc00000";
    my @ids = (); # (lineno, id, text) triples
    my @code = (); # source code of the file

    my $outfilename = outputExtension($filename);

    open INPUT, "<$filename" or 
      die "Failed to open $filename for reading: $!";
    my @lines = <INPUT>;
    close INPUT;

    # Remove comments, glue together multiline strings
    my $text = join("", @lines);
    $text =~ s%//.*%%gm;
    while ($text =~ s/\"(([^\n\"\\]|\\.)*)\"([ \t\n\r]*\n[ \t\n\r]*)\"(([^\n\"\\]|\\.)*)\"/\"$1$4\"$3/) {
	#print STDERR "1=$1\n3=$3\n4=$4\n";
    }

    @lines = split(/\n/,$text);
    @lines = map { s/$/\n/; $_ } @lines;

    # Processes each string literal, substitute the translation
    for (@lines) {
	$lineno++;

	my @fragments = ();

	unless (/^\s*\#/) { # skip preprocessor directive
	    while (/\"(([^\n\"\\]|\\.)*)\"/) {
	      my $text = $1;
	      my ($translation, $warning) = lookuptrans($transref, $lang, $text);
	      if (defined $warning && $writetranslated) {
		print STDERR "$filename:$lineno: $warning\n";
	      }
	      s/\"(([^\n\"\\]|\\.)*)\"//;
	      push @fragments, ($`, "\"$translation\"");
	      $_ = $'; # '
	    }
	}
	push @fragments, $_;
	push @code, (join "", @fragments);
    }

    if ($writetranslated) {
	# Generate output

	open OUTPUT, ">$outfilename" or 
	    die "Failed to open $outfilename for writing: $!";

	print OUTPUT "/*\n";
	print OUTPUT " *\n";
	print OUTPUT " * WARNING: DO NOT EDIT THIS FILE, EDIT $filename INSTEAD\n";
	print OUTPUT " * WARNING: DO NOT EDIT THIS FILE, EDIT $filename INSTEAD\n";
	print OUTPUT " * WARNING: DO NOT EDIT THIS FILE, EDIT $filename INSTEAD\n";
	print OUTPUT " * WARNING: DO NOT EDIT THIS FILE, EDIT $filename INSTEAD\n";
	print OUTPUT " * WARNING: DO NOT EDIT THIS FILE, EDIT $filename INSTEAD\n";
	print OUTPUT " *\n";
	print OUTPUT " */\n";
	print OUTPUT "\n";
	print OUTPUT "#line \"$filename\" 1\n";

#     print OUTPUT "#ifdef __cplusplus\n";
#     print OUTPUT "extern \"C\" {\n";
#     print OUTPUT "#endif\n";

#     print OUTPUT "char *scDecode( char * );\n";

#     print OUTPUT "#ifdef __cplusplus\n";
#     print OUTPUT "}\n";
#     print OUTPUT "#endif\n";

#     foreach my $k (@ids) {
# 	my ($lineno, $id, $text) = @$k;
# 	my @encoded = encode(unquote($text));
# 	my $enctext = quote(@encoded);
# 	my $lines = splitlonglines($enctext);
# 	print OUTPUT "static char $id"."[".(@encoded+2).
# 	  "] = \"X$lines\";\n";
#     }

	foreach my $l (@code) {
	    print OUTPUT $l;
	}

	close OUTPUT;
    }
}

sub unquote {
    local $_ = shift;
    s/\\\\/##BACK---SLASH##/g;
    s/\\n/\n/g;
    s/\\r/\r/g;
    s/\\t/\t/g;
    s/\\f/\f/g;
    s/\\b/\b/g;
    s/\\a/\a/g;
    s/##BACK---SLASH##/\\/g;
    return $_;
};

sub quote {
    my $s = "";
    foreach my $ch (@_) {
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
    foreach my $ch (@c) {
	unless ($ch == 128 || $ch == 0) {
	    $ch += 128;
	    $ch %= 256;
	}
	push @cc, $ch;
    }

    @cc;
};

sub outputExtension {
    local $_ = shift;
    my $filename = $_;
    return $_ if (s/.(cpp|cxx|cc|c)$/-trans.$1/i) > 0;
    die "$filename must have .cpp, .cc or .C, .c extension";
};
