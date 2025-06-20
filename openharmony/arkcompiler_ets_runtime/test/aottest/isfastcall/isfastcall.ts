/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

function assert (expr: unknown, msg?: string): asserts expr {
    if (!expr) throw new Error(msg);
}


// support fastcall
class Car {
};

// support fastcall
class Base {
  constructor() {
  }
};

// donot support fastcall: CALLRUNTIME_SUPERCALLFORWARDALLARGS_PREF_V8
class SuperCallForwordAllArgs extends Base {
}

// donot support fastcall: SUPERCALLTHISRANGE_IMM8_IMM8_V8
class SuperCallThisRange extends Base {
    constructor() {
        super()
    }
}

// donot support fastcall: WIDE_SUPERCALLTHISRANGE_PREF_IMM16_V8
class WideSuperCallThisRange extends Base {
    constructor() {
        super(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,arg15,arg16,arg17,arg18,arg19,arg20,arg21,arg22,arg23,arg24,arg25,arg26,arg27,arg28,arg29,arg30,arg31,arg32,arg33,arg34,arg35,arg36,arg37,arg38,arg39,arg40,arg41,arg42,arg43,arg44,arg45,arg46,arg47,arg48,arg49,arg50,arg51,arg52,arg53,arg54,arg55,arg56,arg57,arg58,arg59,arg60,arg61,arg62,arg63,arg64,arg65,arg66,arg67,arg68,arg69,arg70,arg71,arg72,arg73,arg74,arg75,arg76,arg77,arg78,arg79,arg80,arg81,arg82,arg83,arg84,arg85,arg86,arg87,arg88,arg89,arg90,arg91,arg92,arg93,arg94,arg95,arg96,arg97,arg98,arg99,arg100,arg101,arg102,arg103,arg104,arg105,arg106,arg107,arg108,arg109,arg110,arg111,arg112,arg113,arg114,arg115,arg116,arg117,arg118,arg119,arg120,arg121,arg122,arg123,arg124,arg125,arg126,arg127,arg128,arg129,arg130,arg131,arg132,arg133,arg134,arg135,arg136,arg137,arg138,arg139,arg140,arg141,arg142,arg143,arg144,arg145,arg146,arg147,arg148,arg149,arg150,arg151,arg152,arg153,arg154,arg155,arg156,arg157,arg158,arg159,arg160,arg161,arg162,arg163,arg164,arg165,arg166,arg167,arg168,arg169,arg170,arg171,arg172,arg173,arg174,arg175,arg176,arg177,arg178,arg179,arg180,arg181,arg182,arg183,arg184,arg185,arg186,arg187,arg188,arg189,arg190,arg191,arg192,arg193,arg194,arg195,arg196,arg197,arg198,arg199,arg200,arg201,arg202,arg203,arg204,arg205,arg206,arg207,arg208,arg209,arg210,arg211,arg212,arg213,arg214,arg215,arg216,arg217,arg218,arg219,arg220,arg221,arg222,arg223,arg224,arg225,arg226,arg227,arg228,arg229,arg230,arg231,arg232,arg233,arg234,arg235,arg236,arg237,arg238,arg239,arg240,arg241,arg242,arg243,arg244,arg245,arg246,arg247,arg248,arg249,arg250,arg251,arg252,arg253,arg254,arg255,arg256)
    }
}

// donot support fastcall: SUPERCALLSPREAD_IMM8_V8
class SuperCallSpread extends Base {
    constructor() {
        super(...args)
    }
}

// donot support fastcall: COPYRESTARGS_IMM8
function CopyRestArgs(x, ...theArgs) {
    return x + theArgs.length;
}

// donot support fastcall: WIDE_COPYRESTARGS_PREF_IMM16
function WideCopyRestArgs(x, arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,arg15,arg16,arg17,arg18,arg19,arg20,arg21,arg22,arg23,arg24,arg25,arg26,arg27,arg28,arg29,arg30,arg31,arg32,arg33,arg34,arg35,arg36,arg37,arg38,arg39,arg40,arg41,arg42,arg43,arg44,arg45,arg46,arg47,arg48,arg49,arg50,arg51,arg52,arg53,arg54,arg55,arg56,arg57,arg58,arg59,arg60,arg61,arg62,arg63,arg64,arg65,arg66,arg67,arg68,arg69,arg70,arg71,arg72,arg73,arg74,arg75,arg76,arg77,arg78,arg79,arg80,arg81,arg82,arg83,arg84,arg85,arg86,arg87,arg88,arg89,arg90,arg91,arg92,arg93,arg94,arg95,arg96,arg97,arg98,arg99,arg100,arg101,arg102,arg103,arg104,arg105,arg106,arg107,arg108,arg109,arg110,arg111,arg112,arg113,arg114,arg115,arg116,arg117,arg118,arg119,arg120,arg121,arg122,arg123,arg124,arg125,arg126,arg127,arg128,arg129,arg130,arg131,arg132,arg133,arg134,arg135,arg136,arg137,arg138,arg139,arg140,arg141,arg142,arg143,arg144,arg145,arg146,arg147,arg148,arg149,arg150,arg151,arg152,arg153,arg154,arg155,arg156,arg157,arg158,arg159,arg160,arg161,arg162,arg163,arg164,arg165,arg166,arg167,arg168,arg169,arg170,arg171,arg172,arg173,arg174,arg175,arg176,arg177,arg178,arg179,arg180,arg181,arg182,arg183,arg184,arg185,arg186,arg187,arg188,arg189,arg190,arg191,arg192,arg193,arg194,arg195,arg196,arg197,arg198,arg199,arg200,arg201,arg202,arg203,arg204,arg205,arg206,arg207,arg208,arg209,arg210,arg211,arg212,arg213,arg214,arg215,arg216,arg217,arg218,arg219,arg220,arg221,arg222,arg223,arg224,arg225,arg226,arg227,arg228,arg229,arg230,arg231,arg232,arg233,arg234,arg235,arg236,arg237,arg238,arg239,arg240,arg241,arg242,arg243,arg244,arg245,arg246,arg247,arg248,arg249,arg250,arg251,arg252,arg253,arg254,arg255,arg256,...theArgs) {
    return x + theArgs.length;
}

