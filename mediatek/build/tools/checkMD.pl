#!/usr/bin/perl

#######################################
# Description
## The script is for checking the consistency between DSP_BL, DSP_ROM, modem.img and MTK_MODEM_SUPPORT/MTK_PLATFORM option setting.
## Check item: mode(2G/3G), debug/release mode, file type, platform, project ID(branch/week)
#######################################
usage() if ($#ARGV < 1);

my ($DSP_BL, $DSP_ROM, $MD_IMG, $MTK_MODEM_SUPPORT, $MTK_PLATFORM) = @ARGV;

#######################################
# Initialization
#######################################
my $position1 = 22; # 0x16
my $position2 = 8; # 0x08
my $whence = 0;

my $magic_length = 3;
my $length1 = 4;
my $length2 = 16;
my $length3 = 188; # modem.img rear
my $length4 = 152; # new GFH
my $length5 = 168;

my $mode;
my $temp;
my $temp1;
my $header_str; # modem.img

my $DSP_BL_DEBUG;
my $DSP_BL_MODE;
my $DSP_BL_FILE_TYPE;
my $DSP_BL_PLATFORM;
my $DSP_BL_CHIPVER;
my $DSP_BL_PROJECT_ID;

my $DSP_ROM_DEBUG;
my $DSP_ROM_MODE;
my $DSP_ROM_FILE_TYPE;
my $DSP_ROM_PLATFORM;
my $DSP_ROM_CHIPVER;
my $DSP_ROM_PROJECT_ID;

my $MD_IMG_DEBUG;
my $MD_IMG_MODE;
my $MD_IMG_PLATFORM;
my $MD_IMG_CHIPVER;
my $MD_IMG_PROJECT_ID;

my $errCnt = 0;

if ($MTK_PLATFORM eq "MT6573") { # Temp for MT6573
	if ($MTK_MODEM_SUPPORT eq "modem_3g_tdd") {
		$mode = 3; # 3G TDD
	} elsif ($MTK_MODEM_SUPPORT eq "modem_3g_fdd") {
		$mode = 2; # 3G FDD
	} elsif ($MTK_MODEM_SUPPORT eq "modem_2g") {
		$mode = 1; # 2G
	} else {
		$mode = -1; # invalid
	}
} else {
	if ($MTK_MODEM_SUPPORT eq "modem_3g") {
		$mode = 2; # 3G
	} elsif ($MTK_MODEM_SUPPORT eq "modem_2g") {
		$mode = 1; # 2G
	} else {
		$mode = -1; # invalid
	}
}

#######################################
# Check if DSP_BL/DSP_ROM/MODEM file exists 
#######################################
die "[MODEM CHECK FAILED]: The file \"$DSP_BL\" does NOT exist!\n" if (!-e $DSP_BL);
die "[MODEM CHECK FAILED]: The file \"$DSP_ROM\" does NOT exist!\n" if (!-e $DSP_ROM);
die "[MODEM CHECK FAILED]: The file \"$MD_IMG\" does NOT exist!\n" if (!-e $MD_IMG);

#######################################
# Read mode(2G/3G), debug/release flag, file type, platform info, project info, etc. from DSP_BL/DSP_ROM/modem.img
#######################################
($DSP_BL_DEBUG, $DSP_BL_MODE, $DSP_BL_FILE_TYPE, $DSP_BL_PLATFORM, $DSP_BL_PROJECT_ID) = &Parse_GFH($DSP_BL);
($DSP_ROM_DEBUG, $DSP_ROM_MODE, $DSP_ROM_FILE_TYPE, $DSP_ROM_PLATFORM, $DSP_ROM_PROJECT_ID) = &Parse_GFH($DSP_ROM);

#######################################
# Read mode(2G/3G), debug/release flag, platform info, project info, etc. from modem.img
#######################################
my $md_file_size = -s $MD_IMG;

open(MODEM, "<$MD_IMG") or die "Can NOT open file $MD_IMG\n";
binmode(MODEM);
seek(MODEM, $md_file_size - $length3, $whence) or die "Can NOT seek to the position $position_point in \"$MD_IMG\"!\n";
read(MODEM, $buffer, $length5) or die "Failed to read the file \"$MD_IMG\"!\n";
($header_str, $temp, $MD_IMG_DEBUG, $MD_IMG_MODE, $MD_IMG_PLATFORM, $temp1, $MD_IMG_PROJECT_ID) = unpack("A12 L L L A16 A64 A64", $buffer);

die "[MODEM CHECK FAILED]: Reading from MODEM failed! No CHECK_HEADER info!\n" if ($header_str ne "CHECK_HEADER") ;
close(MODEM);


#######################################
# Output debug information
#######################################
my $debug = 0; # output debug info.

if ($debug) {
	print "\n=============\n";
	print "Info from DSP_BL\n";
	print "DSP_BL is $DSP_BL\n";
	print "\$DSP_BL_DEBUG = $DSP_BL_DEBUG [" . sprintf("0x%08x",$DSP_BL_DEBUG) . "]\n";
	print "\$DSP_BL_MODE = $DSP_BL_MODE [" . sprintf("0x%08x",$DSP_BL_MODE) . "]\n";
	print "\$DSP_BL_FILE_TYPE = $DSP_BL_FILE_TYPE [" . sprintf("0x%04x",$DSP_BL_FILE_TYPE) . "]\n";
	print "\$DSP_BL_PLATFORM = $DSP_BL_PLATFORM\n";
	print "\$DSP_BL_PROJECT_ID = $DSP_BL_PROJECT_ID\n";

	print "\n=============\n";
	print "Info from DSP_ROM\n";
	print "DSP_ROM is $DSP_ROM\n";
	print "\$DSP_ROM_DEBUG = $DSP_ROM_DEBUG [" . sprintf("0x%08x",$DSP_ROM_DEBUG) . "]\n";
	print "\$DSP_ROM_MODE = $DSP_ROM_MODE [" . sprintf("0x%08x",$DSP_ROM_MODE) . "]\n";
	print "\$DSP_ROM_FILE_TYPE = $DSP_ROM_FILE_TYPE [" . sprintf("0x%04x",$DSP_ROM_FILE_TYPE) . "]\n";
	print "\$DSP_ROM_PLATFORM = $DSP_ROM_PLATFORM\n";
	print "\$DSP_ROM_PROJECT_ID = $DSP_ROM_PROJECT_ID\n";
	
	
	print "\n=============\n";
	print "Info from modem image\n";
	print "modem image is $MD_IMG\n";
	print "\$MD_IMG_DEBUG = $MD_IMG_DEBUG [" . sprintf("0x%08x",$MD_IMG_DEBUG) . "]\n";
	print "\$MD_IMG_MODE = $MD_IMG_MODE [" . sprintf("0x%08x",$MD_IMG_MODE) . "]\n";
	print "\$MD_IMG_PLATFORM = $MD_IMG_PLATFORM\n";
	print "\$MD_IMG_PROJECT_ID = $MD_IMG_PROJECT_ID\n";
	
	print "\n";
}

#######################################
# Check file type of DSP_BL and DSP_ROM
# rules:
## 0x0003 stands for DSP_BL
## 0x0104 stands for DSP_ROM
#######################################
if ($DSP_BL_FILE_TYPE != 0x0003) {
	$errCnt++;
	warn "[MODEM CHECK FAILED]: Wrong file type, NOT DSP bootloader!\n"
     . "[MODEM CHECK FAILED]: \"$DSP_BL\" ==> " . sprintf("%s%04X", "0x", $DSP_BL_FILE_TYPE) ."\n";
}

if ($DSP_ROM_FILE_TYPE != 0x0104) {
	$errCnt++;
	warn "[MODEM CHECK FAILED]: Wrong file type, NOT DSP ROM!\n"
     . "[MODEM CHECK FAILED]: \"$DSP_ROM\" ==> " . sprintf("%s%04X", "0x", $DSP_ROM_FILE_TYPE) . "\n";
}
die "[MODEM CHECK FAILED]: Please check the modem file type!\n" if ($errCnt);

#######################################
# Check file consistency of DSP_BL/DSP_ROM
## debug/release
#######################################
my $debugErrCnt = 0;

$debugErrCnt++ if ($DSP_BL_DEBUG != $DSP_ROM_DEBUG);

if ($debugErrCnt) {
	warn "[MODEM CHECK FAILED]: Inconsistent debug/release mode!\n"
     . "[MODEM CHECK FAILED]: \"$DSP_BL\" ==> " . &getMODE (DEBUG, $DSP_BL_DEBUG) . "\n"
     . "[MODEM CHECK FAILED]: \"$DSP_ROM\" ==> " . &getMODE (DEBUG, $DSP_ROM_DEBUG) . "\n\n";
}

#######################################
# Check file consistency of DSP_BL/DSP_ROM/modem.img and MTK_MODEM_SUPPORT
## 2G/3G
#######################################
my $modeErrCnt = 0;

$modeErrCnt++ if (($DSP_BL_MODE != $DSP_ROM_MODE) || ($DSP_BL_MODE != $MD_IMG_MODE) || ($DSP_ROM_MODE != $MD_IMG_MODE));
$modeErrCnt++ if (($DSP_BL_MODE != $mode) || ($DSP_ROM_MODE != $mode) || ($MD_IMG_MODE != $mode));

if ($modeErrCnt) {
	warn "[MODEM CHECK FAILED]: Inconsistent 2G/3G mode!\n"
     . "[MODEM CHECK FAILED]: \"$DSP_BL\" ==> " . &getMODE (MODE, $DSP_BL_MODE) . "\n"
     . "[MODEM CHECK FAILED]: \"$DSP_ROM\" ==> " . &getMODE (MODE, $DSP_ROM_MODE) . "\n"
     . "[MODEM CHECK FAILED]: \"$MD_IMG\" ==> " . &getMODE (MODE, $MD_IMG_MODE) . "\n"
     . "[MODEM CHECK FAILED]: \"MTK_MODEM_SUPPORT\" ==> " .  &getMODE (MODE, $mode) . "\n\n";
}

#######################################
# Check consistency of DSP_BL/DSP_ROM/modem.img and MTK_PLATFORM
## platform
#######################################
my $platformErrCnt = 0;

$DSP_BL_PLATFORM =~ s/_(.*)//; $DSP_BL_CHIPVER = $1;
$DSP_ROM_PLATFORM =~ s/_(.*)//; $DSP_ROM_CHIPVER = $1;
$MD_IMG_PLATFORM =~ s/_(.*)//; $MD_IMG_CHIPVER = $1;

$platformErrCnt++ if (($DSP_BL_PLATFORM ne $DSP_ROM_PLATFORM) || ($DSP_BL_PLATFORM ne $MD_IMG_PLATFORM) || ($DSP_BL_PLATFORM ne $MTK_PLATFORM));
$platformErrCnt++ if (($DSP_ROM_PLATFORM ne $MD_IMG_PLATFORM) || ($DSP_ROM_PLATFORM ne $MTK_PLATFORM) || ($MD_IMG_PLATFORM ne $MTK_PLATFORM));

if ($platformErrCnt) {
	warn "[MODEM CHECK FAILED]: Inconsistent platform!\n"
     . "[MODEM CHECK FAILED]: \"$DSP_BL\" ==> " . $DSP_BL_PLATFORM . "\n"
     . "[MODEM CHECK FAILED]: \"$DSP_ROM\" ==> " . $DSP_ROM_PLATFORM . "\n"
     . "[MODEM CHECK FAILED]: \"$MD_IMG\" ==> " . $MD_IMG_PLATFORM . "\n"
     . "[MODEM CHECK FAILED]: MTK_PLATFORM ==> $MTK_PLATFORM\n\n";
}

#######################################
# Check consistency of DSP_BL/DSP_ROM/modem.img and MTK_PLATFORM
## chip version
#######################################
my $chipverErrCnt = 0;

$chipverErrCnt++ if (($DSP_BL_CHIPVER ne $DSP_ROM_CHIPVER) || ($DSP_ROM_CHIPVER ne $MD_IMG_CHIPVER) || ($DSP_BL_CHIPVER ne $MD_IMG_CHIPVER));

if ($chipverErrCnt) {
	warn "[MODEM CHECK FAILED]: Inconsistent chip version!\n"
     . "[MODEM CHECK FAILED]: \"$DSP_BL\" ==> " . $DSP_BL_CHIPVER . "\n"
     . "[MODEM CHECK FAILED]: \"$DSP_ROM\" ==> " . $DSP_ROM_CHIPVER . "\n"
     . "[MODEM CHECK FAILED]: \"$MD_IMG\" ==> " . $MD_IMG_CHIPVER . "\n\n";
}

#######################################
# Check consistency of DSP_ROM and modem.img
## project ID (branch/week info)
## skip check DSP_BL 
#######################################
my $projectErrCnt = 0;

#$projectErrCnt++ if ($DSP_ROM_PROJECT_ID ne $MD_IMG_PROJECT_ID);

if ($projectErrCnt) {
	warn "[MODEM CHECK FAILED]: Inconsistent project ID info!\n"
     . "[MODEM CHECK FAILED]: \"$DSP_ROM\" ==> " . $DSP_ROM_PROJECT_ID . "\n"
     . "[MODEM CHECK FAILED]: \"$MD_IMG\" ==> " . $MD_IMG_PROJECT_ID . "\n\n";
}

#######################################
# print error message if any and exit
#######################################
$errCnt = $errCnt + $modeErrCnt + $debugErrCnt + $platformErrCnt + $chipverErrCnt + $projectErrCnt;
die "[MODEM CHECK FAILED]: Please check the modem images inconsistency!\n" if ($errCnt);

print "[MODEM CHECK INFO]: Check modem image DONE!\n";
exit 0;

sub Parse_GFH {
	my $old_GFH = 0;
	my $new_GFH = 0;
	my $finish_sign = 0;
	my $position_point = 0;
	my $buffer;
	my $temp;
	my $struct_size = 1;
	my $struct_type = 0;
	my $FILE_MODE;
	my $FILE_DEBUG;
	my $FILE_TYPE;
	my $PLATFORM_INFO;
	my $PROJECT_ID;
	my ($parse_file) = @_;

	my $file_size = -s $parse_file;
	
	open(FILEHANDLE, "<$parse_file") or die "Can NOT open file $parse_file\n"; 
	binmode(FILEHANDLE);

	while ((!$finish_sign) && ($position_point < ($file_size - $magic_length))) {
		seek(FILEHANDLE, $position_point, $whence) or die "Can NOT seek to the position $position_point in \"$parse_file\"!\n";
		read(FILEHANDLE, $buffer, $magic_length) == $magic_length or die "Failed to read the file \"$parse_file\"!\n";
		my $magic_num = unpack("A*", $buffer); # Should be magic number "MMM" (4D4D4D)
		
		if ($magic_num eq "MMM") { # GFH structure
			seek(FILEHANDLE, $position_point+$magic_length+1, $whence) or die "Can NOT seek to the position $position_point+$magic_length+1 in \"$parse_file\"!\n";
			read(FILEHANDLE, $buffer, $length1) == $length1 or die "Failed to read the file \"$parse_file\"!\n";
			($struct_size, $struct_type) = unpack("S S", $buffer);

			if ($struct_type == 0x00) { # old GFH with info of file type
				$old_GFH = 1;
				seek(FILEHANDLE, $position_point+$position1, $whence) or die "Can NOT seek to the position $position_point+$position1 in \"$parse_file\"!\n";
				read(FILEHANDLE, $buffer, $length1) == $length1 or die "Failed to read the file \"$parse_file\"!\n";
				($temp, $FILE_TYPE) = unpack("H4S", $buffer);	
			} elsif ($struct_type == 0x104){ # new GFH with info of project platform and version
				$new_GFH = 1;
				seek(FILEHANDLE, $position_point+$position2, $whence) or die "Can NOT seek to the position $position_point+$position2 in \"$parse_file\"!\n";
				read(FILEHANDLE, $buffer, $length4) == $length4 or die "Failed to read the file \"$parse_file\"!\n";
				($FILE_DEBUG, $FILE_MODE, $PLATFORM_INFO, $PROJECT_ID) = unpack("L L A16 A64", $buffer);	
			} else {
#				print "Not a valid GFH, skip\n";
			}
		} else {
#			print "Not GFH, will exit parsing...\n";
			$finish_sign = 1;
		}
		$position_point  += $struct_size;
		if ($old_GFH && $new_GFH) {
			$finish_sign = 1; 
		}
	}
	close(FILEHANDLE);
	return ($FILE_DEBUG, $FILE_MODE, $FILE_TYPE,	 $PLATFORM_INFO, $PROJECT_ID);
}

sub getMODE {
	my ($type,$type_value) = @_;
	if ($MTK_PLATFORM eq "MT6573") {
		if ($type_value == 3) {
			$return_tpye = "TDD_3G" if ($type eq "MODE");
		} elsif($type_value == 2) {
			$return_tpye = "3G" if ($type eq "MODE");
			$return_tpye = "release" if ($type eq "DEBUG");
		} elsif($type_value == 1) {
			$return_tpye = "2G" if ($type eq "MODE");
			$return_tpye = "debug" if ($type eq "DEBUG");
		} else {
			$return_tpye = "invalid";
		}
	} else {
		if ($type_value == 2) {
			$return_tpye = "3G" if ($type eq "MODE");
			$return_tpye = "release" if ($type eq "DEBUG");
		} elsif($type_value == 1) {
			$return_tpye = "2G" if ($type eq "MODE");
			$return_tpye = "debug" if ($type eq "DEBUG");
		} else {
			$return_tpye = "invalid";
		}
	}
	return $return_tpye;
}

sub usage
{
	print <<"__EOFUSAGE";

Usage:
$0 DSP_BL DSP_ROM MODEM_IMG [MTK_MODEM_SUPPORT] [MTK_PLATFORM]

DSP_BL            DSP bootloader image
DSP_ROM           DSP rom image
MODEM_IMG         Modem image
MTK_MODEM_SUPPORT    current project used modem mode
                     (ex. modem_3g, modem_2g)
MTK_PLATFORM         current project platform
                     (ex. MT6516, MT6573)
                  
__EOFUSAGE
	exit 1;
}
