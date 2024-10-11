# Application URI Description

## Basic URI Format

**uris** declared in **skills** of the **module.json5** file contains the following fields. Among them, **scheme** is mandatory. Other fields are valid only when **scheme** is configured.

- **scheme**: scheme name, for example, **http**, **https**, **file**, and **ftp**. Custom values are also supported.
- **host**: domain name or IP address, for example, developer.huawei.com or 127.0.0.1.
- **port**: port number, for example, 80 in developer.huawei.com:80.
- **path**: directory or file path on the DNS, for example, consumer in developer.huawei.com/consumer/.
- **pathStartWith**: prefix of the directory or file path on the DNS. It is used for prefix matching.
- **pathRegex**: regular expression of the directory or file path on the DNS. It is used for regular expression matching.
- **linkFeature**: application's function type, such as file opening, sharing, and navigation. The value is a string with a maximum of 127 bytes.

URIs can be expressed in different formats based on the available fields.

The following formats are possible when **scheme**, the combination of **scheme** and **host**, or the combination of **scheme**, **host**, and **port** is configured:

- `scheme://`
- `scheme://host`
- `scheme://host:port`    

When **path**, **pathStartWith**, or **pathRegex** is configured, the formats are as follows. If more than one of these three fields are configured and any field is matched, the matching result is true.

- **Full path expression**: scheme://host:port/path
- **Prefix expression**: scheme://host:port/pathStartWith
- **Regular expression**: scheme://host:port/pathRegex

> **NOTE**
> 
> If multiple applications are configured with the same URLs, these applications will be matched during application redirection, and a dialog box will be displayed for uses to select. For better user experience, you can use the **path** field to distinguish the application to start. For example, use **https://www.example.com/path1** to start target application 1 and use **https://www.example.com/path2** to start target application 2.

## Description of linkFeature

The use of the **linkFeature** field enables an application to deliver a more user-friendly redirection experience. (The declaration of the **linkFeature** field must be reviewed by the application market before being released.) The use scenarios are as follows:

1. Identification of applications of the same type: When the caller starts a vertical application (for example, navigation applications), the system identifies the matched applications based on the **linkFeature** field and displays the applications on the vertical domain panel.

    |Value|Description|
    |---|---|
    |AppStorageMgmt|Clears cache data in the application sandbox directory.|
    |FileOpen|Opens a file.|
    |Navigation|Provides navigation.|
    |RoutePlan|Plans a route.|
    |PlaceSerach|Searches a location.|

2. One-touch return: When a user switches from application A to application B, application B calls the [quick return API](../reference/apis-ability-kit/js-apis-inner-application-uiAbilityContext.md#uiabilitycontextbacktocallerabilitywithresult12) to return to application A. For example, if application A is redirected to the payment page of application B and application B has applied for the linkfeature of payment, the user can return to application A at one touch after finishing the payment in application B.

    |Value|Description|
    |---|---|
    |Login|Common login and authorized login.|
    |Pay|Payment and cashier.|
    |Share|Sharing.|

## Example


The following uses the authorized login scenario as an example:

```json
"uris": [
    {
        "scheme": "https",
        "host": "developer.huawei.com",
        "path": "consumer",
        "linkFeature": "Login"  
    }
]
```
