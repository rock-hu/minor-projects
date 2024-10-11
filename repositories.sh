#!/bin/bash
while IFS="," read -r host organization repository
do
   echo $host $organization $repository
   # git@gitee.com:harmonyos_samples/scan-kit_-sample-code_-clientdemo_-arkts.git
   # https://gitee.com/harmonyos_samples/scan-kit_-sample-code_-clientdemo_-arkts.git
   mkdir $organization
   cd $organization
   rm -rf ${repository}
   echo "git@$host:$organization/$repository.git"
   git clone git@$host:$organization/$repository.git
   # git clone https://${host}/${organization}/${repository}.git
   rm -rf ${repository}/.git 
   cd ..
done < harmonyos_codelabs.csv