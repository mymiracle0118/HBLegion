// Copyright (C) 2005-2010 Code Synthesis Tools CC
//
// This program was generated by CodeSynthesis XSD, an XML Schema to
// C++ data binding compiler.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
// In addition, as a special exception, Code Synthesis Tools CC gives
// permission to link this program with the Xerces-C++ library (or with
// modified versions of Xerces-C++ that use the same license as Xerces-C++),
// and distribute linked combinations including the two. You must obey
// the GNU General Public License version 2 in all respects for all of
// the code used other than Xerces-C++. If you modify this copy of the
// program, you may extend this exception to your version of the program,
// but you are not obligated to do so. If you do not wish to do so, delete
// this exception statement from your version.
//
// Furthermore, Code Synthesis Tools CC makes a special exception for
// the Free/Libre and Open Source Software (FLOSS) which is described
// in the accompanying FLOSSE file.
//

#ifndef DESKTOP_DROPLIST_HXX
#define DESKTOP_DROPLIST_HXX

// Begin prologue.
//
//
// End prologue.

#include <xsd/cxx/config.hxx>

#if (XSD_INT_VERSION != 3030000L)
#error XSD runtime version mismatch
#endif

#include <xsd/cxx/pre.hxx>

#ifndef XSD_USE_CHAR
#define XSD_USE_CHAR
#endif

#ifndef XSD_CXX_TREE_USE_CHAR
#define XSD_CXX_TREE_USE_CHAR
#endif

#include <xsd/cxx/xml/char-utf8.hxx>

#include <xsd/cxx/tree/exceptions.hxx>
#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/types.hxx>

#include <xsd/cxx/xml/error-handler.hxx>

#include <xsd/cxx/xml/dom/auto-ptr.hxx>

#include <xsd/cxx/tree/parsing.hxx>
#include <xsd/cxx/tree/parsing/byte.hxx>
#include <xsd/cxx/tree/parsing/unsigned-byte.hxx>
#include <xsd/cxx/tree/parsing/short.hxx>
#include <xsd/cxx/tree/parsing/unsigned-short.hxx>
#include <xsd/cxx/tree/parsing/int.hxx>
#include <xsd/cxx/tree/parsing/unsigned-int.hxx>
#include <xsd/cxx/tree/parsing/long.hxx>
#include <xsd/cxx/tree/parsing/unsigned-long.hxx>
#include <xsd/cxx/tree/parsing/boolean.hxx>
#include <xsd/cxx/tree/parsing/float.hxx>
#include <xsd/cxx/tree/parsing/double.hxx>
#include <xsd/cxx/tree/parsing/decimal.hxx>

namespace xml_schema
{
  // anyType and anySimpleType.
  //
  typedef ::xsd::cxx::tree::type type;
  typedef ::xsd::cxx::tree::simple_type< type > simple_type;
  typedef ::xsd::cxx::tree::type container;

  // 8-bit
  //
  typedef signed char byte;
  typedef unsigned char unsigned_byte;

  // 16-bit
  //
  typedef short short_;
  typedef unsigned short unsigned_short;

  // 32-bit
  //
  typedef int int_;
  typedef unsigned int unsigned_int;

  // 64-bit
  //
  typedef long long long_;
  typedef unsigned long long unsigned_long;

  // Supposed to be arbitrary-length integral types.
  //
  typedef long long integer;
  typedef long long non_positive_integer;
  typedef unsigned long long non_negative_integer;
  typedef unsigned long long positive_integer;
  typedef long long negative_integer;

  // Boolean.
  //
  typedef bool boolean;

  // Floating-point types.
  //
  typedef float float_;
  typedef double double_;
  typedef double decimal;

  // String types.
  //
  typedef ::xsd::cxx::tree::string< char, simple_type > string;
  typedef ::xsd::cxx::tree::normalized_string< char, string > normalized_string;
  typedef ::xsd::cxx::tree::token< char, normalized_string > token;
  typedef ::xsd::cxx::tree::name< char, token > name;
  typedef ::xsd::cxx::tree::nmtoken< char, token > nmtoken;
  typedef ::xsd::cxx::tree::nmtokens< char, simple_type, nmtoken > nmtokens;
  typedef ::xsd::cxx::tree::ncname< char, name > ncname;
  typedef ::xsd::cxx::tree::language< char, token > language;

  // ID/IDREF.
  //
  typedef ::xsd::cxx::tree::id< char, ncname > id;
  typedef ::xsd::cxx::tree::idref< char, ncname, type > idref;
  typedef ::xsd::cxx::tree::idrefs< char, simple_type, idref > idrefs;

