#!/bin/bash
while IFS="," read -r host organization repository


organization=$(echo "$organization" | tr '[:upper:]' '[:lower:]')
repository=$(echo "$repository" | tr '[:upper:]' '[:lower:]')


do
   echo $host $organization $repository
   # git@gitee.com:harmonyos_samples/scan-kit_-sample-code_-clientdemo_-arkts.git
   # https://gitee.com/harmonyos_samples/scan-kit_-sample-code_-clientdemo_-arkts.git
   mkdir $organization
   cd $organization
   echo "git@$host:$organization/$repository.git"
   git clone git@$host:$organization/$repository.git
   # git clone https://${host}/${organization}/${repository}.git
   # rm -rf ${repository}/.git 
   cd ..
done < repositories.csv