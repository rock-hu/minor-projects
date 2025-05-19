# Description
This is a demo project that does not inherit the RNAbilty and switches from the native page to the RN page.


# Directory Structure
RNProject -- RN project 
NativeProject -- Native project


# Environment Setup
1. In the `entry` directory, run **ohpm i @rnoh/react-native-openharmony@x.x.x** to install the dependency.
2. Check whether the `oh-modules` folder is generated in the `NativeProject`, `entry`, and `library` directories.
3. Run **npm i @react-native-oh/react-native-harmony@x.x.x** in `RNProject/MainProject` to install the dependency.
4. Run **npm run dev** in `RNProject/MainProject` to generate the bundle.
4. Open `NativeProject` in DevEco Studio and run **Sync and Refresh Project**.
5. Click **run** in the upper right corner to start the project.