  // URI.
  //
  typedef ::xsd::cxx::tree::uri< char, simple_type > uri;

  // Qualified name.
  //
  typedef ::xsd::cxx::tree::qname< char, simple_type, uri, ncname > qname;

  // Binary.
  //
  typedef ::xsd::cxx::tree::buffer< char > buffer;
  typedef ::xsd::cxx::tree::base64_binary< char, simple_type > base64_binary;
  typedef ::xsd::cxx::tree::hex_binary< char, simple_type > hex_binary;

  // Date/time.
  //
  typedef ::xsd::cxx::tree::time_zone time_zone;
  typedef ::xsd::cxx::tree::date< char, simple_type > date;
  typedef ::xsd::cxx::tree::date_time< char, simple_type > date_time;
  typedef ::xsd::cxx::tree::duration< char, simple_type > duration;
  typedef ::xsd::cxx::tree::gday< char, simple_type > gday;
  typedef ::xsd::cxx::tree::gmonth< char, simple_type > gmonth;
  typedef ::xsd::cxx::tree::gmonth_day< char, simple_type > gmonth_day;
  typedef ::xsd::cxx::tree::gyear< char, simple_type > gyear;
  typedef ::xsd::cxx::tree::gyear_month< char, simple_type > gyear_month;
  typedef ::xsd::cxx::tree::time< char, simple_type > time;

  // Entity.
  //
  typedef ::xsd::cxx::tree::entity< char, ncname > entity;
  typedef ::xsd::cxx::tree::entities< char, simple_type, entity > entities;

  // Flags and properties.
  //
  typedef ::xsd::cxx::tree::flags flags;
  typedef ::xsd::cxx::tree::properties< char > properties;

  // Parsing/serialization diagnostics.
  //
  typedef ::xsd::cxx::tree::severity severity;
  typedef ::xsd::cxx::tree::error< char > error;
  typedef ::xsd::cxx::tree::diagnostics< char > diagnostics;

  // Exceptions.
  //
  typedef ::xsd::cxx::tree::exception< char > exception;
  typedef ::xsd::cxx::tree::bounds< char > bounds;
  typedef ::xsd::cxx::tree::duplicate_id< char > duplicate_id;
  typedef ::xsd::cxx::tree::parsing< char > parsing;
  typedef ::xsd::cxx::tree::expected_element< char > expected_element;
  typedef ::xsd::cxx::tree::unexpected_element< char > unexpected_element;
  typedef ::xsd::cxx::tree::expected_attribute< char > expected_attribute;
  typedef ::xsd::cxx::tree::unexpected_enumerator< char > unexpected_enumerator;
  typedef ::xsd::cxx::tree::expected_text_content< char > expected_text_content;
  typedef ::xsd::cxx::tree::no_prefix_mapping< char > no_prefix_mapping;

  // Error handler callback interface.
  //
  typedef ::xsd::cxx::xml::error_handler< char > error_handler;

  // DOM interaction.
  //
  namespace dom
  {
    // Automatic pointer for DOMDocument.
    //
    using ::xsd::cxx::xml::dom::auto_ptr;

#ifndef XSD_CXX_TREE_TREE_NODE_KEY__XML_SCHEMA
#define XSD_CXX_TREE_TREE_NODE_KEY__XML_SCHEMA
    // DOM user data key for back pointers to tree nodes.
    //
    const XMLCh* const tree_node_key = ::xsd::cxx::tree::user_data_keys::node;
#endif
  }
}

// Forward declarations.
//
class droplist_t;
class npc_t;
class tier_t;
class item_t;

#include <memory>    // std::auto_ptr
#include <limits>    // std::numeric_limits
#include <algorithm> // std::binary_search

#include <xsd/cxx/xml/char-utf8.hxx>

#include <xsd/cxx/tree/exceptions.hxx>
#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/containers.hxx>
#include <xsd/cxx/tree/list.hxx>

#include <xsd/cxx/xml/dom/parsing-header.hxx>

class droplist_t: public ::xml_schema::type
{
  public:
  // npc_t
  // 
  typedef ::npc_t npc_t_type;
  typedef ::xsd::cxx::tree::sequence< npc_t_type > npc_t_sequence;
  typedef npc_t_sequence::iterator npc_t_iterator;
  typedef npc_t_sequence::const_iterator npc_t_const_iterator;
  typedef ::xsd::cxx::tree::traits< npc_t_type, char > npc_t_traits;

