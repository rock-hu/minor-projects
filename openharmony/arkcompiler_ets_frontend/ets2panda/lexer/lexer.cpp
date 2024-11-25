/**
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "lexer.h"

#include "generated/keywords.h"

namespace ark::es2panda::lexer {
LexerPosition::LexerPosition(const util::StringView &source) : iterator_(source) {}

Lexer::Lexer(const parser::ParserContext *parserContext, bool startLexer)
    : allocator_(parserContext->GetProgram()->Allocator()),
      parserContext_(parserContext),
      source_(parserContext->GetProgram()->SourceCode()),
      pos_(source_)
{
    if (startLexer) {
        SkipWhiteSpaces();
    }
}

char32_t Lexer::ScanUnicodeEscapeSequence()
{
    ASSERT(Iterator().Peek() == LEX_CHAR_LOWERCASE_U);
    auto constexpr UNICODE_ESCAPE_SEQUENCE_LENGTH = 4;

    Iterator().Forward(1);

    if (Iterator().Peek() == LEX_CHAR_LEFT_BRACE) {
        Iterator().Forward(1);
        return ScanUnicodeCodePointEscape();
    }

    if (parserContext_->GetProgram()->Extension() == ScriptExtension::AS) {
        return ScanHexEscape<UNICODE_ESCAPE_SEQUENCE_LENGTH, true>();
    }

    return ScanHexEscape<UNICODE_ESCAPE_SEQUENCE_LENGTH>();
}

//  '/u{...}' escape sequence should have at least one hex digit inside brackets!
char32_t Lexer::ScanUnicodeCodePointEscape()
{
    double code = 0;
    char32_t cp = Iterator().Peek();
    if (!IsHexDigit(cp)) {
        ThrowError("Invalid unicode escape sequence");
    }

    while (true) {
        Iterator().Forward(1);

        constexpr auto MULTIPLIER = 16;
        code = code * MULTIPLIER + HexValue(cp);
        if (code > UNICODE_CODE_POINT_MAX) {
            ThrowError("Invalid unicode escape sequence");
        }

        cp = Iterator().Peek();
        if (!IsHexDigit(cp)) {
            break;
        }
    }

    if (cp != LEX_CHAR_RIGHT_BRACE) {
        ThrowError("Invalid unicode escape sequence");
    }

    Iterator().Forward(1);
    return static_cast<char32_t>(code);
}

ArenaAllocator *Lexer::Allocator()
{
    return allocator_;
}

Token &Lexer::GetToken()
{
    return pos_.token_;
}

const Token &Lexer::GetToken() const
{
    return pos_.token_;
}

size_t Lexer::Line() const
{
    return pos_.line_;
}

LexerPosition Lexer::Save() const
{
    return pos_;
}

void Lexer::BackwardToken(TokenType type, size_t offset)
{
    pos_.token_.type_ = type;
    pos_.iterator_.Reset(GetToken().End().index - offset);
    pos_.nextTokenLine_ = 0;
}

void Lexer::ForwardToken(TokenType type, size_t offset)
{
    pos_.token_.type_ = type;
    pos_.iterator_.Forward(offset);
    SkipWhiteSpaces();
}

void Lexer::Rewind(const LexerPosition &pos)
{
    pos_ = pos;
}

char32_t Lexer::Lookahead()
{
    return Iterator().Peek();
}

util::StringView Lexer::SourceView(const util::StringView::Iterator &begin, const util::StringView::Iterator &end) const
{
    return SourceView(begin.Index(), end.Index());
}

util::StringView Lexer::SourceView(size_t begin, size_t end) const
{
    return source_.Substr(begin, end);
}

void Lexer::SkipMultiLineComment()
{
    while (true) {
        switch (Iterator().Next()) {
            case util::StringView::Iterator::INVALID_CP: {
                ThrowError("Unterminated multi-line comment");
            }
            case LEX_CHAR_LF:
            case LEX_CHAR_CR:
            case LEX_CHAR_LS:
            case LEX_CHAR_PS: {
                pos_.nextTokenLine_++;
                continue;
            }
            case LEX_CHAR_ASTERISK: {
                if (Iterator().Peek() == LEX_CHAR_SLASH) {
                    Iterator().Forward(1);
                    return;
                }

                break;
            }
            default: {
                break;
            }
        }
    }
}

/* New line character is not processed */
void Lexer::SkipSingleLineComment()
{
    while (true) {
        switch (Iterator().Next()) {
            case util::StringView::Iterator::INVALID_CP:
            case LEX_CHAR_CR: {
                if (Iterator().Peek() == LEX_CHAR_LF) {
                    Iterator().Forward(1);
                }

                [[fallthrough]];
            }
            case LEX_CHAR_LF:
            case LEX_CHAR_LS:
            case LEX_CHAR_PS: {
                pos_.nextTokenLine_++;
                return;
            }
            default: {
                break;
            }
        }
    }
}

[[noreturn]] void Lexer::ThrowUnexpectedToken(lexer::TokenType tokenType) const
{
    std::stringstream ss;
    ss << "Unexpected token: '" << TokenToString(tokenType) << "'.";
    ThrowError(ss.str());
}

[[noreturn]] void Lexer::ThrowError(std::string_view message) const
{
    lexer::LineIndex index(source_);
    lexer::SourceLocation loc = index.GetLocation(SourcePosition(Iterator().Index(), pos_.line_));

    throw Error(es2panda::ErrorType::SYNTAX, parserContext_->GetProgram()->SourceFilePath().Utf8(), message, loc.line,
                loc.col);
}

void Lexer::CheckNumberLiteralEnd()
{
    if (Iterator().Peek() == LEX_CHAR_LOWERCASE_N) {
        GetToken().flags_ |= TokenFlags::NUMBER_BIGINT;
        GetToken().src_ = SourceView(GetToken().Start().index, Iterator().Index());
        Iterator().Forward(1);
    } else {
        GetToken().src_ = SourceView(GetToken().Start().index, Iterator().Index());
    }

    const auto nextCp = Iterator().PeekCp();
    if (KeywordsUtil::IsIdentifierStart(nextCp) || IsDecimalDigit(nextCp)) {
        ThrowError("Invalid numeric literal");
    }
}

