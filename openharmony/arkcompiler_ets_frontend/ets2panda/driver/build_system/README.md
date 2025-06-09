# Build system

The build system is designed to compile ArkTS1.2 source files. It takes source files and build configurations as input, compiles the source files, invokes plugins for transformation, and generates a merged bytecode file.

## How to Run

The build system has two usage scenarios:

1. **Compiling the Deveco Project**: The build system is initiated by hvigor, which calls the build method in the `entry.js` file, passing in the `projectConfig` object to perform the compilation.

2. **Compiling the Local Test Project**: The build system is initiated by executing `node entry.js`, where the `build_config.json` file is passed as a compilation parameter to perform the compilation. The `build_config.json` is a mock to the `projectConfig` for scenario 1.

Below are the steps for compiling the local test project:

### step 1
Mock sdk 
There are 2 methods to mock sdk locally.  
Method 1: use soft links
```bash
mkdir -p test/mock_sdk/build-tools/ets2panda
cd test/mock_sdk/build-tools/ets2panda
ln -s ${path_to_ets2panda_lib} lib
ln -s ${path_to_ets2panda_stdlib} lib/stdlib
ln -s ${path_to_ets2panda_bin} bin
```

Method 2: specify `pandaSdkPath` and `pandaStdlibPath` in build_config.json


### step 2
Mock koala-wrapper
Modify `initKoalaWrapper` method in `src/init/process_build_config.ts`, set the `koalaWrapperPath` to the actual path in your environment. In OpenHarmony repo, it is now resides in `${path_to_oh_root}/developtools/ace_ets2bundle/koala_wrapper`

### step 3
Build build system source code
```bash
npm install
npm run build
```

### step 4
Build specify local test project.  
In test directory, there are some demo projects. Each project has its own `build_config.json`.  
Replace the path to your own path before running. For example, use sed command in linux:
```
sed -i 's|${absolute_path_to_build_system}|'"$(pwd)"'|g' test/*/build_config.json
```
Then run the following command to build
```bash
npm run mixed_hap:gen_abc  # build demo_mix_hap
npm run demo_hap:gen_abc   # build demo_hap
```
