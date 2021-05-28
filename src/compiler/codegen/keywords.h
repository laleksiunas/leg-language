#pragma once

#include <set>
#include <string>

namespace codegen::keywords {
    const auto Integer = "int";
    const auto Float = "float";
    const auto String = "string";
    const auto Boolean = "bool";
    const auto Void = "void";

    const auto Let = "let";
    const auto Fn = "fn";
    const auto If = "if";
    const auto Else = "else";
    const auto For = "for";
    const auto While = "while";
    const auto Return = "return";
    const auto Kick = "kick";
    const auto Catch = "catch";
    const auto New = "new";
    const auto In = "in";

    bool isReservedKeyword(const std::string& value) {
        static std::set<std::string> reservedKeywords {
            Integer,
            Float,
            String,
            Void,
            Let,
            Fn,
            If,
            Else,
            For,
            While,
            Return,
            Kick,
            Catch,
            New,
            In
        };

        return reservedKeywords.contains(value);
    }
}