void Lexer::ScanDecimalNumbers()
{
    bool allowNumericOnNext = true;

    while (true) {
        switch (Iterator().Peek()) {
            case LEX_CHAR_0:
            case LEX_CHAR_1:
            case LEX_CHAR_2:
            case LEX_CHAR_3:
            case LEX_CHAR_4:
            case LEX_CHAR_5:
            case LEX_CHAR_6:
            case LEX_CHAR_7:
            case LEX_CHAR_8:
            case LEX_CHAR_9: {
                Iterator().Forward(1);
                allowNumericOnNext = true;
                break;
            }
            case LEX_CHAR_UNDERSCORE: {
                Iterator().Backward(1);

                if (Iterator().Peek() == LEX_CHAR_DOT || !allowNumericOnNext) {
                    Iterator().Forward(1);
                    ThrowError("Invalid numeric separator");
                }

                GetToken().flags_ |= TokenFlags::NUMBER_HAS_UNDERSCORE;
                Iterator().Forward(2U);
                allowNumericOnNext = false;
                break;
            }
            default: {
                if (!allowNumericOnNext) {
                    ThrowError("Numeric separators are not allowed at the end of numeric literals");
                }
                return;
            }
        }
    }
}

void Lexer::ConvertNumber([[maybe_unused]] NumberFlags flags)
{
    ConversionResult res;
    const long double temp = StrToNumeric(&std::strtold, GetToken().src_.Utf8().data(), res);
    if (res == ConversionResult::SUCCESS) {
        GetToken().number_ = Number(GetToken().src_, static_cast<double>(temp));
    } else if (res == ConversionResult::INVALID_ARGUMENT) {
        ThrowError("Invalid number");
    } else if (res == ConversionResult::OUT_OF_RANGE) {
        GetToken().number_ = Number(GetToken().src_, std::numeric_limits<double>::infinity());
    }
}

void Lexer::ScanNumber(bool const leadingMinus, bool allowBigInt)
{
    const bool isPeriod = GetToken().type_ == TokenType::PUNCTUATOR_PERIOD;
    GetToken().type_ = TokenType::LITERAL_NUMBER;
    GetToken().keywordType_ = TokenType::LITERAL_NUMBER;

    if (!isPeriod) {
        ScanDecimalNumbers();
    }

    bool parseExponent = true;
    auto flags = NumberFlags::NONE;

    if (Iterator().Peek() == LEX_CHAR_DOT || isPeriod) {
        flags |= NumberFlags::DECIMAL_POINT;
        allowBigInt = false;
        if (!isPeriod) {
            Iterator().Forward(1);
        }

        auto cp = Iterator().Peek();
        if (IsDecimalDigit(cp) || cp == LEX_CHAR_LOWERCASE_E || cp == LEX_CHAR_UPPERCASE_E) {
            ScanDecimalNumbers();
        } else {
            parseExponent = false;
        }
    }

    auto const signPosition = ScanCharLex(parseExponent, allowBigInt, flags);

    CheckNumberLiteralEnd();

    if ((GetToken().flags_ & TokenFlags::NUMBER_BIGINT) != 0) {
        if (!allowBigInt) {
            ThrowError("Invalid BigInt number");
        }

        return;
    }

    util::StringView sv = SourceView(GetToken().Start().index, Iterator().Index());
    std::string utf8 = !leadingMinus ? std::string {sv.Utf8()} : '-' + std::string {sv.Utf8()};
    bool needConversion = leadingMinus;

    if (signPosition) {
        utf8.insert(*signPosition + (!leadingMinus ? 0U : 1U), 1U, '+');
        needConversion = true;
    }

    if ((GetToken().flags_ & TokenFlags::NUMBER_HAS_UNDERSCORE) != 0U) {
        utf8.erase(std::remove(utf8.begin(), utf8.end(), LEX_CHAR_UNDERSCORE), utf8.end());
        needConversion = true;
    }

    GetToken().src_ = needConversion ? util::UString(utf8, Allocator()).View() : sv;

    ConvertNumber(flags);
}

std::optional<std::size_t> Lexer::ScanCharLex(bool const parseExponent, bool &allowBigInt, NumberFlags &flags)
{
    std::optional<std::size_t> rc {};

    if (auto const ch = Iterator().Peek(); ch == LEX_CHAR_LOWERCASE_E || ch == LEX_CHAR_UPPERCASE_E) {
        allowBigInt = false;

        if (parseExponent) {
            flags |= NumberFlags::EXPONENT;

            Iterator().Forward(1);

            rc = ScanSignOfNumber();

            if (!IsDecimalDigit(Iterator().Peek())) {
                ThrowError("Invalid numeric literal");
            }
            ScanDecimalNumbers();
        }
    }

    return rc;
}

std::optional<std::size_t> Lexer::ScanSignOfNumber() noexcept
{
    switch (Iterator().Peek()) {
        case LEX_CHAR_UNDERSCORE: {
            break;
        }
        case LEX_CHAR_PLUS:
        case LEX_CHAR_MINUS: {
            Iterator().Forward(1);
            break;
        }
        default: {
            return std::make_optional(Iterator().Index() - GetToken().Start().index);
        }
    }
    return std::nullopt;
}

void Lexer::PushTemplateContext(TemplateLiteralParserContext *ctx)
{
    tlCtx_ = ctx;
}

void Lexer::ScanTemplateStringEnd()
{
    ASSERT(Iterator().Peek() == LEX_CHAR_BACK_TICK);
    Iterator().Forward(1);
    SetTokenEnd();
    SkipWhiteSpaces();
}

void Lexer::CheckOctalDigit(char32_t const nextCp)
{
    if (IsOctalDigit(nextCp)) {
        Iterator().Forward(1);

        if (Iterator().Peek() != LEX_CHAR_BACK_TICK) {
            ThrowError("Octal escape sequences are not allowed in template strings");
        }

        Iterator().Backward(1);
    }
}

