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

#include "base/utils/string_utils.h"
#include "base/utils/linear_map.h"
#include "core/components_ng/svg/parse/svg_node.h"
#include "core/components_ng/svg/parse/svg_transform.h"

namespace OHOS::Ace::NG {
namespace {

const char TRANSFORM_MATRIX[] = "matrix";
const char TRANSFORM_ROTATE[] = "rotate";
const char TRANSFORM_SCALE[] = "scale";
const char TRANSFORM_SKEWX[] = "skewX";
const char TRANSFORM_SKEWY[] = "skewY";
const char TRANSFORM_TRANSLATE[] = "translate";
constexpr int32_t AMOUNT1 = 1;
constexpr int32_t AMOUNT2 = 2;
constexpr int32_t AMOUNT3 = 3;
constexpr int32_t TRANSFORM_MATRIX_PARA_AMOUNT = 6;
constexpr int32_t NUM0 = 0;
constexpr int32_t NUM1 = 1;
constexpr int32_t NUM2 = 2;
constexpr int32_t NUM3 = 3;
constexpr int32_t NUM4 = 4;
constexpr int32_t NUM5 = 5;
} // namespace

using namespace StringUtils;

void NGSvgTransform::ApplyTransformPivot(const std::string& funcType, const Offset& finalPivot, Matrix4& matrix)
{
    /* move to final pivot to transform.
       effect = translate(finalPivot) -> transform -> translate(-finalPivot) */
    double x = finalPivot.GetX();
    double y = finalPivot.GetY();
    matrix = Matrix4::CreateTranslate(x, y, 0) * matrix * Matrix4::CreateTranslate(-x, -y, 0);
}

bool CreateFromTranslate(const std::vector<std::string>& paramVec, Matrix4& matrix)
{
    double tx = 0.0f;
    double ty = 0.0f;
    if (paramVec.size() == AMOUNT1) {
        tx = StringToDouble(paramVec[NUM0]);
    } else if (paramVec.size() == AMOUNT2) {
        tx = StringToDouble(paramVec[NUM0]);
        ty = StringToDouble(paramVec[NUM1]);
    } else {
        return false;
    }
    matrix = Matrix4::CreateTranslate(tx, ty, 0);
    return true;
}

bool CreateFromScale(const std::vector<std::string>& paramVec, Matrix4& matrix)
{
    double sx = 0.0f;
    double sy = 0.0f;
    if (paramVec.size() == AMOUNT1) {
        sx = StringToDouble(paramVec[NUM0]);
        sy = sx;
    } else if (paramVec.size() == AMOUNT2) {
        sx = StringToDouble(paramVec[NUM0]);
        sy = StringToDouble(paramVec[NUM1]);
    } else {
        return false;
    }
    matrix = Matrix4::CreateScale(sx, sy, 1);
    return true;
}

bool CreateFromRotate(const std::vector<std::string>& paramVec, Matrix4& matrix)
{
    if ((paramVec.size() != AMOUNT1) && (paramVec.size() != AMOUNT3)) {
        return false;
    }
    double rotateAngle = StringToDouble(paramVec[NUM0]);
    matrix = Matrix4::CreateRotate(rotateAngle, 0, 0, 1);
    return true;
}

bool CreateFromSkewx(const std::vector<std::string>& paramVec, Matrix4& matrix)
{
    if (paramVec.size() != AMOUNT1) {
        return false;
    }
    double skewAngleAxisX = StringToDouble(paramVec[NUM0]);
    double skewAngleAxisY = 0.0f;
    matrix = Matrix4::CreateSkew(skewAngleAxisX, skewAngleAxisY);
    return true;
}

bool CreateFromSkewy(const std::vector<std::string>& paramVec, Matrix4& matrix)
{
    if (paramVec.size() != AMOUNT1) {
        return false;
    }
    double skewAngleAxisX = 0.0f;
    double skewAngleAxisY = StringToDouble(paramVec[NUM0]);
    matrix = Matrix4::CreateSkew(skewAngleAxisX, skewAngleAxisY);
    return true;
}

bool CreateFromMatrix(const std::vector<std::string>& paramVec, Matrix4& matrix)
{
    if (paramVec.size() != TRANSFORM_MATRIX_PARA_AMOUNT) {
        return false;
    }
    double a = StringToDouble(paramVec[NUM0]);
    double b = StringToDouble(paramVec[NUM1]);
    double c = StringToDouble(paramVec[NUM2]);
    double d = StringToDouble(paramVec[NUM3]);
    double e = StringToDouble(paramVec[NUM4]);
    double f = StringToDouble(paramVec[NUM5]);
    matrix = Matrix4::CreateMatrix2D(a, b, c, d, e, f);
    return true;
}

using TransformMatrixCreator = bool (*)(const std::vector<std::string>& paramVec, Matrix4& matrix);

bool NGSvgTransform::UpdateSingleTransform(
    const std::string& funcType, const std::vector<std::string>& paramVec, Matrix4& matrix)
{
    static const LinearMapNode<TransformMatrixCreator> matrix4Creator[] = {
        { TRANSFORM_MATRIX, CreateFromMatrix },
        { TRANSFORM_ROTATE, CreateFromRotate },
        { TRANSFORM_SCALE, CreateFromScale },
        { TRANSFORM_SKEWX, CreateFromSkewx },
        { TRANSFORM_SKEWY, CreateFromSkewy },
        { TRANSFORM_TRANSLATE, CreateFromTranslate },
    };
    int64_t index = BinarySearchFindIndex(matrix4Creator, ArraySize(matrix4Creator), funcType.c_str());
    if (index < 0) {
        return false;
    }
    return matrix4Creator[index].value(paramVec, matrix);
}

void UpdateTransformPivot(const std::string& funcType, const std::vector<std::string>& paramVec, Offset& finalPivot)
{
    if ((funcType == TRANSFORM_ROTATE) && (paramVec.size() == AMOUNT3)) {
        Offset localPivot = Offset(StringToDouble(paramVec[NUM1]), StringToDouble(paramVec[NUM2]));
        finalPivot += localPivot;
    }
}

Matrix4 NGSvgTransform::CreateMatrix4(const std::vector<NG::TransformInfo>& transformVec, const Offset& globalPivot)
{
    Matrix4 retMatrix;
    for (auto& attr : transformVec) {
        Matrix4 funcMatrix;
        Offset finalPivot = globalPivot;
        bool isUpdated = UpdateSingleTransform(attr.funcType, attr.paramVec, funcMatrix);
        if (!isUpdated) {
            TAG_LOGW(AceLogTag::ACE_IMAGE, "CreateMatrix4 failed. funcType:[%{public}s], param cnt:%{public}zu",
                attr.funcType.c_str(), attr.paramVec.size());
            return Matrix4();
        }
        UpdateTransformPivot(attr.funcType, attr.paramVec, finalPivot);
        ApplyTransformPivot(attr.funcType, finalPivot, funcMatrix);
        retMatrix = retMatrix * funcMatrix;
    }
    return retMatrix;
}
} // namespace OHOS::Ace::NG