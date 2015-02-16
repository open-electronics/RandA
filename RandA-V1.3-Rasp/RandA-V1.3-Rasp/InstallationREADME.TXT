RandA installation guide.
If you use a new Raspbian OS distribution, run "sudo raspi-config" first of all.
Set  "Internationalisation Options - Change Locale" (it_IT ISO-8859-1 or 
euro ISO for Italy or proper country selection in long list).
(Use space bar for set <tab> for move and <return> to confirm)
Then set "Internationalisation Options - Change Timezone"(Europe-Rome for Italy)
Next step : Expand Filesystem. And reboot.
These only steps are required for basic functionality.
Actually, recent Raspbian distribution make SSH protocol active for default.
For this reason, you can use a console SSH program (like MobaXterm or Putty)
immediately, also before raspi-config. (i.e. you don't need keyboard and screen
directly connected at all).
Default username = pi, password = raspberry.
Moreover, Java is installed. But RandA installation defines JAVA_HOME as:
JAVA_HOME="/usr/lib/jvm/jdk-8-oracle-arm-vfp-hflt"
If your Raspbian has different location for Java directory, you have to edit
a couple of files after RandA installation: 
file /home/apache-tomcat-7.0.47/startWebS.sh and stopWebS.sh 
Just modify first line where JAVA_HOME is defined. 
Otherwise WEB server doesn't work.

Start RandA installation.

Extract RandAinstall.sh script and RandAinstall.tar.gz archive from RandARasp.zip.
Load these files to Raspberry using MobaXterm (or equivalent) into /home/pi 
(default home directory for pi user)
Run RandAinstall.sh script; in other words:

           chmod 777 RandAinstall.sh
           ./RandAinstall.sh
           
This script copies output message to a log file: RandAinstall.log.
(Note: if any problem with script, check if you have modified format from Linux 
(line feed only) to Window (lf cr))
This installation creates a directory RandAinstall (extracted from archive). But,
a the end of process only few important files remain in this directory, just for 
backup.

Because the packages installation steps (arduino (IDE) and codeblocks), internet 
connection is required.
You can install arduino later; but after installation you need to copy modified
library /usr/share/arduino/lib/pde.jar  from RandAinstall directory.

If everything is ok reboot Raspberry to start new behaviour :
             
             sudo reboot
             and now you can use  button to switch on/off

If something has gone wrong, you can extract RandARemove.sh from zip file 
and use it. This script restore principal files (rc.local etc.), deletes
files installed and remove arduino IDE installation; but doesn't remove 
codeblocks , Java ide and libraries : wiringPi, pi4j and javamail.
Because these software can be used also without RandA.

To use JavaMail you have to edit file /home/pi/bin/Mail.properties and put there
your personal data for mail server (username and password).

Attention!
Randa installation overwrites : /home/pi/bin and /etc/init.d/ups-monitor.
If these directory/file exists before, please save it with a backup name before 
installation.

 