std::tuple<bool, bool, LexerTemplateString> Lexer::ScanTemplateStringCpHelper(char32_t cp,
                                                                              LexerTemplateString templateStr)
{
    switch (cp) {
        case util::StringView::Iterator::INVALID_CP:
            ThrowError("Unexpected token, expected '${' or '`'");
            return {true, false, templateStr};
        case LEX_CHAR_BACK_TICK:
            templateStr.end = Iterator().Index();
            return {true, false, templateStr};
        case LEX_CHAR_CR: {
            Iterator().Forward(1);

            if (Iterator().Peek() != LEX_CHAR_LF) {
                Iterator().Backward(1);
            }

            [[fallthrough]];
        }
        case LEX_CHAR_LF:
            pos_.line_++;
            templateStr.str.Append(LEX_CHAR_LF);
            Iterator().Forward(1);
            return {false, true, templateStr};
        case LEX_CHAR_BACKSLASH: {
            Iterator().Forward(1);

            char32_t nextCp = Iterator().Peek();
            CheckOctalDigit(nextCp);

            if (nextCp == LEX_CHAR_BACK_TICK || nextCp == LEX_CHAR_BACKSLASH || nextCp == LEX_CHAR_DOLLAR_SIGN) {
                templateStr.str.Append(cp);
                templateStr.str.Append(nextCp);
                Iterator().Forward(1);
                return {false, true, templateStr};
            }

            Iterator().Backward(1);
            return {false, false, templateStr};
        }
        case LEX_CHAR_DOLLAR_SIGN:
            templateStr.end = Iterator().Index();
            Iterator().Forward(1);

            if (Iterator().Peek() == LEX_CHAR_LEFT_BRACE) {
                Iterator().Forward(1);
                templateStr.scanExpression = true;
                SkipWhiteSpaces();
                return {true, false, templateStr};
            }

            templateStr.str.Append(cp);
            return {false, true, templateStr};
        default:
            return {false, false, templateStr};
    }
    return {false, false, templateStr};
}

LexerTemplateString Lexer::ScanTemplateString()
{
    LexerTemplateString templateStr(Allocator());
    size_t cpSize = 0U;

    while (true) {
        char32_t cp = Iterator().PeekCp(&cpSize);

        bool isReturn = false;
        bool isContinue = false;
        std::tie(isReturn, isContinue, templateStr) = ScanTemplateStringCpHelper(cp, templateStr);
        if (isReturn) {
            return templateStr;
        }
        if (isContinue) {
            continue;
        }

        templateStr.str.Append(cp);
        Iterator().Forward(cpSize);
    }

    UNREACHABLE();
    return templateStr;
}

void Lexer::ResetTokenEnd()
{
    SetTokenStart();
    pos_.iterator_.Reset(GetToken().End().index);
    pos_.line_ = GetToken().End().line;
    pos_.nextTokenLine_ = 0;
}

void Lexer::ScanStringUnicodePart(util::UString *str)
{
    if (auto cp = ScanUnicodeCharacter(); cp != util::StringView::Iterator::INVALID_CP) {
        str->Append(cp);
    }
}

char32_t Lexer::ScanUnicodeCharacterHelper(size_t cpSize, char32_t cp)
{
    Iterator().Forward(cpSize);
    return cp;
}

char32_t Lexer::ScanUnicodeCharacter()
{
    size_t cpSize {};
    char32_t cp = Iterator().PeekCp(&cpSize);

    switch (cp) {
        case util::StringView::Iterator::INVALID_CP:
            ThrowError("Unterminated string");
        case LEX_CHAR_CR:
            Iterator().Forward(1);
            if (Iterator().Peek() != LEX_CHAR_LF) {
                Iterator().Backward(1);
            }

            [[fallthrough]];
        case LEX_CHAR_LS:
        case LEX_CHAR_PS:
        case LEX_CHAR_LF:
            pos_.line_++;
            return ScanUnicodeCharacterHelper(cpSize, util::StringView::Iterator::INVALID_CP);
        case LEX_CHAR_LOWERCASE_B:
            return ScanUnicodeCharacterHelper(cpSize, LEX_CHAR_BS);
        case LEX_CHAR_LOWERCASE_T:
            return ScanUnicodeCharacterHelper(cpSize, LEX_CHAR_TAB);
        case LEX_CHAR_LOWERCASE_N:
            return ScanUnicodeCharacterHelper(cpSize, LEX_CHAR_LF);
        case LEX_CHAR_LOWERCASE_V:
            return ScanUnicodeCharacterHelper(cpSize, LEX_CHAR_VT);
        case LEX_CHAR_LOWERCASE_F:
            return ScanUnicodeCharacterHelper(cpSize, LEX_CHAR_FF);
        case LEX_CHAR_LOWERCASE_R:
            return ScanUnicodeCharacterHelper(cpSize, LEX_CHAR_CR);
        case LEX_CHAR_LOWERCASE_X:
            Iterator().Forward(1);
            return ScanHexEscape<2U>();
        case LEX_CHAR_LOWERCASE_U:
            return ScanUnicodeEscapeSequence();
        case LEX_CHAR_0: {
            Iterator().Forward(1);
            bool isDecimal = IsDecimalDigit(Iterator().Peek());
            Iterator().Backward(1);

            if (!isDecimal) {
                return ScanUnicodeCharacterHelper(cpSize, LEX_CHAR_NULL);
            }

            [[fallthrough]];
        }
        default:
            if (IsDecimalDigit(Iterator().Peek())) {
                ThrowError("Invalid character escape sequence in strict mode");
            }
    }

    return ScanUnicodeCharacterHelper(cpSize, cp);
}

void Lexer::ScanQuestionPunctuator()
{
    GetToken().type_ = TokenType::PUNCTUATOR_QUESTION_MARK;

    switch (Iterator().Peek()) {
        case LEX_CHAR_QUESTION: {
            GetToken().type_ = TokenType::PUNCTUATOR_NULLISH_COALESCING;
            Iterator().Forward(1);

            switch (Iterator().Peek()) {
                case LEX_CHAR_EQUALS: {
                    GetToken().type_ = TokenType::PUNCTUATOR_LOGICAL_NULLISH_EQUAL;
                    Iterator().Forward(1);
                    break;
                }
                default: {
                    break;
                }
            }

            break;
        }
        case LEX_CHAR_DOT: {
            Iterator().Forward(1);

            if (!IsDecimalDigit(Iterator().Peek())) {
                GetToken().type_ = TokenType::PUNCTUATOR_QUESTION_DOT;
                return;
            }

            Iterator().Backward(1);
            break;
        }
        default: {
            break;
        }
    }
}

