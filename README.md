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


[harmonyos_samples/projects](https://gitee.com/organizations/harmonyos_samples/projects)
[harmonyos_codelabs/projects](https://gitee.com/organizations/harmonyos_codelabs/projects)