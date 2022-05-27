#!/bin/perl -l
#
use strict;
use warnings;

use File::Spec::Functions;

# Test script for validating programmer

my $testdir = 'test';

my @tests = @ARGV;
my $chip;

sub run { print $_[0]; return `$_[0]`; }

if (@tests == 0) {
	opendir(DIR, $testdir) or die "opendir: $!";
	@tests = grep { m/\.hex$/ && -f catfile($testdir, $_) } readdir(DIR);
	closedir(DIR);
	@tests = map { s/\.hex$// } @tests;
}

TEST: foreach my $test (@tests) {
	my $pass = 0; # Fail by default

	$chip = uc $test;
	my $hexfile = "$test.hex";
	print "$chip: $hexfile";

	my $hexpath = catfile($testdir, $hexfile);

	my $randfile = sprintf "_tmp_%08X.hex", rand(0xffffffff);
	print "Insert $chip and then press ENTER or type q to abort";
	my $input = <STDIN>;
	if ($input =~ m/q/i) { exit 0; }

	my $out = &run("./op -d $chip -w $hexpath");

	if ($? != 0) {
		print "Write failed - output follows:\n\n$out\n";
		next TEST;
	}

	$out = &run("./op -d $chip -s $randfile");

	if ($? != 0) {
		print "Read failed - output follows:\n\n$out\n";
		next TEST;
	}

	my $origsum = `md5sum $hexpath | cut -f1 -d" "`;
	my $newsum = `md5sum $randfile | cut -f1 -d" "`;

	if ($origsum ne $newsum) {
		print "output different from input - see files $hexpath and $randfile";
	}
	else {
		print "Test PASSED";
		unlink $randfile;
	}
}
