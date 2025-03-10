/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "compare_tools.h"
#include <cstring>
#include "common/screen.h"
#include "dock/screen_device_proxy.h"
#include "draw/draw_utils.h"
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/graphic_math.h"
#include "graphic_config.h"
#include "securec.h"

#ifdef _WIN32
    #define STR(STRING) #STRING
    #define STRPATH(STRING) STR(STRING)
#endif

namespace OHOS {
bool CompareTools::enableLog_ = false;
char* CompareTools::logPath_ = nullptr;

void CompareTools::WaitSuspend(const uint16_t waitTime)
{
    uint16_t sleepTime = waitTime == 0 ? DEFAULT_WAIT_TIME_MS : waitTime;
#ifdef _WIN32
    Sleep(sleepTime);
#else
    usleep(1000 * sleepTime); // 1000: us to ms
#endif // _WIN32
}

bool CompareTools::StrnCatPath(char* filePath, size_t pathMax, const char* fileName, size_t count)
{
    if ((filePath == nullptr) || (pathMax > DEFAULT_FILE_NAME_MAX_LENGTH)) {
        return false;
    }
#ifdef _WIN32
    char dest[DEFAULT_FILE_NAME_MAX_LENGTH] = STRPATH(AUTO_TEST_RESOURCE_PATH);
#else
    char dest[DEFAULT_FILE_NAME_MAX_LENGTH] = AUTO_TEST_RESOURCE_PATH;
#endif // _WIN32
    if (strncat_s(dest, DEFAULT_FILE_NAME_MAX_LENGTH, fileName, count) != EOK) {
        return false;
    }
    if (memcpy_s(static_cast<void *>(filePath), pathMax, dest, DEFAULT_FILE_NAME_MAX_LENGTH) != EOK) {
        return false;
    }
    return true;
}

bool CompareTools::CompareByBit(uint32_t fd)
{
    ImageInfo imageBit;
    if (!(Screen::GetInstance().GetCurrentScreenBitmap(imageBit))) {
        return false;
    }
    struct BitmapInfoHeader bitmapInfo = {0};
    lseek(fd, sizeof(uint16_t), SEEK_SET);
    if (read(fd, &bitmapInfo, sizeof(bitmapInfo)) < 0) {
        ImageCacheFree(imageBit);
        return false;
    }
    if (bitmapInfo.biSizeImage != imageBit.dataSize) {
        ImageCacheFree(imageBit);
        return false;
    }
    bool flag = true;
    uint32_t buffSize = bitmapInfo.biSizeImage / MATH_ABS(bitmapInfo.biHeight);
    auto buff = new uint8_t[buffSize];
    for (uint32_t i = 0; i < MATH_ABS(bitmapInfo.biHeight); i++) {
        if (flag && (memset_s(buff, buffSize, 0, buffSize) != EOK)) {
            flag = false;
            break;
        }
        int32_t ret = read(fd, buff, buffSize);
        if (ret < 0) {
            flag = false;
            break;
        }
        for (uint32_t j = 0; j < ret; j++) {
            if (buff[j] != imageBit.data[i * buffSize + j]) {
                flag = false;
                break;
            }
        }
    }
    ImageCacheFree(imageBit);
    delete [] buff;
    buff = nullptr;
    return flag;
}

bool CompareTools::CompareByBitmap(const BitmapInfoHeader bitmapInfoBase,
    const BitmapInfoHeader bitmapInfoRun, uint32_t fdBase, uint32_t fdRun)
{
    bool flag = true;
    uint32_t buffSizeBase = bitmapInfoBase.biSizeImage / MATH_ABS(bitmapInfoBase.biHeight);
    auto buffBase = new uint8_t[buffSizeBase];

    uint32_t buffSizeRun = bitmapInfoRun.biSizeImage / MATH_ABS(bitmapInfoRun.biHeight);
    auto buffRun = new uint8_t[buffSizeRun];

    for (uint32_t i = 0; i < MATH_ABS(bitmapInfoBase.biHeight); i++) {
        if (flag && (memset_s(buffBase, buffSizeBase, 0, buffSizeBase) != EOK)) {
            flag = false;
            break;
        }
        if (flag && (memset_s(buffRun, buffSizeRun, 0, buffSizeRun) != EOK)) {
            flag = false;
            break;
        }
        int32_t retBase = read(fdBase, buffBase, buffSizeBase);
        if (retBase < 0) {
            flag = false;
            break;
        }
        int32_t retRun = read(fdRun, buffRun, buffSizeBase);
        if (retRun < 0) {
            flag = false;
            break;
        }
        if (retBase != retRun) {
            flag = false;
            break;
        }

        for (uint32_t j = 0; j < retBase; j++) {
            if (buffBase[j] != buffRun[j]) {
                flag = false;
                break;
            }
        }
    }

    delete [] buffBase;
    buffBase = nullptr;
    delete [] buffRun;
    buffRun = nullptr;

    return flag;
}


bool CompareTools::CompareFile(const char* fileBasePath, const char* fileRunPath)
{
    if (fileBasePath == nullptr || fileRunPath == nullptr) {
        return false;
    }
#ifdef _WIN32
    uint32_t fdBase = open(fileBasePath, O_RDONLY | O_BINARY);
    uint32_t fdRun = open(fileRunPath, O_RDONLY | O_BINARY);
#else
    uint32_t fdBase = open(fileBasePath, O_RDONLY);
    uint32_t fdRun = open(fileRunPath, O_RDONLY);
#endif
    struct BitmapInfoHeader bitmapInfoBase = {0};
    lseek(fdBase, sizeof(uint16_t), SEEK_SET);
    if (read(fdBase, &bitmapInfoBase, sizeof(bitmapInfoBase)) < 0) {
        close(fdBase);
        close(fdRun);
        return false;
    }

    struct BitmapInfoHeader bitmapInfoRun = {0};
    lseek(fdRun, sizeof(uint16_t), SEEK_SET);
    if (read(fdRun, &bitmapInfoRun, sizeof(bitmapInfoRun)) < 0) {
        close(fdBase);
        close(fdRun);
        return false;
    }

    if (bitmapInfoBase.biSizeImage != bitmapInfoRun.biSizeImage) {
        close(fdBase);
        close(fdRun);
        return false;
    }

    if (!CompareByBitmap(bitmapInfoBase, bitmapInfoRun, fdBase, fdRun)) {
        close(fdBase);
        close(fdRun);
        return false;
    }

    close(fdBase);
    close(fdRun);
    return true;
}

bool CompareTools::CompareFile(const char* filePath, size_t length)
{
    if ((filePath == nullptr) || (length > DEFAULT_FILE_NAME_MAX_LENGTH)) {
        return false;
    }
#ifdef _WIN32
    uint32_t fd = open(filePath, O_RDONLY | O_BINARY);
#else
    uint32_t fd = open(filePath, O_RDONLY);
#endif
    if (fd == -1) {
        return false;
    }
    bool flag = CompareByBit(fd);
    close(fd);
    if (flag) {
        GRAPHIC_LOGI("[COMPARE_SUCCESS]:fileName=%s", filePath);
        if (enableLog_) {
            char logInfo[DEFAULT_FILE_NAME_MAX_LENGTH] = {0};
            if (sprintf_s(logInfo, sizeof(logInfo), "[COMPARE_SUCCESS]:fileName=%s\n", filePath) < 0) {
                return false;
            }
            SaveLog(logInfo, strlen(logInfo));
        }
    } else {
        GRAPHIC_LOGI("[COMPARE_FAILURE]:fileName=%s", filePath);
        if (enableLog_) {
            char logInfo[DEFAULT_FILE_NAME_MAX_LENGTH] = {0};
            if (sprintf_s(logInfo, sizeof(logInfo), "[COMPARE_FAILURE]:fileName=%s\n", filePath) < 0) {
                return false;
            }
            SaveLog(logInfo, strlen(logInfo));
        }
    }
    return flag;
}

bool CompareTools::SaveByBit(uint32_t fd)
{
    ImageInfo imageBit;
    if (!(Screen::GetInstance().GetCurrentScreenBitmap(imageBit))) {
        return false;
    }
    bool flag = false;
    uint8_t sizeByColorMode = DrawUtils::GetByteSizeByColorMode(ScreenDeviceProxy::GetInstance()->GetBufferMode());
    uint16_t bfType = 0x4D42;
    struct BitmapInfoHeader bitmapInfo = {0};
    bitmapInfo.bfSize = imageBit.dataSize + BITMAP_HEADER_SIZE;
    bitmapInfo.bfOffBits = BITMAP_HEADER_SIZE;
    bitmapInfo.biSize = 40; // 40: bitmap information header size
    bitmapInfo.biWidth = imageBit.header.width;
    bitmapInfo.biHeight = -imageBit.header.height;
    bitmapInfo.biPlanes = 1;
    bitmapInfo.biBitCount = sizeByColorMode * 8; // 8: uint8_t bit
    bitmapInfo.biSizeImage = imageBit.dataSize;
    if (write(fd, &bfType, sizeof(bfType)) > 0) {
        if (write(fd, &bitmapInfo, sizeof(bitmapInfo)) > 0) {
            if (write(fd, imageBit.data, imageBit.dataSize) > 0) {
                flag = true;
            }
        }
    }
    ImageCacheFree(imageBit);
    return flag;
}

void CompareTools::SaveResultLog(const char* filePath, const char* buff, size_t bufSize)
{
    if (filePath == nullptr || buff == nullptr || bufSize <= 0) {
        return;
    }

    SaveLog(buff, bufSize, filePath);
}

bool CompareTools::SaveFile(const char* filePath, size_t length)
{
    if ((filePath == nullptr) || (length > DEFAULT_FILE_NAME_MAX_LENGTH)) {
        return false;
    }
#ifdef _WIN32
    uint32_t fd = open(filePath, O_WRONLY | O_CREAT | O_BINARY, DEFAULT_FILE_PERMISSION);
#else
    uint32_t fd = open(filePath, O_WRONLY | O_CREAT, DEFAULT_FILE_PERMISSION);
#endif
    if (fd == -1) {
        return false;
    }
    bool flag = SaveByBit(fd);
    close(fd);
    return flag;
}

bool CompareTools::CheckFileExist(const char* filePath, size_t length)
{
    if ((filePath == nullptr) || (length > DEFAULT_FILE_NAME_MAX_LENGTH)) {
        return false;
    }
    uint32_t fd = open(filePath, O_RDONLY);
    if (fd == -1) {
        return false;
    }
    close(fd);
    return true;
}

void CompareTools::SetLogPath(const char* filePath, size_t length)
{
    if (logPath_ == nullptr) {
        logPath_ = new char[length];
        if (logPath_ == nullptr) {
            return;
        }
        if (memcpy_s(logPath_, length, filePath, length) != EOK) {
            GRAPHIC_LOGE("memcpy filepath failed");
            return;
        }
        enableLog_ = true;
    }
}

void CompareTools::UnsetLogPath()
{
    if (logPath_ != nullptr) {
        delete[] logPath_;
        logPath_ = nullptr;
        enableLog_ = false;
    }
}

bool CompareTools::SaveLog(const char* buff, size_t bufSize, const char* filePath)
{
    if (buff == nullptr) {
        return false;
    }

    const char* useLogPath = filePath == nullptr ? logPath_ : filePath;
    if (useLogPath == nullptr) {
        return false;
    }

    uint32_t logFd = open(useLogPath, O_WRONLY | O_CREAT | O_APPEND, DEFAULT_FILE_PERMISSION);
    if (logFd == -1) {
        GRAPHIC_LOGE("open log failed");
        return false;
    }
    if (write(logFd, buff, bufSize) < 0) {
        close(logFd);
        GRAPHIC_LOGE("write log failed");
        return false;
    }
    close(logFd);
    return true;
}
} // namespace OHOS
