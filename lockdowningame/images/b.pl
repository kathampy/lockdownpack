#!c:\perl\bin\perl.exe

opendir(DIR, ".");
@files = grep(/\.raw$/,readdir(DIR));
closedir(DIR);

open (OUT,">b.sh");
foreach $file (@files) {
	$temp = $file;
	$temp =~ s/\.raw$//i;
	print OUT "/usr/local/pspdev/bin/bin2c $file $temp.c $temp\n";
}
close (OUT);