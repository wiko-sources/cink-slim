#!/usr/bin/perl

##########################################################
# Initialize Variables
##########################################################
my $prj = $ARGV[0];
my $custom_dir = $ARGV[1];
my $secro_ac = $ARGV[2];
my $secro_out = $ARGV[3];
my $sml_dir = "mediatek/custom/$custom_dir/security/sml_auth";
my $secro_tool = "mediatek/build/tools/SecRo/SECRO_POST";

print " Project           =  $prj\n";
print " Custom Directory  =  $custom_dir\n";

##########################################################
# SecRo Post Processing
##########################################################

my $ac_region = "mediatek/custom/$custom_dir/secro/AC_REGION";
my $and_secro = "mediatek/custom/$custom_dir/secro/AND_SECURE_RO";
my $md_secro = "mediatek/custom/$custom_dir/secro/SECURE_RO";

system("chmod 777 $ac_region") == 0 or die "can't configure $ac_region as writable";
print "MTK_SEC_SECRO_AC_SUPPORT = $secro_ac\n";
if (${secro_ac} eq "yes")
{		
	system("./$secro_tool $sml_dir/SML_ENCODE_KEY.ini $and_secro $md_secro $ac_region $secro_out") == 0 or die "SECRO POST Tool return error\n";
}
