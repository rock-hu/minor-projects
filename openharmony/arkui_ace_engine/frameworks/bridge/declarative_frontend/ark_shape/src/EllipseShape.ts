/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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


declare class __EllipseShape__<T> {
    constructor(options: ShapeSize);
    width(width: string | number): T;
    height(height: string | number): T;
    size(size: SizeOptions): T;
    offset(offset: Position): T;
    fill(color: number | string): T;
    position(position: Position): T;
}

class EllipseShape extends __EllipseShape__<EllipseShape> {
    constructor(options: ShapeSize) {
        super(options);
    }

    width(width: string | number): EllipseShape {
        return super.width(width);
    }

    height(height: string | number): EllipseShape {
        return super.height(height);
    }

    size(size: SizeOptions): EllipseShape {
        return super.size(size);
    }

    offset(offset: Position): EllipseShape {
        return super.offset(offset);
    }

    fill(width: string | number): EllipseShape {
        return super.fill(width);
    }

    position(position: Position): EllipseShape {
        return super.position(position);
    }
}