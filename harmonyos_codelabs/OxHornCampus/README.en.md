# Xiliubeipo Village

### Introduction

This codelab introduces how to implement an app that displays the Xiliubeipo Village based on ArkUI, demonstrating the basic capabilities and animation effects of ArkUI as follows:

![](screenshots/device/example.en.gif)

### Concepts

- Tabs: a container that allows users to switch between content views through tabs. Each tab corresponds to a content 
- List: a component that provides a series of list items with the same width and contains the child component **ListItem**.
- TapGesture: identifies tap gestures with one, two, or more taps.
- PanGesture: triggers the pan gesture, which requires a minimum 5 vp movement distance of a finger on the screen.
- PinchGesture: triggers a pinch gesture, which requires two to five fingers with a minimum 5 vp distance between the fingers.
- Property animation: used to implement gradient when the universal properties of a component change, improving user experience.
- Custom dialog box: displayed by using the **CustomDialogController** class. You can set the style and content to your preference for a custom dialog box.
- Canvas: a component that is used to customize drawings.

### Permissions

N/A

### How to Use

1. When the app is opened, the images of Xiliubeipo Village landscape are displayed through a carousel on the splash page before you enter the home page.
2. On the Maps tab page, you can drag and zoom in or out the map to search for the corresponding landmark building.
3. On the Zones tab page, you can swipe up or down to view the zone cards of Xiliubeipo Village. You can also tap a card to view the zone details and high-definition building images.
4. On the Trains tab page, you can view the routes of the trains in Xiliubeipo Village.

### Constraints

1. The sample app is supported only on Huawei phones running the standard system.
2. HarmonyOS: HarmonyOS 5.0.5 Release or later
3. DevEco Studio: DevEco Studio 5.0.5 Release or later
4. HarmonyOS SDK: HarmonyOS 5.0.5 Release SDK or later
