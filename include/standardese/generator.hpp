// Copyright (C) 2016 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_GENERATOR_HPP_INCLUDED
#define STANDARDESE_GENERATOR_HPP_INCLUDED

#include <standardese/output.hpp>
#include <standardese/translation_unit.hpp>

namespace standardese
{
    const char* get_entity_type_spelling(cpp_entity::type t);

    void generate_doc_entity(output_base &output, unsigned level, const cpp_entity &e);

    void generate_doc_file(output_base &output, const cpp_file &f);
} // namespace standardese

#endif // STANDARDESE_GENERATOR_HPP_INCLUDED
