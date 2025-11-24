# **Remote Communication Platform**
## Overview
Remote communication platform (RCP) provides network data request functionalities. Compared with the HTTP request capability in Network Kit, RCP is easier to use and stands out with the following features:

* Sending PATCH requests

* Setting BaseURL

* Canceling the specified or ongoing session request.

* Customizing certificate verification based on service requirements.

* Customizing DNS resolution.

* Caching responses by custom interceptors

* Capturing detailed tracing information during the HTTP request in a session

## Preview
![](screenshots/device/rcp_en.gif)

## Project Directory
````
├──entry/src/main/ets                                   // Code
│  ├──common
│  |  ├──CommonConstants.ets                            // Constants
│  |  └──Logger.ets                                     // Logging utility
│  ├──entryability
│  |  └──EntryAbility.ets                               // Entry ability
│  ├──entrybackupability
│  |  └──EntryBackupAbility.ets
│  ├──pages
│  |  ├──BaseRequest.ets                                // Basic structure and logic for making network requests
│  |  ├──DNSSetting.ets                                 // DNS settings
│  │  └──Index.ets                                      // Home page
│  |  ├──MultipartForm.ets                              // APIs for handling multipart form data, useful for handling complex form submissions
│  |  ├──RequestAndResponse.ets                         // Request and response example
│  |  └──TracingPoint.ets                               // Tracing and logging HTTP requests and responses
|  └──utils.ets
│     ├──Interceptors.ets                               // Utility for intercepting network requests and responses
│     └──NetworkStateSimulator.ets                      // Utility for simulating a slow-speed network
└──entry/src/main/resources                             // Static resources of the app
````
## Required Permissions
ohos.permission.INTERNET: allows an app to access Internet.

## How to Use
1. Click the "Basic Request" button to jump to the secondary page. Click the "Initiate PATCH Request" button to initiate a PATCH request. Click the "Set the Base Address of the URL in the Session" button to set the requested BaseUrl.
2. Click the "Submit Multiple Forms" button to jump to the secondary page, and enter your name in Form One. Switch to Form Two and select your hobbies. Click the "Submit" button to submit the form and display the relevant parameters.
3. Click the "Request and Response Blocking" button to jump to the secondary page, and click the "Get Image" button to obtain the address of the image that has not been blocked. Turn on the request cache switch and then click the "Get Image" button to obtain the address of the image after interception processing.
4. Click the "DNS Settings" button to jump to the secondary page. Click the "Customize DNS Server" button to customize the DNS server. Click the "Customize Static DNS" button to set up a static DNS server. Click the "Configure DNS on HTTP" button to configure the DNS Settings on HTTP, thereby achieving secure DNS resolution through the HTTPS endpoint.
5. Click the "Capture Details about HTTP Request/Response Flow" button to jump to the secondary page. After clicking the "Configure Details Collection" button, you can make relevant Settings and display the parameters of the Settings.
## Constraints
* The sample app is supported only on Huawei phones running the standard system.

* The HarmonyOS version must be HarmonyOS 5.0.5 Release or later.

* The DevEco Studio version must be DevEco Studio 5.0.5 Release or later.

* The HarmonyOS SDK version must be HarmonyOS 5.0.5 Release SDK or later.