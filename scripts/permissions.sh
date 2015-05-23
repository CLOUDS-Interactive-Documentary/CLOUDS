#!/bin/bash



#http://superuser.com/questions/698939/fix-multiple-user-permissions-recursively-for-one-directory
#Normalize permissions
sudo chown -R Winslow:staff .
sudo chmod -R u=rwX,go=rX .
sudo chmod -R -N .