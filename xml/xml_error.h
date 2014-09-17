///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef XML_ERROR_H
#define XML_ERROR_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <system_error>
#include <expat.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace xml
{

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class errc
{
    none                            = XML_ERROR_NONE,
    no_memory                       = XML_ERROR_NO_MEMORY,
    syntax                          = XML_ERROR_SYNTAX,
    no_elements                     = XML_ERROR_NO_ELEMENTS,
    invalid_token                   = XML_ERROR_INVALID_TOKEN,
    unclosed_token                  = XML_ERROR_UNCLOSED_TOKEN,
    partial_char                    = XML_ERROR_PARTIAL_CHAR,
    tag_mismatch                    = XML_ERROR_TAG_MISMATCH,
    duplicate_attribute             = XML_ERROR_DUPLICATE_ATTRIBUTE,
    junk_after_doc_element          = XML_ERROR_JUNK_AFTER_DOC_ELEMENT,
    param_entity_ref                = XML_ERROR_PARAM_ENTITY_REF,
    undefined_entity                = XML_ERROR_UNDEFINED_ENTITY,
    recursive_entity_ref            = XML_ERROR_RECURSIVE_ENTITY_REF,
    async_entity                    = XML_ERROR_ASYNC_ENTITY,
    bad_char_ref                    = XML_ERROR_BAD_CHAR_REF,
    binary_entity_ref               = XML_ERROR_BINARY_ENTITY_REF,
    attribute_external_entity_ref   = XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF,
    misplaced_xml_pi                = XML_ERROR_MISPLACED_XML_PI,
    unknown_encoding                = XML_ERROR_UNKNOWN_ENCODING,
    incorrect_encoding              = XML_ERROR_INCORRECT_ENCODING,
    unclosed_cdata_section          = XML_ERROR_UNCLOSED_CDATA_SECTION,
    external_entity_handling        = XML_ERROR_EXTERNAL_ENTITY_HANDLING,
    not_standalone                  = XML_ERROR_NOT_STANDALONE,
    unexpected_state                = XML_ERROR_UNEXPECTED_STATE,
    entity_declared_in_pe           = XML_ERROR_ENTITY_DECLARED_IN_PE,
    feature_requires_xml_dtd        = XML_ERROR_FEATURE_REQUIRES_XML_DTD,
    cant_change_feature_once_parsing= XML_ERROR_CANT_CHANGE_FEATURE_ONCE_PARSING,

    unbound_prefix                  = XML_ERROR_UNBOUND_PREFIX,

    undeclaring_prefix              = XML_ERROR_UNDECLARING_PREFIX,
    incomplete_pe                   = XML_ERROR_INCOMPLETE_PE,
    xml_decl                        = XML_ERROR_XML_DECL,
    text_decl                       = XML_ERROR_TEXT_DECL,
    publicid                        = XML_ERROR_PUBLICID,
    suspended                       = XML_ERROR_SUSPENDED,
    not_suspended                   = XML_ERROR_NOT_SUSPENDED,
    aborted                         = XML_ERROR_ABORTED,
    finished                        = XML_ERROR_FINISHED,
    suspend_pe                      = XML_ERROR_SUSPEND_PE,

    reserved_prefix_xml             = XML_ERROR_RESERVED_PREFIX_XML,
    reserved_prefix_xmlns           = XML_ERROR_RESERVED_PREFIX_XMLNS,
    reserved_namespace_uri          = XML_ERROR_RESERVED_NAMESPACE_URI,

    user_error                     = 0x7fff
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class xml_category: public std::error_category
{
public:
    const char* name() const noexcept override { return "xml"; }
    std::string message(int ev) const override { return ev==int(errc::user_error)? std::string(): XML_ErrorString(XML_Error(ev)); }
};

const std::error_category& xml_category();

///////////////////////////////////////////////////////////////////////////////////////////////////
inline std::error_code make_error_code(xml::errc e)
{ return std::error_code(int(e), xml_category()); }

inline std::error_condition make_error_condition(xml::errc e)
{ return std::error_condition(int(e), xml_category()); }

///////////////////////////////////////////////////////////////////////////////////////////////////
class xml_error: public std::system_error
{
public:
    xml_error(int code): std::system_error(std::error_code(code, xml::xml_category())) { }
    xml_error(int code, const std::string& message): std::system_error(std::error_code(code, xml::xml_category()), message) { }

    xml_error(xml::errc code): std::system_error(std::error_code(int(code), xml::xml_category())) { }
    xml_error(xml::errc code, const std::string& message): std::system_error(std::error_code(static_cast<int>(code), xml::xml_category()), message) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace std
{
    template<>
    struct is_error_code_enum<app::xml::errc>: public true_type { };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // XML_ERROR_H