void Lexer::ScanLessThanPunctuator()
{
    GetToken().type_ = TokenType::PUNCTUATOR_LESS_THAN;

    switch (Iterator().Peek()) {
        case LEX_CHAR_LESS_THAN: {
            GetToken().type_ = TokenType::PUNCTUATOR_LEFT_SHIFT;
            Iterator().Forward(1);

            if (Iterator().Peek() == LEX_CHAR_EQUALS) {
                GetToken().type_ = TokenType::PUNCTUATOR_LEFT_SHIFT_EQUAL;
                Iterator().Forward(1);
            }
            break;
        }
        case LEX_CHAR_EQUALS: {
            GetToken().type_ = TokenType::PUNCTUATOR_LESS_THAN_EQUAL;
            Iterator().Forward(1);
            break;
        }
        default: {
            break;
        }
    }
}

void Lexer::ScanGreaterThanPunctuator()
{
    GetToken().type_ = TokenType::PUNCTUATOR_GREATER_THAN;

    switch (Iterator().Peek()) {
        case LEX_CHAR_GREATER_THAN: {
            GetToken().type_ = TokenType::PUNCTUATOR_RIGHT_SHIFT;
            Iterator().Forward(1);

            switch (Iterator().Peek()) {
                case LEX_CHAR_GREATER_THAN: {
                    GetToken().type_ = TokenType::PUNCTUATOR_UNSIGNED_RIGHT_SHIFT;
                    Iterator().Forward(1);

                    if (Iterator().Peek() == LEX_CHAR_EQUALS) {
                        GetToken().type_ = TokenType::PUNCTUATOR_UNSIGNED_RIGHT_SHIFT_EQUAL;
                        Iterator().Forward(1);
                    }
                    break;
                }
                case LEX_CHAR_EQUALS: {
                    GetToken().type_ = TokenType::PUNCTUATOR_RIGHT_SHIFT_EQUAL;
                    Iterator().Forward(1);
                    break;
                }
                default: {
                    break;
                }
            }
            break;
        }
        case LEX_CHAR_EQUALS: {
            GetToken().type_ = TokenType::PUNCTUATOR_GREATER_THAN_EQUAL;
            Iterator().Forward(1);
            break;
        }
        default: {
            break;
        }
    }
}

void Lexer::ScanEqualsPunctuator()
{
    GetToken().type_ = TokenType::PUNCTUATOR_SUBSTITUTION;

    switch (Iterator().Peek()) {
        case LEX_CHAR_EQUALS: {
            GetToken().type_ = TokenType::PUNCTUATOR_EQUAL;
            Iterator().Forward(1);

            if (Iterator().Peek() == LEX_CHAR_EQUALS) {
                GetToken().type_ = TokenType::PUNCTUATOR_STRICT_EQUAL;
                Iterator().Forward(1);
            }
            break;
        }
        case LEX_CHAR_GREATER_THAN: {
            GetToken().type_ = TokenType::PUNCTUATOR_ARROW;
            Iterator().Forward(1);
            break;
        }
        default: {
            break;
        }
    }
}

void Lexer::ScanExclamationPunctuator()
{
    GetToken().type_ = TokenType::PUNCTUATOR_EXCLAMATION_MARK;

    switch (Iterator().Peek()) {
        case LEX_CHAR_EQUALS: {
            GetToken().type_ = TokenType::PUNCTUATOR_NOT_EQUAL;
            Iterator().Forward(1);

            if (Iterator().Peek() == LEX_CHAR_EQUALS) {
                GetToken().type_ = TokenType::PUNCTUATOR_NOT_STRICT_EQUAL;
                Iterator().Forward(1);
            }
            break;
        }
        default: {
            break;
        }
    }
}

void Lexer::ScanAmpersandPunctuator()
{
    GetToken().type_ = TokenType::PUNCTUATOR_BITWISE_AND;

    switch (Iterator().Peek()) {
        case LEX_CHAR_AMPERSAND: {
            GetToken().type_ = TokenType::PUNCTUATOR_LOGICAL_AND;
            Iterator().Forward(1);

            switch (Iterator().Peek()) {
                case LEX_CHAR_EQUALS: {
                    GetToken().type_ = TokenType::PUNCTUATOR_LOGICAL_AND_EQUAL;
                    Iterator().Forward(1);
                    break;
                }
                default: {
                    break;
                }
            }

            break;
        }
        case LEX_CHAR_EQUALS: {
            GetToken().type_ = TokenType::PUNCTUATOR_BITWISE_AND_EQUAL;
            Iterator().Forward(1);
            break;
        }
        default: {
            break;
        }
    }
}

void Lexer::ScanAtPunctuator()
{
    GetToken().type_ = TokenType::PUNCTUATOR_AT;

    if (Iterator().Peek() == LEX_CHAR_AT) {
        GetToken().type_ = TokenType::PUNCTUATOR_FORMAT;
        Iterator().Forward(1U);
    }
}

void Lexer::ScanVLinePunctuator()
{
    GetToken().type_ = TokenType::PUNCTUATOR_BITWISE_OR;

    switch (Iterator().Peek()) {
        case LEX_CHAR_VLINE: {
            GetToken().type_ = TokenType::PUNCTUATOR_LOGICAL_OR;
            Iterator().Forward(1);

            switch (Iterator().Peek()) {
                case LEX_CHAR_EQUALS: {
                    GetToken().type_ = TokenType::PUNCTUATOR_LOGICAL_OR_EQUAL;
                    Iterator().Forward(1);
                    break;
                }
                default: {
                    break;
                }
            }

            break;
        }
        case LEX_CHAR_EQUALS: {
            GetToken().type_ = TokenType::PUNCTUATOR_BITWISE_OR_EQUAL;
            Iterator().Forward(1);
            break;
        }
        default: {
            break;
        }
    }
}

