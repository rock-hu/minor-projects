/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "console_log_impl.h"
#if IS_ENABLED(CONSOLE_LOG_OUTPUT)
#include "js_app_environment.h"
#if (defined(FEATURE_USER_MC_LOG_PRINTF) && (FEATURE_USER_MC_LOG_PRINTF == 1))
#include "product_adapter.h"
#endif // FEATURE_USER_MC_LOG_PRINTF
#if (defined(FEATURE_ACELITE_HI_LOG_PRINTF) && (FEATURE_ACELITE_HI_LOG_PRINTF == 1))
#undef LOG_DOMAIN
#undef LOG_TAG
#define LOG_DOMAIN 0xD003B00
#define LOG_TAG "JS-3RD-APP"
#ifndef __ICCARM__
#include "hilog/log.h"
#else
#include "hilog_lite/log.h"
#endif
#endif // FEATURE_ACELITE_HI_LOG_PRINTF
#if (defined(TARGET_SIMULATOR) && (TARGET_SIMULATOR == 1))
#include "handler.h"
#endif
#include <stdio.h>
#include <string.h>

namespace OHOS {
namespace ACELite {
#ifdef CONSOLE_LOG_LINE_MAX_LENGTH
const int16_t LOG_BUFFER_SIZE = CONSOLE_LOG_LINE_MAX_LENGTH;
#else
const int16_t LOG_BUFFER_SIZE = 256; // use 256 as default if it's not config
#endif // CONSOLE_LOG_LINE_MAX_LENGTH
jerry_value_t LogNative(const LogLevel logLevel,
                        const jerry_value_t *args,
                        const jerry_length_t argc)
{
    // print out log level if needed
    LogOutLevel(logLevel);

    jerry_value_t retVal = jerry_create_undefined();
    for (jerry_length_t argIndex = 0; argIndex < argc; argIndex++) {
        jerry_value_t strVal = jerry_value_to_string(args[argIndex]);
        if (jerry_value_is_error(strVal)) {
            retVal = strVal;
            break;
        }

        jerry_length_t substrPos = 0;
        jerry_length_t length = jerry_get_utf8_string_length(strVal);
        const uint16_t bufLength = LOG_BUFFER_SIZE;
        jerry_char_t substrBuf[bufLength] = {0};

        do {
            jerry_size_t substrSize =
                jerry_substring_to_char_buffer(strVal, substrPos, length, substrBuf, bufLength - 1);

            jerry_char_t *bufEndPos = substrBuf + substrSize;

            for (jerry_char_t *bufPos = substrBuf; bufPos < bufEndPos; bufPos++) {
                if ((*bufPos & 0xc0) != 0x80) {
                    substrPos++;
                }
                char chr = static_cast<char>(*bufPos);

                if (chr != '\0') {
                    LogChar(chr, logLevel);
                    continue;
                }
            }
        } while (length > substrPos);
        jerry_release_value(strVal);
    }
    // output end
    LogChar('\n', logLevel, true);
    FlushOutput();
    return retVal;
}

void LogOutLevel(const LogLevel logLevel)
{
    switch (logLevel) {
        case LOG_LEVEL_ERR:
            LogString(logLevel, "[Console Error] "); // console error
            break;
        case LOG_LEVEL_WARN:
            LogString(logLevel, "[Console Warn] "); // console warn
            break;
        case LOG_LEVEL_INFO:
            LogString(logLevel, "[Console Info] "); // console info
            break;
        case LOG_LEVEL_DEBUG:
            LogString(logLevel, "[Console Debug] "); // console debug
            break;
        case LOG_LEVEL_TRACE:
            LogString(logLevel, "[Console Trace] "); // console trace, this is not supported yet
            break;
        case LOG_LEVEL_NONE:
            LogString(logLevel, "[Console Debug] "); // console.log(), default apply the DEBUG level
            break;
        default: // just return for not supported log level
            break;
    }
}

/**
 * @brief: the str to print out.
 *
 * @param str the string to print out
 */
void LogString(const LogLevel logLevel, const char * const str)
{
    if (str == nullptr) {
        return;
    }
#if ((FEATURE_ACELITE_HI_LOG_PRINTF == 1) || (FEATURE_USER_MC_LOG_PRINTF == 1))
    size_t strLength = strlen(str);
    for (size_t i = 0; i < strLength; i++) {
        LogChar(str[i], logLevel, false);
    }
#else
    Output(logLevel, str, strlen(str));
#endif
}

#if !defined(TARGET_SIMULATOR) || (TARGET_SIMULATOR != 1)
static char logBuffer[LOG_BUFFER_SIZE] = {0};
static uint16_t logBufferIndex = 0;
#endif

void LogChar(char c, const LogLevel logLevel, bool endFlag)
{
#if (defined(TARGET_SIMULATOR) && (TARGET_SIMULATOR == 1))
    char tempBuffer[2] = {0};
    tempBuffer[0] = c;
    Output(logLevel, tempBuffer, 1);
#else
    logBuffer[logBufferIndex++] = c;
    if ((logBufferIndex == (LOG_BUFFER_SIZE - 1)) || (c == '\n')) {
        if ((c == '\n') && (logBufferIndex > 0)) {
            logBufferIndex--; // will trace out line separator after print the content out
        }
        logBuffer[logBufferIndex] = '\0';
        Output(logLevel, logBuffer, logBufferIndex);
        logBufferIndex = 0;
        if (c == '\n' || !endFlag) {
            // this is the newline during the console log, need to append the loglevel prefix,
            // example: console.log("aa\nbb");
#if ((FEATURE_ACELITE_HI_LOG_PRINTF != 1) && (FEATURE_USER_MC_LOG_PRINTF != 1))
            Output(logLevel, "\n", 1); // hilog will trace our the line separator directly
#endif
            if (!endFlag) {
                LogOutLevel(logLevel);
            }
        }
    }
#endif
}

#if (defined(FEATURE_ACELITE_HI_LOG_PRINTF) && (FEATURE_ACELITE_HI_LOG_PRINTF == 1))
static void OutputToHiLog(const LogLevel logLevel, const char * const str)
{
    switch (logLevel) {
        case LOG_LEVEL_ERR:
            HILOG_ERROR(HILOG_MODULE_APP, "%{public}s", str);
            break;
        case LOG_LEVEL_WARN:
            HILOG_WARN(HILOG_MODULE_APP, "%{public}s", str);
            break;
        case LOG_LEVEL_INFO:
            HILOG_INFO(HILOG_MODULE_APP, "%{public}s", str);
            break;
        case LOG_LEVEL_DEBUG:
            HILOG_DEBUG(HILOG_MODULE_APP, "%{public}s", str);
            break;
        case LOG_LEVEL_TRACE:
            HILOG_INFO(HILOG_MODULE_APP, "%{public}s", str);
            break;
        case LOG_LEVEL_NONE:
            HILOG_DEBUG(HILOG_MODULE_APP, "%{public}s", str);
            break;
        default:
            break;
    }
}
#elif (defined(FEATURE_USER_MC_LOG_PRINTF) && (FEATURE_USER_MC_LOG_PRINTF == 1))
static void OutputToHiLog(const LogLevel logLevel, const char * const str)
{
    switch (logLevel) {
        case LOG_LEVEL_ERR:
            ProductAdapter::OutputJSConsoleLog((uint8_t)(LOG_LEVEL_ERR), str);
            break;
        case LOG_LEVEL_WARN:
            ProductAdapter::OutputJSConsoleLog((uint8_t)(LOG_LEVEL_WARN), str);
            break;
        case LOG_LEVEL_INFO:
            ProductAdapter::OutputJSConsoleLog((uint8_t)(LOG_LEVEL_INFO), str);
            break;
        case LOG_LEVEL_DEBUG:
            // fall through
        case LOG_LEVEL_TRACE:
            // fall through
        case LOG_LEVEL_NONE:
            ProductAdapter::OutputJSConsoleLog((uint8_t)(LOG_LEVEL_DEBUG), str);
            break;
        default:
            break;
    }
}
#endif

#ifdef TDD_ASSERTIONS
static JSLogOutputExtraHandler g_logOutputExtraHandler = nullptr;
// add extra hanlder for TDD test cases
void RegisterJSLogOutputHandler(JSLogOutputExtraHandler extraHandler)
{
    g_logOutputExtraHandler = extraHandler;
}
#endif // TDD_ASSERTIONS

void Output(const LogLevel logLevel, const char * const str, const uint8_t length)
{
    if (str == nullptr) {
        return;
    }
    (void)length;
    Debugger::GetInstance().Output(str);
#if ((FEATURE_ACELITE_HI_LOG_PRINTF == 1) || (FEATURE_USER_MC_LOG_PRINTF == 1))
    OutputToHiLog(logLevel, str);
#endif
#ifdef TDD_ASSERTIONS
    // output to extra handler if it was set by test cases
    if (g_logOutputExtraHandler != nullptr) {
        g_logOutputExtraHandler(logLevel, str, length);
    }
#endif // TDD_ASSERTIONS
}

void FlushOutput()
{
    Debugger::GetInstance().FlushOutput();
}
} // namespace ACELite
} // namespace OHOS

#endif // ENABLED(CONSOLE_LOG_OUTPUT)
