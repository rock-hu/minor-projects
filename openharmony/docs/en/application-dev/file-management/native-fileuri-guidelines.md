# FileUri Development (C/C++)

## When to Use

FileUri provides APIs for basic file URI operations, such as converting URIs to sandbox paths, converting sandbox paths to URIs, and obtaining URIs of directories where the specified URIs are located, facilitating URI access in file sharing services.

## Basic Concepts

**Result set**: path or URI that meets the service requirements.

## Constraints

- When converting a URI to a path, you are advised to use the system capability to obtain the URI source, for example, the URI returned by the picker, clipboard, dragging, and path-to-URI APIs provided by the system. If the URI combined by applications or users is converted, the converted path may fail to be accessed.

- To ensure data accuracy, only one object can be processed during the conversion or verification of a URI.

## Available APIs

For details about the APIs, see [File URI](../reference/apis-core-file-kit/fileuri.md).

| API| Description|
| -------- |-------|
| FileManagement_ErrCode OH_FileUri_GetUriFromPath(const char *path, unsigned int length, char **result)| Converts the input path to the URI of the application.<br>In this process, Chinese characters and non-digit characters in the path will be encoded to their corresponding ASCII codes and concatenated to the URI.|
| FileManagement_ErrCode OH_FileUri_GetPathFromUri(const char *uri, unsigned int length, char **result) | Converts the URI to the corresponding sandbox path.<br>1. During URI-to-path conversion, the ASCII code in the URI is decoded and then concatenated to the original position. The URI generated by a non-system API may contain characters beyond the ASCII code parsing range. As a result, the string cannot be concatenated.<br>2. The conversion is performed based on the string replacement rule specified by the system (the rule may change with the system evolution). During the conversion, the path is not verified, so that the conversion result may not be accessible.|
| FileManagement_ErrCode OH_FileUri_GetFullDirectoryUri(const char *uri, unsigned int length, char **result) | Obtains the URI of the path.<br>If the URI points to a file, the URI of the path is returned. If the URI points to a directory, the original string is returned without processing.<br>If the file specified by the URI does not exist or the attribute fails to be obtained, an empty string is returned.|
| bool OH_FileUri_IsValidUri(const char *uri, unsigned int length) | Checks whether the format of the input URI is correct. The system only checks whether the URI meets the format specifications defined by the system. The validity of the URI is not verified.|
| FileManagement_ErrCode OH_FileUri_GetFileName(const char *uri, unsigned int length, char **result) | Obtains the file name based on the given URI. (The ASCII code in the file name will be decoded and then concatenated to the original position.)|

## How to Develop

**Linking the Dynamic Library in the CMake Script**

Add the following library to **CMakeLists.txt**.

```txt
target_link_libraries(sample PUBLIC libohfileuri.so)
```

**Adding the Header File**

```c++
#include <filemanagement/file_uri/oh_file_uri.h>
```

1. Call **OH_FileUri_GetUriFromPath** to obtain the URI from a path. The memory allocated must be released using **free()**. <br>Example:

    ```c
    #include <cstring>

    void OH_FileUri_GetUriFromPathExample() {
        char *path = "/data/storage/el2/base/files/test.txt";
        unsigned int length = strlen(path);
        char *uriResult = NULL;
        FileManagement_ErrCode ret = OH_FileUri_GetUriFromPath(path, length ,&uriResult); 
        if (ret == 0 && uriResult !=NULL) {
            printf("pathUri=%s", uriResult); // The URI obtained by application a is file://com.example.demo/data/storage/el2/base/files/test.txt.
        }
        if (uriResult != NULL) {
            free(uriResult);
        }
    }    
    ```

2. Call **OH_FileUri_GetPathFromUri** to convert the URI into a path. The memory allocated must be released using **free()**. <br>Example:

    ```c
    #include <cstring>

    void OH_FileUri_GetPathFromUriExample() {
        char *uri = "file://com.example.demo/data/storage/el2/base/files/test.txt";
        unsigned int length = strlen(uri);
        char *pathResult = NULL;
        FileManagement_ErrCode ret = OH_FileUri_GetPathFromUri(uri, length, &pathResult);
        if (ret == 0 && pathResult != NULL) {
            printf("pathResult=%s", pathResult); // PathResult is /data/storage/el2/base/files/test.txt.
        }
        if (pathResult != NULL) {
            free(pathResult);
        }
    }
    ```

3. Call **OH_FileUri_GetFullDirectoryUri** to obtain the URI of the directory where the specified URI is located. The memory allocated must be released using **free()**. <br>Example:

    ```c
    #include <cstring>
    
    void OH_FileUri_GetFullDirectoryUriExample() {
        char *uri = "file://com.example.demo/data/storage/el2/base/files/test.txt";
        unsigned int length = strlen(uri);
        char *uriResult = NULL;
        FileManagement_ErrCode ret = OH_FileUri_GetFullDirectoryUri(uri, length, &uriResult);
        if (ret == 0 && uriResult != NULL) {
            printf("pathUri=%s",uriResult);// The URI obtained is file://com.example.demo/data/storage/el2/base/files/.
        }
        if (uriResult != NULL) {
            free(uriResult);
        }
    }
    ```

4. Call **OH_FileUri_IsValidUri** to check whether a URI is valid. <br>Example:

   ```c
    #include <cstring>
    
    void OH_FileUri_IsValidUriExample() {
        char *uri = "file://com.example.demo/data/storage/el2/base/files/test.txt";
        unsigned int length = strlen(uri);
        bool falgs = OH_FileUri_IsValidUri(uri, length);
        printf("The URI is valid? flags=%d", flags);
    }
   ```

5. Call **OH_FileUri_GetFileName** to obtain the file name from the URI. The memory allocated must be released using **free()**. <br>Example:

    ```c
    #include <cstring>
    
    void OH_FileUri_GetFileNameExample() {
        char *uri = "file://com.example.demo/data/storage/el2/base/files/test.txt";
        unsigned int length = strlen(uri);
        char *uriResult = NULL;
        FileManagement_ErrCode ret = OH_FileUri_GetFileName(uri, length, &uriResult);
        if (ret == 0 && uriResult != NULL) {
            printf("pathUri=%s",uriResult);// Obtain the file name test.txt from the URI.
        }
        if (uriResult != NULL) {
            free(uriResult);
        }
    }
    ```