void Lexer::ScanCircumflexPunctuator()
{
    GetToken().type_ = TokenType::PUNCTUATOR_BITWISE_XOR;

    switch (Iterator().Peek()) {
        case LEX_CHAR_EQUALS: {
            GetToken().type_ = TokenType::PUNCTUATOR_BITWISE_XOR_EQUAL;
            Iterator().Forward(1);
            break;
        }
        default: {
            break;
        }
    }
}

void Lexer::ScanPlusPunctuator()
{
    GetToken().type_ = TokenType::PUNCTUATOR_PLUS;

    switch (Iterator().Peek()) {
        case LEX_CHAR_PLUS: {
            GetToken().type_ = TokenType::PUNCTUATOR_PLUS_PLUS;
            Iterator().Forward(1);
            break;
        }
        case LEX_CHAR_EQUALS: {
            GetToken().type_ = TokenType::PUNCTUATOR_PLUS_EQUAL;
            Iterator().Forward(1);
            break;
        }
        default: {
            break;
        }
    }
}

void Lexer::ScanMinusPunctuator()
{
    GetToken().type_ = TokenType::PUNCTUATOR_MINUS;

    switch (Iterator().Peek()) {
        case LEX_CHAR_MINUS: {
            GetToken().type_ = TokenType::PUNCTUATOR_MINUS_MINUS;
            Iterator().Forward(1);
            break;
        }
        case LEX_CHAR_EQUALS: {
            GetToken().type_ = TokenType::PUNCTUATOR_MINUS_EQUAL;
            Iterator().Forward(1);
            break;
        }
        default: {
            break;
        }
    }
}

void Lexer::ScanSlashPunctuator()
{
    GetToken().type_ = TokenType::PUNCTUATOR_DIVIDE;

    switch (Iterator().Peek()) {
        case LEX_CHAR_EQUALS: {
            GetToken().type_ = TokenType::PUNCTUATOR_DIVIDE_EQUAL;
            Iterator().Forward(1);
            break;
        }
        default: {
            break;
        }
    }
}

void Lexer::ScanDotPunctuator()
{
    GetToken().type_ = TokenType::PUNCTUATOR_PERIOD;

    switch (Iterator().Peek()) {
        case LEX_CHAR_0:
        case LEX_CHAR_1:
        case LEX_CHAR_2:
        case LEX_CHAR_3:
        case LEX_CHAR_4:
        case LEX_CHAR_5:
        case LEX_CHAR_6:
        case LEX_CHAR_7:
        case LEX_CHAR_8:
        case LEX_CHAR_9: {
            ScanNumber();
            break;
        }
        case LEX_CHAR_QUESTION: {
            GetToken().type_ = TokenType::PUNCTUATOR_PERIOD_QUESTION;
            Iterator().Forward(1);
            break;
        }
        case LEX_CHAR_DOT: {
            Iterator().Forward(1);

            if (Iterator().Peek() == LEX_CHAR_DOT) {
                GetToken().type_ = TokenType::PUNCTUATOR_PERIOD_PERIOD_PERIOD;
                Iterator().Forward(1);
                break;
            }

            Iterator().Backward(1);
            break;
        }
        default: {
            break;
        }
    }
}

void Lexer::ScanAsteriskPunctuator()
{
    GetToken().type_ = TokenType::PUNCTUATOR_MULTIPLY;

    switch (Iterator().Peek()) {
        case LEX_CHAR_ASTERISK: {
            GetToken().type_ = TokenType::PUNCTUATOR_EXPONENTIATION;
            Iterator().Forward(1);

            if (Iterator().Peek() == LEX_CHAR_EQUALS) {
                GetToken().type_ = TokenType::PUNCTUATOR_EXPONENTIATION_EQUAL;
                Iterator().Forward(1);
            }
            break;
        }
        case LEX_CHAR_EQUALS: {
            GetToken().type_ = TokenType::PUNCTUATOR_MULTIPLY_EQUAL;
            Iterator().Forward(1);
            break;
        }
        default: {
            break;
        }
    }
}

void Lexer::ScanPercentPunctuator()
{
    GetToken().type_ = TokenType::PUNCTUATOR_MOD;

    switch (Iterator().Peek()) {
        case LEX_CHAR_EQUALS: {
            GetToken().type_ = TokenType::PUNCTUATOR_MOD_EQUAL;
            Iterator().Forward(1);
            break;
        }
        default: {
            break;
        }
    }
}

bool Lexer::IsLineTerminatorOrEos() const
{
    switch (Iterator().PeekCp()) {
        case util::StringView::Iterator::INVALID_CP:
        case LEX_CHAR_LF:
        case LEX_CHAR_CR:
        case LEX_CHAR_LS:
        case LEX_CHAR_PS: {
            return true;
        }
        default: {
            break;
        }
    }

    return false;
}

void Lexer::ScanRegExpPattern()
{
    bool isCharClass = false;
    size_t cpSize {};

    while (true) {
        switch (Iterator().PeekCp(&cpSize)) {
            case util::StringView::Iterator::INVALID_CP:
            case LEX_CHAR_LF:
            case LEX_CHAR_CR:
            case LEX_CHAR_LS:
            case LEX_CHAR_PS: {
                ThrowError("Unterminated RegExp");
            }
            case LEX_CHAR_SLASH: {
                if (!isCharClass) {
                    return;
                }

                break;
            }
            case LEX_CHAR_LEFT_SQUARE: {
                isCharClass = true;
                break;
            }
            case LEX_CHAR_RIGHT_SQUARE: {
                isCharClass = false;
                break;
            }
            case LEX_CHAR_BACKSLASH: {
                Iterator().Forward(1);

                if (IsLineTerminatorOrEos()) {
                    continue;
                }

                break;
            }
            default: {
                break;
            }
        }

        Iterator().Forward(cpSize);
    }
}

