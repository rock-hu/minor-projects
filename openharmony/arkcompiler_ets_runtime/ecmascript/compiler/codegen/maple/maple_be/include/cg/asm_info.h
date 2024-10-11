/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef MAPLEBE_INCLUDE_CG_ASM_INFO_H
#define MAPLEBE_INCLUDE_CG_ASM_INFO_H

#include "maple_string.h"

namespace maplebe {
enum AsmLabel : uint8 {
    kAsmGlbl,
    kAsmLocal,
    kAsmWeak,
    kAsmBss,
    kAsmComm,
    kAsmData,
    kAsmAlign,
    kAsmSyname,
    kAsmZero,
    kAsmByte,
    kAsmShort,
    kAsmValue,
    kAsmLong,
    kAsmQuad,
    kAsmSize,
    kAsmType,
    kAsmText,
    kAsmHidden
};

class AsmInfo {
public:
    const MapleString &GetCmnt() const
    {
        return asmCmnt;
    }

    const MapleString &GetAtobt() const
    {
        return asmAtObt;
    }

    const MapleString &GetFile() const
    {
        return asmFile;
    }

    const MapleString &GetSection() const
    {
        return asmSection;
    }

    const MapleString &GetRodata() const
    {
        return asmRodata;
    }

    const MapleString &GetGlobal() const
    {
        return asmGlobal;
    }

    const MapleString &GetLocal() const
    {
        return asmLocal;
    }

    const MapleString &GetWeak() const
    {
        return asmWeak;
    }

    const MapleString &GetBss() const
    {
        return asmBss;
    }

    const MapleString &GetComm() const
    {
        return asmComm;
    }

    const MapleString &GetData() const
    {
        return asmData;
    }

    const MapleString &GetAlign() const
    {
        return asmAlign;
    }

    const MapleString &GetZero() const
    {
        return asmZero;
    }

    const MapleString &GetByte() const
    {
        return asmByte;
    }

    const MapleString &GetShort() const
    {
        return asmShort;
    }

    const MapleString &GetValue() const
    {
        return asmValue;
    }

    const MapleString &GetLong() const
    {
        return asmLong;
    }

    const MapleString &GetQuad() const
    {
        return asmQuad;
    }

    const MapleString &GetSize() const
    {
        return asmSize;
    }

    const MapleString &GetType() const
    {
        return asmType;
    }

    const MapleString &GetHidden() const
    {
        return asmHidden;
    }

    const MapleString &GetText() const
    {
        return asmText;
    }

    const MapleString &GetSet() const
    {
        return asmSet;
    }

    const MapleString &GetWeakref() const
    {
        return asmWeakref;
    }

    explicit AsmInfo(MemPool &memPool)
#if TARGX86 || TARGX86_64
        : asmCmnt("\t//\t", &memPool),
#elif TARGARM32
        : asmCmnt("\t@\t", &memPool),
#else
        : asmCmnt("\t#\t", &memPool),
#endif

          asmAtObt("\t%object\t", &memPool),
          asmFile("\t.file\t", &memPool),
          asmSection("\t.section\t", &memPool),
          asmRodata(".rodata\t", &memPool),
          asmGlobal("\t.global\t", &memPool),
          asmLocal("\t.local\t", &memPool),
          asmWeak("\t.weak\t", &memPool),
          asmBss("\t.bss\t", &memPool),
          asmComm("\t.comm\t", &memPool),
          asmData("\t.data\t", &memPool),
          asmAlign("\t.align\t", &memPool),
          asmZero("\t.zero\t", &memPool),
          asmByte("\t.byte\t", &memPool),
          asmShort("\t.short\t", &memPool),
#ifdef TARGARM32
          asmValue("\t.short\t", &memPool),
#else
          asmValue("\t.value\t", &memPool),
#endif
#ifdef TARGARM32
          asmLong("\t.word\t", &memPool),
#else
          asmLong("\t.long\t", &memPool),
#endif
          asmQuad("\t.quad\t", &memPool),
          asmSize("\t.size\t", &memPool),
          asmType("\t.type\t", &memPool),
          asmHidden("\t.hidden\t", &memPool),
          asmText("\t.text\t", &memPool),
          asmSet("\t.set\t", &memPool),
          asmWeakref("\t.weakref\t", &memPool)
    {
    }

    ~AsmInfo() = default;

private:
    MapleString asmCmnt;
    MapleString asmAtObt;
    MapleString asmFile;
    MapleString asmSection;
    MapleString asmRodata;
    MapleString asmGlobal;
    MapleString asmLocal;
    MapleString asmWeak;
    MapleString asmBss;
    MapleString asmComm;
    MapleString asmData;
    MapleString asmAlign;
    MapleString asmZero;
    MapleString asmByte;
    MapleString asmShort;
    MapleString asmValue;
    MapleString asmLong;
    MapleString asmQuad;
    MapleString asmSize;
    MapleString asmType;
    MapleString asmHidden;
    MapleString asmText;
    MapleString asmSet;
    MapleString asmWeakref;
};
} /* namespace maplebe */

#endif /* MAPLEBE_INCLUDE_CG_ASM_INFO_H */
