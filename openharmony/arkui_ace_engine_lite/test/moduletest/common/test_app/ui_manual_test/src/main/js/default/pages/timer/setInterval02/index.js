/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
import {backPage,routePage} from "../../../common/js/general";

var index = 0;
export default {
    data:{
        value: "show",
        index:0},
    startTimer:function() {
        var id = setInterval(()=> {
            if(index >5) {
                this.value = "clearInterval";
                clearInterval(id);
            }
            index ++;
            this.index = index;
        }, 200);
    },
    goPrevious:routePage("/pages/timer/setTimeout01/index").changePage,
    goHome:routePage("/").changePage,
    goNext:routePage("/pages/timer/setTimeout03/index").changePage
}