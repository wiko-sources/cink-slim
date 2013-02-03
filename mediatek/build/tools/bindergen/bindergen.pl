#!/usr/bin/perl

use strict;
use File::Find;
use File::Basename;

my @path = qw(./frameworks/ ./mediatek/source/);
my @aidl;
my $path;
my $name;
my $base_name;
my $curr_line;
my $cnt;
my $outdir = dirname($0);

sub collect_aidl
{
	if ($File::Find::name =~ /\/I[^\/]*\.aidl$/) {
		#print "$File::Find::name\n";
		push @aidl, "$File::Find::name";
	}
}

find (\&collect_aidl, @path);

open (FMAP, ">$outdir/binder_fmap.txt");

foreach $name (@aidl) {
	$cnt = 0;
	$base_name = basename($name, '.aidl');
	open (AIDL, "<$name") || die "Can't open file $name : $!\n";
	while ($curr_line = <AIDL>) {
		if ($curr_line =~ /^[\s\w\d]*interface[\s]*$base_name/) {
			print FMAP "$base_name:\n";
			$cnt++;
		}
		if ($cnt > 0) {
			if ($curr_line =~ /\); *\r?\n$/) {
				$curr_line =~ s/^[^\w\d]+//g;
				print FMAP "$cnt: $curr_line";
				$cnt++;
			}
		}
	}
	close (AIDL);
}

close (FMAP);