RegExpFlags Lexer::ScanRegExpFlags()
{
    RegExpFlags resultFlags = RegExpFlags::EMPTY;

    while (true) {
        size_t cpSize {};
        auto cp = Iterator().PeekCp(&cpSize);
        if (!KeywordsUtil::IsIdentifierPart(cp)) {
            break;
        }

        Iterator().Forward(cpSize);

        RegExpFlags flag = RegExpFlags::EMPTY;

        switch (cp) {
            case LEX_CHAR_LOWERCASE_G: {
                flag = RegExpFlags::GLOBAL;
                break;
            }
            case LEX_CHAR_LOWERCASE_I: {
                flag = RegExpFlags::IGNORE_CASE;
                break;
            }
            case LEX_CHAR_LOWERCASE_M: {
                flag = RegExpFlags::MULTILINE;
                break;
            }
            case LEX_CHAR_LOWERCASE_S: {
                flag = RegExpFlags::DOTALL;
                break;
            }
            case LEX_CHAR_LOWERCASE_U: {
                flag = RegExpFlags::UNICODE;
                break;
            }
            case LEX_CHAR_LOWERCASE_Y: {
                flag = RegExpFlags::STICKY;
                break;
            }
            case LEX_CHAR_SP: {
                return resultFlags;
            }
            default: {
                ThrowError("Invalid RegExp flag");
            }
        }

        if (flag == RegExpFlags::EMPTY || (resultFlags & flag) != 0) {
            ThrowError("Invalid RegExp flag");
        }

        resultFlags = resultFlags | flag;
    }

    return resultFlags;
}

void Lexer::CheckOctal()
{
    switch (Iterator().Peek()) {
        case LEX_CHAR_8:
        case LEX_CHAR_9: {
            ThrowError("Invalid octal digit");
        }
        default: {
            break;
        }
    }
}

RegExp Lexer::ScanRegExp()
{
    // for proper handling such regexps as /=/
    if (GetToken().Type() == lexer::TokenType::PUNCTUATOR_DIVIDE_EQUAL) {
        Iterator().Backward(1);
    }

    GetToken().type_ = TokenType::LITERAL_REGEXP;
    GetToken().keywordType_ = TokenType::LITERAL_REGEXP;

    const auto patternStart = Iterator().Index();
    ScanRegExpPattern();
    const auto pattern = SourceView(patternStart, Iterator().Index());

    ASSERT(Iterator().Peek() == LEX_CHAR_SLASH);
    Iterator().Forward(1);

    const auto flagsStart = Iterator().Index();
    RegExpFlags resultFlags = ScanRegExpFlags();
    const auto flags = SourceView(flagsStart, Iterator().Index());

    SkipWhiteSpaces();
    SetTokenEnd();

    return {pattern, flags, resultFlags};
}

bool Lexer::CheckArrow()
{
    if (Iterator().Peek() != LEX_CHAR_EQUALS) {
        return false;
    }
    Iterator().Forward(1);

    bool res = Iterator().Peek() == LEX_CHAR_GREATER_THAN;
    Iterator().Backward(1);

    return res;
}

void Lexer::SetTokenStart()
{
    if (pos_.nextTokenLine_ != 0) {
        pos_.line_ += pos_.nextTokenLine_;
        pos_.nextTokenLine_ = 0;
        GetToken().flags_ = TokenFlags::NEW_LINE;
    } else {
        GetToken().flags_ = TokenFlags::NONE;
    }

    pos_.token_.loc_.start = SourcePosition {Iterator().Index(), pos_.line_};
    GetToken().keywordType_ = TokenType::EOS;
}

void Lexer::SetTokenEnd()
{
    pos_.token_.loc_.end = SourcePosition {Iterator().Index(), pos_.line_};
}

bool Lexer::SkipWhiteSpacesHelperSlash(char32_t *cp)
{
    Iterator().Forward(1);
    *cp = Iterator().Peek();
    if (*cp == LEX_CHAR_SLASH || *cp == LEX_CHAR_ASTERISK) {
        Iterator().Forward(1);
        *cp == LEX_CHAR_SLASH ? SkipSingleLineComment() : SkipMultiLineComment();
        return true;
    }

    Iterator().Backward(1);
    return false;
}

bool Lexer::SkipWhiteSpacesHelperDefault(const char32_t &cp)
{
    if (cp < LEX_ASCII_MAX_BITS) {
        return false;
    }

    size_t cpSize {};

    switch (Iterator().PeekCp(&cpSize)) {
        case LEX_CHAR_LS:
        case LEX_CHAR_PS:
            pos_.nextTokenLine_++;
            [[fallthrough]];
        case LEX_CHAR_NBSP:
        case LEX_CHAR_ZWNBSP:
            Iterator().Forward(cpSize);
            return true;
        default:
            return false;
    }
}

void Lexer::SkipWhiteSpaces()
{
    while (true) {
        auto cp = Iterator().Peek();

        switch (cp) {
            case LEX_CHAR_CR:
                Iterator().Forward(1);

                if (Iterator().Peek() != LEX_CHAR_LF) {
                    Iterator().Backward(1);
                }

                [[fallthrough]];
            case LEX_CHAR_LF:
                Iterator().Forward(1);
                pos_.nextTokenLine_++;
                continue;
            case LEX_CHAR_VT:
            case LEX_CHAR_FF:
            case LEX_CHAR_SP:
            case LEX_CHAR_TAB:
                Iterator().Forward(1);
                continue;
            case LEX_CHAR_SLASH:
                if (!SkipWhiteSpacesHelperSlash(&cp)) {
                    return;
                }
                continue;
            default:
                if (!SkipWhiteSpacesHelperDefault(cp)) {
                    return;
                }
                continue;
        }
    }
}

void Lexer::ScanHashMark()
{
    GetToken().type_ = TokenType::PUNCTUATOR_HASH_MARK;
}

void Lexer::ScanBackTick()
{
    GetToken().type_ = TokenType::PUNCTUATOR_BACK_TICK;
    SetTokenEnd();
}

// NOLINTNEXTLINE(google-default-arguments)
void Lexer::NextToken(NextTokenFlags flags)
{
    JSKeywords kws(this, flags);
    NextToken(&kws);
}

void Lexer::ScanColonPunctuator()
{
    GetToken().type_ = TokenType::PUNCTUATOR_COLON;
}

