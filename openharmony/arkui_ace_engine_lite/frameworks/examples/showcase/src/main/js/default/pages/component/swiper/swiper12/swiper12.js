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
import { backPage, routePage } from '../../../../common/js/general';

const DEFAULT_CARD_ITEM = {
    defaultCard: 'Unknown'
};

function buildCards(count = 5) {
    let cardList = [];
    cardList = Array.from({length: count}, (_, i)=>({
        defaultCard: 'current page:' + i
    }));
    return cardList;
}

export default {
    data: {
        result: '',
        currentIndex: 0,
        isLoop: false,
        cards: [DEFAULT_CARD_ITEM]
    },
    onInit() {
        this.result = 'Welcome';
    },
    onChange(e) {
        const {index} = e;
        this.currentIndex = index;
        this.result = `Current Index: ${this.currentIndex}`;
    },
    onDestroy() {

    },
    onResult() {
        this.cards = buildCards();
    },
    ...backPage('pages/component/swiper/indexAttr/index')
};
