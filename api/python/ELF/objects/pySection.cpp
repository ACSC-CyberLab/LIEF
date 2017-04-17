/* Copyright 2017 R. Thomas
 * Copyright 2017 Quarkslab
 *
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
#include "pyELF.hpp"

#include "LIEF/visitors/Hash.hpp"
#include "LIEF/Abstract/Section.hpp"
#include "LIEF/ELF/Section.hpp"

#include <string>
#include <sstream>

template<class T>
using getter_t = T (Section::*)(void) const;

template<class T>
using setter_t = void (Section::*)(T);

template<class T>
using no_const_getter = T (Section::*)(void);

void init_ELF_Section_class(py::module& m) {

  // Section object
  py::class_<Section, LIEF::Section>(m, "Section")
    .def(py::init<>())

    .def("__init__",
        [] (Section& section, std::vector<uint8_t>& content, ELF_CLASS type)
        {
          new (&section) Section(content.data(), type);
        })

    .def_property_readonly("name_idx",
        static_cast<getter_t<uint32_t>>(&Section::name_idx),
        "Index of the section's name in the string table\n\n"
        ".. warning:: The value will probably change when re-building binary.")

    .def_property("type",
        static_cast<getter_t<SECTION_TYPES>>(&Section::type),
        static_cast<setter_t<SECTION_TYPES>>(&Section::type),
        "Return a " RST_CLASS_REF(lief.ELF.SECTION_TYPES) "")

    .def_property("flags",
        static_cast<getter_t<uint64_t>>(&Section::flags),
        static_cast<setter_t<uint64_t>>(&Section::flags),
        "Return sections flags")

    .def_property("virtual_address",
        static_cast<getter_t<uint64_t>>(&Section::virtual_address),
        static_cast<setter_t<uint64_t>>(&Section::virtual_address),
        "Return address where the section will be mapped in memory\n\n"
        ".. warning:: This value is not reliable use segment's virtual address "
        "(:attr:`~lief.ELF.Segment.virtual_address`) instead.")

    .def_property("file_offset",
        static_cast<getter_t<uint64_t>>(&Section::file_offset),
        static_cast<setter_t<uint64_t>>(&Section::file_offset),
        "Data offset in the binary")

    .def_property_readonly("original_size",
        static_cast<getter_t<uint64_t>>(&Section::original_size),
        "original data size. Without modification we have `original_size == size`")

    .def_property("alignment",
        static_cast<getter_t<uint64_t>>(&Section::alignment),
        static_cast<setter_t<uint64_t>>(&Section::alignment)
        )

    .def_property("information",
        static_cast<getter_t<uint64_t>>(&Section::information),
        static_cast<setter_t<uint32_t>>(&Section::information))

    .def_property("entry_size",
        static_cast<getter_t<uint64_t>>(&Section::entry_size),
        static_cast<setter_t<uint64_t>>(&Section::entry_size),
        "If section's content is an array, `entry_size` holds the element's size\n\n"
        ":Example:\n"
        "\tThe `.dynamic` section contains an array of " RST_CLASS_REF(lief.ELF.DynamicEntry) ". As the size "
        "of a dynamic entry is 0x10 (for ELF64), entry_size will contains this value\n\n"
        ".. warning:: This value is not necessarily reliable.")

    .def_property("link",
        static_cast<getter_t<uint32_t>>(&Section::link),
        static_cast<setter_t<uint32_t>>(&Section::link))

    .def_property_readonly("segments",
      static_cast<no_const_getter<it_segments>>(&Section::segments),
      "Return segment(s) associated with the given section",
      py::return_value_policy::reference_internal)

    .def("__contains__",
        [] (const Section &section, SECTION_FLAGS flag)
        {
          return section.has_flag(flag);
        }, "Test if the current section has the given flag")


    .def("__eq__", &Section::operator==)
    .def("__ne__", &Section::operator!=)
    .def("__hash__",
        [] (const Section& section) {
          return LIEF::Hash::hash(section);
        })

    .def("__str__",
        [] (const Section& section)
        {
          std::ostringstream stream;
          stream << section;
          std::string str =  stream.str();
          return str;
        });
}