bool Lexer::ScanDollarPunctuator()
{
    return false;
}

// CC-OFFNXT(huge_method,huge_cyclomatic_complexity,G.FUN.01-CPP) big switch-case, solid logic
// NOLINTNEXTLINE(readability-function-size)
void Lexer::NextToken(Keywords *kws)
{
    KeywordsUtil &kwu = kws->Util();

    SetTokenStart();

    auto cp = Iterator().Peek();
    Iterator().Forward(1);

    switch (cp) {
        case LEX_CHAR_EXCLAMATION: {
            ScanExclamationPunctuator();
            break;
        }
        case LEX_CHAR_SINGLE_QUOTE: {
            ScanString<LEX_CHAR_SINGLE_QUOTE>();
            break;
        }
        case LEX_CHAR_DOUBLE_QUOTE: {
            ScanString<LEX_CHAR_DOUBLE_QUOTE>();
            break;
        }
        case LEX_CHAR_HASH_MARK: {
            ScanHashMark();
            break;
        }
        case LEX_CHAR_PERCENT: {
            ScanPercentPunctuator();
            break;
        }
        case LEX_CHAR_AMPERSAND: {
            ScanAmpersandPunctuator();
            break;
        }
        case LEX_CHAR_LEFT_PAREN: {
            GetToken().type_ = TokenType::PUNCTUATOR_LEFT_PARENTHESIS;
            break;
        }
        case LEX_CHAR_RIGHT_PAREN: {
            GetToken().type_ = TokenType::PUNCTUATOR_RIGHT_PARENTHESIS;
            break;
        }
        case LEX_CHAR_ASTERISK: {
            ScanAsteriskPunctuator();
            break;
        }
        case LEX_CHAR_PLUS: {
            ScanPlusPunctuator();
            break;
        }
        case LEX_CHAR_COMMA: {
            GetToken().type_ = TokenType::PUNCTUATOR_COMMA;
            break;
        }
        case LEX_CHAR_MINUS: {
            ScanMinusPunctuator();
            break;
        }
        case LEX_CHAR_DOT: {
            ScanDotPunctuator();
            break;
        }
        case LEX_CHAR_SLASH: {
            ScanSlashPunctuator();
            break;
        }
        case LEX_CHAR_0: {
            if (Iterator().Peek() != LEX_CHAR_DOT) {
                ScanNumberLeadingZero((kwu.Flags() & NextTokenFlags::UNARY_MINUS) !=
                                      std::underlying_type_t<NextTokenFlags>(0U));
                break;
            }
            [[fallthrough]];
        }
        case LEX_CHAR_1:
        case LEX_CHAR_2:
        case LEX_CHAR_3:
        case LEX_CHAR_4:
        case LEX_CHAR_5:
        case LEX_CHAR_6:
        case LEX_CHAR_7:
        case LEX_CHAR_8:
        case LEX_CHAR_9: {
            ScanNumber((kwu.Flags() & NextTokenFlags::UNARY_MINUS) != std::underlying_type_t<NextTokenFlags>(0U));
            break;
        }
        case LEX_CHAR_COLON: {
            ScanColonPunctuator();
            break;
        }
        case LEX_CHAR_SEMICOLON: {
            GetToken().type_ = TokenType::PUNCTUATOR_SEMI_COLON;
            break;
        }
        case LEX_CHAR_LESS_THAN: {
            ScanLessThanPunctuator();
            break;
        }
        case LEX_CHAR_EQUALS: {
            ScanEqualsPunctuator();
            break;
        }
        case LEX_CHAR_GREATER_THAN: {
            ScanGreaterThanPunctuator();
            break;
        }
        case LEX_CHAR_QUESTION: {
            ScanQuestionPunctuator();
            break;
        }
        case LEX_CHAR_AT: {
            ScanAtPunctuator();
            break;
        }
        case LEX_CHAR_DOLLAR_SIGN: {
            if (ScanDollarPunctuator()) {
                break;
            }

            [[fallthrough]];
        }
        case LEX_CHAR_UPPERCASE_A:
        case LEX_CHAR_UPPERCASE_B:
        case LEX_CHAR_UPPERCASE_C:
        case LEX_CHAR_UPPERCASE_D:
        case LEX_CHAR_UPPERCASE_E:
        case LEX_CHAR_UPPERCASE_F:
        case LEX_CHAR_UPPERCASE_G:
        case LEX_CHAR_UPPERCASE_H:
        case LEX_CHAR_UPPERCASE_I:
        case LEX_CHAR_UPPERCASE_J:
        case LEX_CHAR_UPPERCASE_K:
        case LEX_CHAR_UPPERCASE_L:
        case LEX_CHAR_UPPERCASE_M:
        case LEX_CHAR_UPPERCASE_N:
        case LEX_CHAR_UPPERCASE_O:
        case LEX_CHAR_UPPERCASE_P:
        case LEX_CHAR_UPPERCASE_Q:
        case LEX_CHAR_UPPERCASE_R:
        case LEX_CHAR_UPPERCASE_S:
        case LEX_CHAR_UPPERCASE_T:
        case LEX_CHAR_UPPERCASE_U:
        case LEX_CHAR_UPPERCASE_V:
        case LEX_CHAR_UPPERCASE_W:
        case LEX_CHAR_UPPERCASE_X:
        case LEX_CHAR_UPPERCASE_Y:
        case LEX_CHAR_UPPERCASE_Z:
        case LEX_CHAR_UNDERSCORE: {
            kwu.ScanIdContinue();
            break;
        }
        case LEX_CHAR_LEFT_SQUARE: {
            GetToken().type_ = TokenType::PUNCTUATOR_LEFT_SQUARE_BRACKET;
            break;
        }
        case LEX_CHAR_BACKSLASH: {
            GetToken().flags_ |= TokenFlags::HAS_ESCAPE;

            if (Iterator().Peek() != LEX_CHAR_LOWERCASE_U) {
                ThrowError("Invalid character");
            }

            cp = ScanUnicodeEscapeSequence();

            kwu.ScanIdentifierStart(kws, cp);
            break;
        }
        case LEX_CHAR_RIGHT_SQUARE: {
            GetToken().type_ = TokenType::PUNCTUATOR_RIGHT_SQUARE_BRACKET;
            break;
        }
        case LEX_CHAR_CIRCUMFLEX: {
            ScanCircumflexPunctuator();
            break;
        }
        case LEX_CHAR_BACK_TICK: {
            ScanBackTick();
            return;
        }
        case LEX_CHAR_LOWERCASE_C: {
            if (ScanCharLiteral()) {
                break;
            }
        }
            [[fallthrough]];
        case LEX_CHAR_LOWERCASE_A:
        case LEX_CHAR_LOWERCASE_B:
        case LEX_CHAR_LOWERCASE_D:
        case LEX_CHAR_LOWERCASE_E:
        case LEX_CHAR_LOWERCASE_F:
        case LEX_CHAR_LOWERCASE_G:
        case LEX_CHAR_LOWERCASE_H:
        case LEX_CHAR_LOWERCASE_I:
        case LEX_CHAR_LOWERCASE_J:
        case LEX_CHAR_LOWERCASE_K:
        case LEX_CHAR_LOWERCASE_L:
        case LEX_CHAR_LOWERCASE_M:
        case LEX_CHAR_LOWERCASE_N:
        case LEX_CHAR_LOWERCASE_O:
        case LEX_CHAR_LOWERCASE_P:
        case LEX_CHAR_LOWERCASE_Q:
        case LEX_CHAR_LOWERCASE_R:
        case LEX_CHAR_LOWERCASE_S:
        case LEX_CHAR_LOWERCASE_T:
        case LEX_CHAR_LOWERCASE_U:
        case LEX_CHAR_LOWERCASE_V:
        case LEX_CHAR_LOWERCASE_W:
        case LEX_CHAR_LOWERCASE_X:
        case LEX_CHAR_LOWERCASE_Y:
        case LEX_CHAR_LOWERCASE_Z: {
            kws->ScanKeyword(cp);
            break;
        }
        case LEX_CHAR_LEFT_BRACE: {
            GetToken().type_ = TokenType::PUNCTUATOR_LEFT_BRACE;

            if (tlCtx_ != nullptr) {
                tlCtx_->ConsumeLeftBrace();
            }

            break;
        }
        case LEX_CHAR_VLINE: {
            ScanVLinePunctuator();
            break;
        }
        case LEX_CHAR_RIGHT_BRACE: {
            GetToken().type_ = TokenType::PUNCTUATOR_RIGHT_BRACE;

            if (tlCtx_ != nullptr && tlCtx_->ConsumeRightBrace()) {
                SetTokenEnd();
                return;
            }

            break;
        }
        case LEX_CHAR_TILDE: {
            GetToken().type_ = TokenType::PUNCTUATOR_TILDE;
            break;
        }
        default: {
            Iterator().Backward(1);

            if (cp == util::StringView::Iterator::INVALID_CP) {
                GetToken().type_ = TokenType::EOS;
                break;
            }

            cp = Iterator().Next();
            kwu.ScanIdentifierStart(kws, cp);
            break;
        }
    }

    SetTokenEnd();
    SkipWhiteSpaces();
}

