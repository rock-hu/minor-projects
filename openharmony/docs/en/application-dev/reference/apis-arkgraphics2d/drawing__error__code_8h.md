# drawing_error_code.h


## Overview

The **drawing_error_code.h** file declares the functions related to the error code in the drawing module.

**File to include**: &lt;native_drawing/drawing_error_code.h&gt;

**Library**: libnative_drawing.so

**Since**: 12

**Related module**: [Drawing](_drawing.md)


## Summary


### Types

| Name| Description| 
| -------- | -------- |
| typedef enum [OH_Drawing_ErrorCode](_drawing.md#oh_drawing_errorcode)  [OH_Drawing_ErrorCode](_drawing.md#oh_drawing_errorcode) | Defines an enum for the error codes that may be generated by the module.| 


### Enums

| Name| Description| 
| -------- | -------- |
| [OH_Drawing_ErrorCode](_drawing.md#oh_drawing_errorcode) { OH_DRAWING_SUCCESS = 0, OH_DRAWING_ERROR_NO_PERMISSION = 201, OH_DRAWING_ERROR_INVALID_PARAMETER = 401, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE = 26200001,OH_DRAWING_ERROR_ALLOCATION_FAILED = 26200002 } | Enumerates the error codes that may be generated by the module.| 


### Functions

| Name| Description| 
| -------- | -------- |
| [OH_Drawing_ErrorCode](_drawing.md#oh_drawing_errorcode)  [OH_Drawing_ErrorCodeGet](_drawing.md#oh_drawing_errorcodeget) () | Obtains the latest error code of the module. After the function is successfully executed, the error code returned by this function will not be modified.| 
