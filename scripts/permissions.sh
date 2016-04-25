#!/bin/bash



#http://superuser.com/questions/698939/fix-multiple-user-permissions-recursively-for-one-directory
#Normalize permissions
sudo chown -R jamesgeorge:staff .
sudo chmod -R u=rwX,go=rX .
sudo chmod -R -N .