  const npc_t_sequence&
  npc_t () const;

  npc_t_sequence&
  npc_t ();

  void
  npc_t (const npc_t_sequence& s);

  // Constructors.
  //
  droplist_t ();

  droplist_t (const ::xercesc::DOMElement& e,
              ::xml_schema::flags f = 0,
              ::xml_schema::container* c = 0);

  droplist_t (const droplist_t& x,
              ::xml_schema::flags f = 0,
              ::xml_schema::container* c = 0);

  virtual droplist_t*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  virtual 
  ~droplist_t ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  npc_t_sequence npc_t_;
};

class npc_t: public ::xml_schema::type
{
  public:
  // tier_t
  // 
  typedef ::tier_t tier_t_type;
  typedef ::xsd::cxx::tree::sequence< tier_t_type > tier_t_sequence;
  typedef tier_t_sequence::iterator tier_t_iterator;
  typedef tier_t_sequence::const_iterator tier_t_const_iterator;
  typedef ::xsd::cxx::tree::traits< tier_t_type, char > tier_t_traits;

  const tier_t_sequence&
  tier_t () const;

  tier_t_sequence&
  tier_t ();

  void
  tier_t (const tier_t_sequence& s);

  // name
  // 
  typedef ::xml_schema::string name_type;
  typedef ::xsd::cxx::tree::traits< name_type, char > name_traits;

  const name_type&
  name () const;

  name_type&
  name ();

  void
  name (const name_type& x);

  void
  name (::std::auto_ptr< name_type > p);

  // num
  // 
  typedef ::xml_schema::positive_integer num_type;
  typedef ::xsd::cxx::tree::traits< num_type, char > num_traits;

  const num_type&
  num () const;

  num_type&
  num ();

  void
  num (const num_type& x);

  static num_type
  num_default_value ();

  // Constructors.
  //
  npc_t (const name_type&);

  npc_t (const ::xercesc::DOMElement& e,
         ::xml_schema::flags f = 0,
         ::xml_schema::container* c = 0);

  npc_t (const npc_t& x,
         ::xml_schema::flags f = 0,
         ::xml_schema::container* c = 0);

  virtual npc_t*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  virtual 
  ~npc_t ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  tier_t_sequence tier_t_;
  ::xsd::cxx::tree::one< name_type > name_;
  ::xsd::cxx::tree::one< num_type > num_;
};

class tier_t: public ::xml_schema::type
{
  public:
  // item_t
  // 
  typedef ::item_t item_t_type;
  typedef ::xsd::cxx::tree::sequence< item_t_type > item_t_sequence;
  typedef item_t_sequence::iterator item_t_iterator;
  typedef item_t_sequence::const_iterator item_t_const_iterator;
  typedef ::xsd::cxx::tree::traits< item_t_type, char > item_t_traits;

  const item_t_sequence&
  item_t () const;

  item_t_sequence&
  item_t ();

  void
  item_t (const item_t_sequence& s);

  // prob
  // 
  typedef ::xml_schema::positive_integer prob_type;
  typedef ::xsd::cxx::tree::traits< prob_type, char > prob_traits;

  const prob_type&
  prob () const;

  prob_type&
  prob ();

  void
  prob (const prob_type& x);

  // type
  // 
  typedef ::xml_schema::string type_type;
  typedef ::xsd::cxx::tree::traits< type_type, char > type_traits;

  const type_type&
  type () const;

  type_type&
  type ();

  void
  type (const type_type& x);

  void
  type (::std::auto_ptr< type_type > p);

  // Constructors.
  //
  tier_t (const prob_type&,
          const type_type&);

  tier_t (const ::xercesc::DOMElement& e,
          ::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0);

  tier_t (const tier_t& x,
          ::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0);

  virtual tier_t*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  virtual 
  ~tier_t ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  item_t_sequence item_t_;
  ::xsd::cxx::tree::one< prob_type > prob_;
  ::xsd::cxx::tree::one< type_type > type_;
};

class item_t: public ::xml_schema::type
{
  public:
  // name
  // 
  typedef ::xml_schema::string name_type;
  typedef ::xsd::cxx::tree::traits< name_type, char > name_traits;

  const name_type&
  name () const;

  name_type&
  name ();

  void
  name (const name_type& x);

  void
  name (::std::auto_ptr< name_type > p);

  // prob
  // 
  typedef ::xml_schema::positive_integer prob_type;
  typedef ::xsd::cxx::tree::traits< prob_type, char > prob_traits;

  const prob_type&
  prob () const;