void Lexer::ScanNumberLeadingZeroImplNonAllowedCases()
{
    switch (Iterator().Peek()) {
        case LEX_CHAR_0:
        case LEX_CHAR_1:
        case LEX_CHAR_2:
        case LEX_CHAR_3:
        case LEX_CHAR_4:
        case LEX_CHAR_5:
        case LEX_CHAR_6:
        case LEX_CHAR_7: {
            ThrowError("Implicit octal literal not allowed");
        }
        case LEX_CHAR_8:
        case LEX_CHAR_9: {
            ThrowError("NonOctalDecimalIntegerLiteral is not enabled in strict mode code");
        }
        case LEX_CHAR_UNDERSCORE: {
            ThrowError("Numeric separator '_' is not allowed in numbers that start with '0'.");
        }
        default: {
            break;
        }
    }
}

void Lexer::HandleNewlineHelper(util::UString *str, size_t *escapeEnd)
{
    GetToken().flags_ |= TokenFlags::HAS_ESCAPE;
    str->Append(SourceView(*escapeEnd, Iterator().Index()));

    if (Iterator().Peek() == LEX_CHAR_CR) {
        Iterator().Forward(1);
        if (Iterator().Peek() != LEX_CHAR_LF) {
            Iterator().Backward(1);
        }
    }

    pos_.line_++;
    str->Append(LEX_CHAR_LF);
    Iterator().Forward(1);
    *escapeEnd = Iterator().Index();
}

void Lexer::HandleBackslashHelper(util::UString *str, size_t *escapeEnd)
{
    GetToken().flags_ |= TokenFlags::HAS_ESCAPE;
    str->Append(SourceView(*escapeEnd, Iterator().Index()));
    Iterator().Forward(1);
    ScanStringUnicodePart(str);
    *escapeEnd = Iterator().Index();
}

bool Lexer::HandleDollarSignHelper(const char32_t &end)
{
    Iterator().Forward(1);
    if (end == LEX_CHAR_BACK_TICK) {
        if (Iterator().Peek() == LEX_CHAR_LEFT_BRACE) {
            Iterator().Backward(1);
            return true;
        }
    }
    return false;
}

bool Lexer::HandleDoubleQuoteHelper(const char32_t &end, const char32_t &cp)
{
    if (end == cp) {
        return false;
    }
    Iterator().Forward(1);
    return true;
}

void Lexer::FinalizeTokenHelper(util::UString *str, const size_t &startPos, size_t escapeEnd)
{
    if ((GetToken().flags_ & TokenFlags::HAS_ESCAPE) != 0U) {
        str->Append(SourceView(escapeEnd, Iterator().Index()));
        GetToken().src_ = str->View();
    } else {
        GetToken().src_ = SourceView(startPos, Iterator().Index());
    }
}

LexerPosition &Lexer::Pos()
{
    return pos_;
}

const LexerPosition &Lexer::Pos() const
{
    return pos_;
}
}  // namespace ark::es2panda::lexer
