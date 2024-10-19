#!/bin/bash
while IFS="," read -r host organization repository
do
   echo $host $organization $repository
   mkdir $organization
   cd $organization
   rm -rf ${repository}
   # git clone git@$host:$organization/$repository.git
   git clone https://${GITEE_USERNAME}:${GITEE_TOKEN}@${host}/${organization}/${repository}.git
   rm -rf ${repository}/.git 
   cd ..
done < harmonyos_samples.csv