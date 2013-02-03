#!usr/bin/perl
my $prj = $ARGV[0];
my $custom_dir = $ARGV[1];
my $secro_ac = $ARGV[2];
my $dir = "out/target/product/$prj";
my $cfg = "mediatek/custom/$prj/security/image_auth/IMG_AUTH_CFG.ini";
my $key = "mediatek/custom/$prj/security/image_auth/IMG_AUTH_KEY.ini";

##########################################################
# Dump Parameter
##########################################################
print "\n\n";
print "********************************************\n";
print " Dump Paramter \n";
print "********************************************\n";
print " Project          : $prj\n";
print " Custom Directory : $custom_dir\n";
print " SECRO AC         : $secro_ac\n";


##########################################################
# Create Folder
##########################################################
print "\n\n";
print "********************************************\n";
print " Create Folder \n";
print "********************************************\n";
`mkdir $dir/signed_bin` if ( ! -d "$dir/signed_bin" );
print "Image Dir '$dir'\n";
my $command = "mediatek/build/tools/SignTool/SignTool.sh";

##########################################################
# File Check
##########################################################
my @imgs_need_sign = ("boot.img", "logo.bin", "recovery.img", "secro.img", "system.img", "uboot_${prj}.bin", "userdata.img");
foreach my $img (@imgs_need_sign) {
	push (@miss_img, $img) if ( ! -e "$dir/$img");
}
die "@miss_img\nall the imgs above is NOT exsit\n" if (@miss_img > 0);

##########################################################
# BACKUP SECRO
##########################################################
my $secro_out = "out/target/product/$prj/secro.img";
my $secro_bak = "out/target/product/$prj/secro_bak.img";
system("cp -rf $secro_out $secro_bak") == 0 or die "backup SECRO fail\n";

##########################################################
# SECRO POST PROCESS
##########################################################
print "\n\n";
print "********************************************\n";
print " SecRo Post Processing \n";
print "********************************************\n";

my $secro_out = "out/target/product/$prj/secro.img";
my $secro_script = "mediatek/build/tools/SecRo/secro_post.pl";
if (${secro_ac} eq "yes")
{
	system("./$secro_script $custom_dir $prj $secro_ac $secro_out") == 0 or die "SECRO post process return error\n";
}

##########################################################
# Process Common Files
##########################################################
print "\n\n";
print "********************************************\n";
print " Sign Common Images \n";
print "********************************************\n";
foreach my $img (@imgs_need_sign) {
	if ( ! -e "$dir/$img") {
		warn "the $img is NOT exsit, please check\n";
		next;
	}
	my $signed_img = $img;
	$signed_img =~ s/\./-sign\./;
	print "Sign Image '$dir/$img' ...\n";
	system("$command $key $cfg $dir/$img $dir/signed_bin/$signed_img") == 0 or die "sign image fail";
}

sub print_system {
	my $command = $_[0];
	my $rslt = system($command);
	print "$command: $rslt\n";
	die "Failed to execute $command" if ($rslt != 0);
}

##########################################################
# Process EMMC Files
##########################################################
print "\n\n";
print "********************************************\n";
print " Sign EMMC Images \n";
print "********************************************\n";

my @imgs_need_sign = ("MBR", "EBR1", "EBR2");

foreach my $img (@imgs_need_sign) {
	if (-e "$dir/$img") {		
		print "Sign Image '$dir/$img' ...\n";
		system("$command $key $cfg $dir/$img $dir/signed_bin/${img}-sign") == 0 or die "sign EMMC image fail";
	}
}

my @imgs_need_sign = ("cache.img");

foreach my $img (@imgs_need_sign) {
	if (-e "$dir/$img") {		
		my $signed_img = $img;
		$signed_img =~ s/\./-sign\./;
		print "Sign Image '$dir/$img' ...\n";
		system("$command $key $cfg $dir/$img $dir/signed_bin/$signed_img") == 0 or die "sign EMMC image fail";
	}
}
##########################################################
# RESTORE SECRO
##########################################################
my $secro_out = "out/target/product/$prj/secro.img";
my $secro_bak = "out/target/product/$prj/secro_bak.img";
system("cp -rf $secro_bak $secro_out") == 0 or die "restore SECRO fail\n";
system("rm -rf $secro_bak") == 0 or die "remove backup SECRO fail\n";