  prob_type&
  prob ();

  void
  prob (const prob_type& x);

  // tempattr
  // 
  typedef ::xml_schema::positive_integer tempattr_type;
  typedef ::xsd::cxx::tree::optional< tempattr_type > tempattr_optional;
  typedef ::xsd::cxx::tree::traits< tempattr_type, char > tempattr_traits;

  const tempattr_optional&
  tempattr () const;

  tempattr_optional&
  tempattr ();

  void
  tempattr (const tempattr_type& x);

  void
  tempattr (const tempattr_optional& x);

  // tempcharge
  // 
  typedef ::xml_schema::positive_integer tempcharge_type;
  typedef ::xsd::cxx::tree::optional< tempcharge_type > tempcharge_optional;
  typedef ::xsd::cxx::tree::traits< tempcharge_type, char > tempcharge_traits;

  const tempcharge_optional&
  tempcharge () const;

  tempcharge_optional&
  tempcharge ();

  void
  tempcharge (const tempcharge_type& x);

  void
  tempcharge (const tempcharge_optional& x);

  // Constructors.
  //
  item_t (const name_type&,
          const prob_type&);

  item_t (const ::xercesc::DOMElement& e,
          ::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0);

  item_t (const item_t& x,
          ::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0);

  virtual item_t*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  virtual 
  ~item_t ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  ::xsd::cxx::tree::one< name_type > name_;
  ::xsd::cxx::tree::one< prob_type > prob_;
  tempattr_optional tempattr_;
  tempcharge_optional tempcharge_;
};

#include <iosfwd>

#include <xercesc/sax/InputSource.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

// Parse a URI or a local file.
//

::std::auto_ptr< ::droplist_t >
droplist (const ::std::string& uri,
          ::xml_schema::flags f = 0,
          const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::auto_ptr< ::droplist_t >
droplist (const ::std::string& uri,
          ::xml_schema::error_handler& eh,
          ::xml_schema::flags f = 0,
          const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::auto_ptr< ::droplist_t >
droplist (const ::std::string& uri,
          ::xercesc::DOMErrorHandler& eh,
          ::xml_schema::flags f = 0,
          const ::xml_schema::properties& p = ::xml_schema::properties ());

// Parse std::istream.
//

::std::auto_ptr< ::droplist_t >
droplist (::std::istream& is,
          ::xml_schema::flags f = 0,
          const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::auto_ptr< ::droplist_t >
droplist (::std::istream& is,
          ::xml_schema::error_handler& eh,
          ::xml_schema::flags f = 0,
          const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::auto_ptr< ::droplist_t >
droplist (::std::istream& is,
          ::xercesc::DOMErrorHandler& eh,
          ::xml_schema::flags f = 0,
          const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::auto_ptr< ::droplist_t >
droplist (::std::istream& is,
          const ::std::string& id,
          ::xml_schema::flags f = 0,
          const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::auto_ptr< ::droplist_t >
droplist (::std::istream& is,
          const ::std::string& id,
          ::xml_schema::error_handler& eh,
          ::xml_schema::flags f = 0,
          const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::auto_ptr< ::droplist_t >
droplist (::std::istream& is,
          const ::std::string& id,
          ::xercesc::DOMErrorHandler& eh,
          ::xml_schema::flags f = 0,
          const ::xml_schema::properties& p = ::xml_schema::properties ());

// Parse xercesc::InputSource.
//

::std::auto_ptr< ::droplist_t >
droplist (::xercesc::InputSource& is,
          ::xml_schema::flags f = 0,
          const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::auto_ptr< ::droplist_t >
droplist (::xercesc::InputSource& is,
          ::xml_schema::error_handler& eh,
          ::xml_schema::flags f = 0,
          const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::auto_ptr< ::droplist_t >
droplist (::xercesc::InputSource& is,
          ::xercesc::DOMErrorHandler& eh,
          ::xml_schema::flags f = 0,
          const ::xml_schema::properties& p = ::xml_schema::properties ());

// Parse xercesc::DOMDocument.
//

::std::auto_ptr< ::droplist_t >
droplist (const ::xercesc::DOMDocument& d,
          ::xml_schema::flags f = 0,
          const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::auto_ptr< ::droplist_t >
droplist (::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument >& d,
          ::xml_schema::flags f = 0,
          const ::xml_schema::properties& p = ::xml_schema::properties ());

#include <xsd/cxx/post.hxx>

// Begin epilogue.
//
//
// End epilogue.

#endif // DESKTOP_DROPLIST_HXX
