#!/bin/bash

# Make doubly sure we're in the right directory.
cd /var/www/html/sites

# Get a list of all the directories and CD in to them
for i in $( ls -1|grep -v all$|grep -v settings ); do
	echo $i
	cd $i
	rm settings.php
	cp ../settings.php.default settings.php
	chown apache:users settings.php
	chmod 675 settings.php
	cd ..
done
