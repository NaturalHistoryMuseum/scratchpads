#!/bin/bash

# Make doubly sure we're in the right directory.
cd /var/www/html/sites

# Get a list of all the directories and CD in to them
for i in $( ls -l|grep ^d|sed -r "s|.*:[0-9][0-9]\ ||"|grep -v all$ ); do
	echo $i
	cd $i
	rm settings.php
	cp ../settings.php~ settings.php
	chown apache:users settings.php
	chmod 675 settings.php
	cd ..
done
