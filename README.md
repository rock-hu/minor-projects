# minor-projects

```mermaid
flowchart LR

Action[GitHub Action] --> Push[On push]
Action[GitHub Action] --> Schdule[On schedule]
GitHub --> CSV
CSV --> Action
Calendar --> Action
```

## github actionis    

### badges    
|                    | on push                                                                                            | on schedule                                                                                                 |
| ------------------ | -------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------- |
| harmonyos_codelabs | ![](https://github.com/rock-hu/minor-projects/actions/workflows/harmonyos_codelabs.yaml/badge.svg) | ![](https://github.com/rock-hu/minor-projects/actions/workflows/harmonyos_codelabs-schedule.yaml/badge.svg) |
| harmonyos_samples  | ![](https://github.com/rock-hu/minor-projects/actions/workflows/harmonyos_samples.yaml/badge.svg)  | ![](https://github.com/rock-hu/minor-projects/actions/workflows/harmonyos_samples-schedule.yaml/badge.svg)  |
| openharmony        | ![](https://github.com/rock-hu/minor-projects/actions/workflows/openharmony.yaml/badge.svg)        | ![](https://github.com/rock-hu/minor-projects/actions/workflows/openharmony.yaml/badge.svg)                 |
| openharmony-sig    | ![](https://github.com/rock-hu/minor-projects/actions/workflows/openharmony-sig.yaml/badge.svg)    | ![](https://github.com/rock-hu/minor-projects/actions/workflows/openharmony-sig-schedule.yaml/badge.svg)    |


### on schedule    
```yml

name: harmonyos_codelabs-schedule
on:
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

### harmonyos_codelabs.sh    
```bash
#!/bin/bash
while IFS="," read -r host organization repository
do
   echo $host $organization $repository
   mkdir $organization
   cd $organization
   rm -rf ${repository}
   echo "git clone ${host}/${organization}/${repository} ..."
   git clone https://${GITEE_USERNAME}:${GITEE_TOKEN}@${host}/${organization}/${repository}.git
   rm -rf ${repository}/.git 
   cd ..
done < harmonyos_codelabs.csv
```


## harmonyos_codelabs.csv    
```
gitee.com,harmonyos_codelabs,httpsrequest
gitee.com,harmonyos_codelabs,mapkit_codelab_demo
```

