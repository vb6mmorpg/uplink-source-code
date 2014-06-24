#!/usr/bin/env perl
use strict;
use File::Spec;

srand();

if (!$ARGV[0]) {
	print "Usage: ./generate.pl <TYPE> [SRC] [HEADER]\n";
	print "This script generates a version of BinReloc with a coding style/API that fits\n";
	print "your project's coding style and prefferred API. The result will be written to\n";
	print "the files SRC ('binreloc.c' by default) and HEADER ('binreloc.h' by default).\n\n";
	print "TYPE is one of the following:\n";
	print "  normal       Use normal (raw C) API, with no further dependancies.\n";
	print "  glib         Use glib-style API.\n\n";
	exit;
}

my $basic = getFile("basic.c");

my $br_license  = getSection($basic, 'LICENSE');
my $br_include  = getSection($basic, 'INCLUDE');
my $br_errors   = getSection($basic, 'ERROR');
my $br_function = getSection($basic, 'FUNCTION');
my $br_c_guard1 = "#ifndef __BINRELOC_C__\n" .
	          "#define __BINRELOC_C__\n";
my $br_c_guard2 = "#endif /* __BINRELOC_C__ */\n";
my $br_h_guard1 = $br_c_guard1;
my $br_h_guard2 = $br_c_guard2;

$br_h_guard1 =~ s/_C_/_H_/mg;
$br_h_guard2 =~ s/_C_/_H_/;

my $cppguard1 = "#ifdef __cplusplus\n" .
	        "extern \"C\" {\n" .
	        "#endif /* __cplusplus */\n";
my $cppguard2 = "#ifdef __cplusplus\n" .
	        "}\n" .
	        "#endif /* __cplusplus */\n";

my $mangle = '';
for (my $i = 0; $i < 3; $i++) {
	my @chars = qw(a b c d e f g h i j k l m n o p q r s t u v w x y z);
	$mangle .= $chars[rand(scalar @chars)];
}
$mangle .= int(rand(99999999999999));


my $srcfile = ($ARGV[1] || "binreloc.c");
my $headerfile = ($ARGV[2] || "binreloc.h");
my (undef, undef, $headerincludefile) = File::Spec->splitpath($headerfile);

my $src = '';
my $header = '';


if ($ARGV[0] eq "normal") {
	$src = "$br_license\n$br_c_guard1\n";
	$br_function =~ s/^static //m;
	$src .= $br_include;
	$src .= "#include \"$headerincludefile\"\n\n";
	$src .= "$cppguard1\n";
	$src .= "\n\n$br_function\n\n";
	$src .= "$cppguard2\n";
	$src .= $br_c_guard2;

	$header = "$br_license\n$br_h_guard1\n$cppguard1\n\n";
	$header .= "$br_errors\n\n";
	$header .= "/* Mangle symbol name to avoid symbol collision with other ELF objects. */\n";
	$header .= "#define br_find_exe ${mangle}_br_find_exe\n\n";
	$header .= "char *br_find_exe (BrFindExeError *error);\n\n\n";
	$header .= "$cppguard2\n";
	$header .= $br_h_guard2;

} elsif ($ARGV[0] eq "glib") {
	my $glib = getFile("glib.c");

	$cppguard1 = "G_BEGIN_DECLS\n";
	$cppguard2 = "G_END_DECLS\n";

	$src = "$br_license\n$br_c_guard1\n";
	$src .= $br_include;
	$src .= "#include \"$headerincludefile\"\n\n";
	$src .= "$cppguard1\n\n";

	$br_function =~ s/BrFind/GbrFind/g;
	$br_function =~ s/BR_FIND/GBR_FIND/g;
	$br_function =~ s/malloc \(buf_size\)/g_try_malloc ((gulong) buf_size)/;
	$br_function =~ s/realloc \(path, buf_size\)/g_try_realloc (path, (gulong) buf_size)/;
	$br_function =~ s/strdup \(/g_strdup (/;
	$br_function =~ s/free \(/g_free (/g;
	$src .= "$br_function\n\n";
	$src .= "$glib\n\n";

	$src .= "$cppguard2\n";
	$src .= $br_c_guard2;

	$br_errors =~ s/BrFindExeError/GbrFindExeError/;
	$br_errors =~ s/BR_FIND/GBR_FIND/g;
	$header = "$br_license\n$br_h_guard1\n";
	$header .= "#include <glib.h>\n\n";
	$header .= "$cppguard1\n\n";
	$header .= "$br_errors\n\n";
	$header .= mangle(qw/gbr_find_exe gbr_find_prefix gbr_find_data_dir gbr_find_locale_dir/);
	$header .= "gboolean gbr_init            (GError **error);\n\n";
	$header .= "gchar   *gbr_find_exe        (void);\n";
	$header .= "gchar   *gbr_find_prefix     (const gchar *default_prefix);\n";
	$header .= "gchar   *gbr_find_data_dir   (const gchar *default_data_dir);\n";
	$header .= "gchar   *gbr_find_locale_dir (const gchar *default_locale_dir);\n";
	$header .= "\n\n";
	$header .= "$cppguard2\n";
	$header .= $br_h_guard2;

} else {
	print STDERR "Unknown type '$ARGV[0]'\n";
	exit 1;
}



my $filename = $srcfile;
if (!open(F, "> $filename")) {
	print STDERR "Cannot write to $filename\n";
	exit 1;
}
print F $src;
close F;
print "Source code written to '$filename'\n";

$filename = $headerfile;
if (!open(F, "> $filename")) {
	print STDERR "Cannot write to $filename\n";
	exit 1;
}
print F $header;
close F;
print "Header written to '$filename'\n";



sub getSection {
	my ($code, $section) = @_;
	my ($result) = $code =~ /\/\*\*\* $section BEGIN \*\/\n(.*?)\/\*\*\* $section END \*\//s;
	return $result;
}

sub getFile {
	my ($file) = @_;
	my ($f, $content);

	if (!open($f, "< $file")) {
	print STDERR "Cannot open $file\n";
	exit 1;
	}
	local($/);
	$content = <$f>;
	close $f;
	return $content;
}

sub mangle {
	my $result = "/* Mangle symbol names to avoid symbol collisions with other ELF objects. */\n";
	foreach (@_) {
		$result .= "#define $_ ${mangle}_$_\n";
	}
	return "$result\n\n";
}