// donot support fastcall: RESUMEGENERATOR\SUSPENDGENERATOR_V8
export class DsHelper {
    private dsHelper: object
    async getDsHelper(): Promise<object> {
        await 1
        return new Promise((resolve) => {
            resolve(this.dsHelper)
        })
    }
}
var x = new DsHelper()

// donot support fastcall: GETUNMAPPEDARGS
function GetUnmappedArgs() {
    return arguments.length;
}

class SuperCallArrow extends Base {
  af;
  constructor() {
    super();
    // donot support fastcall: SUPERCALLARROWRANGE_IMM8_IMM8_V8
    this.af = _ => super();
  }
}
var supercallarrow = new SuperCallArrow();

class WideSuperCallArrow extends Base {
  af;
  constructor() {
    super();
    // donot support fastcall: WIDE_SUPERCALLARROWRANGE_PREF_IMM16_V8
    this.af = _ => super(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,arg15,arg16,arg17,arg18,arg19,arg20,arg21,arg22,arg23,arg24,arg25,arg26,arg27,arg28,arg29,arg30,arg31,arg32,arg33,arg34,arg35,arg36,arg37,arg38,arg39,arg40,arg41,arg42,arg43,arg44,arg45,arg46,arg47,arg48,arg49,arg50,arg51,arg52,arg53,arg54,arg55,arg56,arg57,arg58,arg59,arg60,arg61,arg62,arg63,arg64,arg65,arg66,arg67,arg68,arg69,arg70,arg71,arg72,arg73,arg74,arg75,arg76,arg77,arg78,arg79,arg80,arg81,arg82,arg83,arg84,arg85,arg86,arg87,arg88,arg89,arg90,arg91,arg92,arg93,arg94,arg95,arg96,arg97,arg98,arg99,arg100,arg101,arg102,arg103,arg104,arg105,arg106,arg107,arg108,arg109,arg110,arg111,arg112,arg113,arg114,arg115,arg116,arg117,arg118,arg119,arg120,arg121,arg122,arg123,arg124,arg125,arg126,arg127,arg128,arg129,arg130,arg131,arg132,arg133,arg134,arg135,arg136,arg137,arg138,arg139,arg140,arg141,arg142,arg143,arg144,arg145,arg146,arg147,arg148,arg149,arg150,arg151,arg152,arg153,arg154,arg155,arg156,arg157,arg158,arg159,arg160,arg161,arg162,arg163,arg164,arg165,arg166,arg167,arg168,arg169,arg170,arg171,arg172,arg173,arg174,arg175,arg176,arg177,arg178,arg179,arg180,arg181,arg182,arg183,arg184,arg185,arg186,arg187,arg188,arg189,arg190,arg191,arg192,arg193,arg194,arg195,arg196,arg197,arg198,arg199,arg200,arg201,arg202,arg203,arg204,arg205,arg206,arg207,arg208,arg209,arg210,arg211,arg212,arg213,arg214,arg215,arg216,arg217,arg218,arg219,arg220,arg221,arg222,arg223,arg224,arg225,arg226,arg227,arg228,arg229,arg230,arg231,arg232,arg233,arg234,arg235,arg236,arg237,arg238,arg239,arg240,arg241,arg242,arg243,arg244,arg245,arg246,arg247,arg248,arg249,arg250,arg251,arg252,arg253,arg254,arg255,arg256);
  }
}
var widesupercallarrow = new WideSuperCallArrow();

assert(ArkTools.isAOTCompiled(Car))
assert(ArkTools.isAOTCompiled(Base))
assert(ArkTools.isAOTCompiled(SuperCallForwordAllArgs))
assert(ArkTools.isAOTCompiled(SuperCallThisRange))
assert(ArkTools.isAOTCompiled(WideSuperCallThisRange))
assert(ArkTools.isAOTCompiled(SuperCallSpread))
assert(ArkTools.isAOTCompiled(CopyRestArgs))
assert(ArkTools.isAOTCompiled(WideCopyRestArgs))
assert(ArkTools.isAOTCompiled(x.getDsHelper))
assert(ArkTools.isAOTCompiled(GetUnmappedArgs))
assert(ArkTools.isAOTCompiled(supercallarrow.af))
assert(ArkTools.isAOTCompiled(widesupercallarrow.af))

assert(ArkTools.isFastCall(Car))
assert(ArkTools.isFastCall(Base))
assert(!ArkTools.isFastCall(SuperCallForwordAllArgs))
assert(!ArkTools.isFastCall(SuperCallThisRange))
assert(!ArkTools.isFastCall(WideSuperCallThisRange))
assert(!ArkTools.isFastCall(SuperCallSpread))
assert(!ArkTools.isFastCall(CopyRestArgs))
assert(!ArkTools.isFastCall(WideCopyRestArgs))
assert(!ArkTools.isFastCall(x.getDsHelper))
assert(!ArkTools.isFastCall(GetUnmappedArgs))
assert(!ArkTools.isFastCall(supercallarrow.af))
assert(!ArkTools.isFastCall(widesupercallarrow.af))
