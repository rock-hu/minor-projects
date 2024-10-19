# minor-projects

[Importing an external Git repository using the command line](https://docs.github.com/en/migrations/importing-source-code/using-the-command-line-to-import-source-code/importing-an-external-git-repository-using-the-command-line) 

```
gitee.com,harmonyos_codelabs,network-boost-kit-codelab-arkts
gitee.com,harmonyos_samples,scan-kit_-sample-code_-clientdemo_-arkts
```

```bash
git clone --bare https://external-host.com/EXTUSER/REPO.git
```

```bash
cd REPO.git
git push --mirror https://github.com/USER/REPO.git
```

```bash
cd ..
rm -rf REPO.git
```

```bash
#!/bin/bash
while IFS="," read -r host organization repository
do
   echo $host $organization $repository
   mkdir $organization
   cd $organization
   rm -rf ${repository}
   git clone git@$host:$organization/$repository.git
   # git clone https://${host}/${organization}/${repository}.git
   rm -rf ${repository}/.git 
   cd ..
done < harmonyos_samples.csv
```


## github actionis    

[Accessing contextual information about workflow runs](https://docs.github.com/en/actions/writing-workflows/choosing-what-your-workflow-does/accessing-contextual-information-about-workflow-runs)


### badges    
|                    | on schedule                                                                                        | on push                                                                                                     |
| ------------------ | -------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------- |
| harmonyos_codelabs | ![](https://github.com/rock-hu/minor-projects/actions/workflows/harmonyos_codelabs.yaml/badge.svg) | ![](https://github.com/rock-hu/minor-projects/actions/workflows/harmonyos_codelabs-schedule.yaml/badge.svg) |
| harmonyos_samples  | ![](https://github.com/rock-hu/minor-projects/actions/workflows/harmonyos_samples.yaml/badge.svg)  | ![](https://github.com/rock-hu/minor-projects/actions/workflows/harmonyos_samples-schedule.yaml/badge.svg)  |
| openharmony        | ![](https://github.com/rock-hu/minor-projects/actions/workflows/openharmony.yaml/badge.svg)        | ![](https://github.com/rock-hu/minor-projects/actions/workflows/openharmony.yaml/badge.svg)                 |
| openharmony-sig    | ![](https://github.com/rock-hu/minor-projects/actions/workflows/openharmony-sig.yaml/badge.svg)    | ![](https://github.com/rock-hu/minor-projects/actions/workflows/openharmony-sig-schedule.yaml/badge.svg)    |


-rw-rw-r-- 1 rock rock 1685 10月 19 13:39 harmonyos_codelabs.csv
-rw-rw-r-- 1 rock rock 1749 10月 19 13:40 harmonyos_samples.csv
-rw-rw-r-- 1 rock rock  456 10月 19 13:41 openharmony.csv
-rw-rw-r-- 1 rock rock  302 10月 19 13:38 openharmony-sig.csv



### on schedule    
```yml

name: harmonyos_codelabs-schedule
on:
  # https://docs.github.com/en/actions/writing-workflows/choosing-when-your-workflow-runs/events-that-trigger-workflows
  schedule:
    - cron: "0 0 * * 1"
env:
  GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
  GITEE_USERNAME: ${{ secrets.GITEE_USERNAME }}
  GITEE_TOKEN: ${{ secrets.GITEE_TOKEN }}
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: ./harmonyos_codelabs.sh
      - run: git config --global user.email "rock.c.c.hu@gmail.com"
      - run: git config --global user.name "Rock Hu"
      - run: git remote set-url origin https://git:${GITHUB_TOKEN}@github.com/${GITHUB_REPOSITORY}.git
      - run: git status
      - run: git add .
      - run: git commit -m "${{ github.run_number }}"
      - run: git push
      - run: ls -l

```

### on push
```yml
name: harmonyos_codelabs
on:
  # https://docs.github.com/en/actions/writing-workflows/choosing-when-your-workflow-runs/events-that-trigger-workflows
  push:
    branches:
      - master
    paths:
      - "harmonyos_codelabs.csv"
env:
  GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
  GITEE_USERNAME: ${{ secrets.GITEE_USERNAME }}
  GITEE_TOKEN: ${{ secrets.GITEE_TOKEN }}
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: ./harmonyos_codelabs.sh
      - run: git config --global user.email "rock.c.c.hu@gmail.com"
      - run: git config --global user.name "Rock Hu"
      - run: git remote set-url origin https://git:${GITHUB_TOKEN}@github.com/${GITHUB_REPOSITORY}.git
      - run: git status
      - run: git add .
      - run: git commit -m "${{ github.run_number }}"
      - run: git push
      - run: ls -l

```


