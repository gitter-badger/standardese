// Copyright (C) 2016 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <standardese/cpp_namespace.hpp>

#include <cassert>

#include <standardese/detail/parse_utils.hpp>
#include <standardese/detail/search_token.hpp>
#include <standardese/cpp_cursor.hpp>
#include <standardese/parser.hpp>

using namespace standardese;

namespace
{
    bool is_inline_namespace(CXCursor cur, const cpp_name &name)
    {
        return detail::has_prefix_token(cur, "inline", name.c_str());
    }
}

cpp_namespace::parser::parser(const cpp_name &scope, cpp_cursor cur)
: ns_(new cpp_namespace(scope, detail::parse_name(cur), detail::parse_comment(cur)))
{
    assert(clang_getCursorKind(cur) == CXCursor_Namespace);
    if (is_inline_namespace(cur, ns_->get_name()))
        ns_->inline_ = true;

}

cpp_entity_ptr cpp_namespace::parser::finish(const standardese::parser &par)
{
    par.register_namespace(*ns_);
    return std::move(ns_);
}

namespace
{
    void parse_target(CXCursor cur, cpp_name &target, cpp_name &scope)
    {
        auto first = true;
        detail::visit_children(cur, [&](CXCursor cur, CXCursor parent)
        {
            assert(clang_isReference(clang_getCursorKind(cur)));

            auto ref_cursor = clang_getCursorReferenced(cur);
            if (first)
                scope = detail::parse_scope(ref_cursor);
            first = false;

            string ref(clang_getCursorSpelling(ref_cursor));
            if (!target.empty())
                target += "::";
            target += ref;

            if (clang_getCursorKind(ref_cursor) == CXCursor_OverloadedDeclRef)
                // abort when OverloadedDeclRef is found
                // otherwise for inheriting ctors an extra TypeRef follows
                return CXChildVisit_Break;
            return CXChildVisit_Recurse;
        });
    }
}

cpp_ptr<cpp_namespace_alias> cpp_namespace_alias::parse(cpp_name scope, cpp_cursor cur)
{
    assert(clang_getCursorKind(cur) == CXCursor_NamespaceAlias);
    cpp_name target, target_scope;
    parse_target(cur, target, target_scope);
    auto result = detail::make_ptr<cpp_namespace_alias>(std::move(scope), detail::parse_name(cur),
                                                 detail::parse_comment(cur), target);
    result->unique_ = target_scope.empty() ? std::move(target) : target_scope + "::" + target;
    return result;
}

cpp_ptr<cpp_using_directive> cpp_using_directive::parse(cpp_cursor cur)
{
    assert(clang_getCursorKind(cur) == CXCursor_UsingDirective);
    cpp_name target, target_scope;
    parse_target(cur, target, target_scope);
    return detail::make_ptr<cpp_using_directive>(std::move(target_scope), std::move(target), detail::parse_comment(cur));
}

cpp_ptr<cpp_using_declaration> cpp_using_declaration::parse(cpp_cursor cur)
{
    assert(clang_getCursorKind(cur) == CXCursor_UsingDeclaration);

    cpp_name target, target_scope;
    parse_target(cur, target, target_scope);

    return detail::make_ptr<cpp_using_declaration>(std::move(target_scope), std::move(target), detail::parse_comment(cur));
}
