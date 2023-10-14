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

// Begin prologue.
//
//
// End prologue.

#include <xsd/cxx/pre.hxx>

#include "droplist.hxx"

// droplist_t
// 

const droplist_t::npc_t_sequence& droplist_t::
npc_t () const
{
  return this->npc_t_;
}

droplist_t::npc_t_sequence& droplist_t::
npc_t ()
{
  return this->npc_t_;
}

void droplist_t::
npc_t (const npc_t_sequence& s)
{
  this->npc_t_ = s;
}


// npc_t
// 

const npc_t::tier_t_sequence& npc_t::
tier_t () const
{
  return this->tier_t_;
}

npc_t::tier_t_sequence& npc_t::
tier_t ()
{
  return this->tier_t_;
}

void npc_t::
tier_t (const tier_t_sequence& s)
{
  this->tier_t_ = s;
}

const npc_t::name_type& npc_t::
name () const
{
  return this->name_.get ();
}

npc_t::name_type& npc_t::
name ()
{
  return this->name_.get ();
}

void npc_t::
name (const name_type& x)
{
  this->name_.set (x);
}

void npc_t::
name (::std::auto_ptr< name_type > x)
{
  this->name_.set (x);
}

const npc_t::num_type& npc_t::
num () const
{
  return this->num_.get ();
}

npc_t::num_type& npc_t::
num ()
{
  return this->num_.get ();
}

void npc_t::
num (const num_type& x)
{
  this->num_.set (x);
}

npc_t::num_type npc_t::
num_default_value ()
{
  return num_type (1ULL);
}


// tier_t
// 

const tier_t::item_t_sequence& tier_t::
item_t () const
{
  return this->item_t_;
}

tier_t::item_t_sequence& tier_t::
item_t ()
{
  return this->item_t_;
}

void tier_t::
item_t (const item_t_sequence& s)
{
  this->item_t_ = s;
}

const tier_t::prob_type& tier_t::
prob () const
{
  return this->prob_.get ();
}

tier_t::prob_type& tier_t::
prob ()
{
  return this->prob_.get ();
}

void tier_t::
prob (const prob_type& x)
{
  this->prob_.set (x);
}

const tier_t::type_type& tier_t::
type () const
{
  return this->type_.get ();
}

tier_t::type_type& tier_t::
type ()
{
  return this->type_.get ();
}

void tier_t::
type (const type_type& x)
{
  this->type_.set (x);
}

void tier_t::
type (::std::auto_ptr< type_type > x)
{
  this->type_.set (x);
}


// item_t
// 

const item_t::name_type& item_t::
name () const
{
  return this->name_.get ();
}

item_t::name_type& item_t::
name ()
{
  return this->name_.get ();
}

void item_t::
name (const name_type& x)
{
  this->name_.set (x);
}

void item_t::
name (::std::auto_ptr< name_type > x)
{
  this->name_.set (x);
}

const item_t::prob_type& item_t::
prob () const
{
  return this->prob_.get ();
}

item_t::prob_type& item_t::
prob ()
{
  return this->prob_.get ();
}

void item_t::
prob (const prob_type& x)
{
  this->prob_.set (x);
}

const item_t::tempattr_optional& item_t::
tempattr () const
{
  return this->tempattr_;
}

item_t::tempattr_optional& item_t::
tempattr ()
{
  return this->tempattr_;
}

void item_t::
tempattr (const tempattr_type& x)
{
  this->tempattr_.set (x);
}

void item_t::
tempattr (const tempattr_optional& x)
{
  this->tempattr_ = x;
}

const item_t::tempcharge_optional& item_t::
tempcharge () const
{
  return this->tempcharge_;
}

item_t::tempcharge_optional& item_t::
tempcharge ()
{
  return this->tempcharge_;
}

void item_t::
tempcharge (const tempcharge_type& x)
{
  this->tempcharge_.set (x);
}

void item_t::
tempcharge (const tempcharge_optional& x)
{
  this->tempcharge_ = x;
}


#include <xsd/cxx/xml/dom/parsing-source.hxx>

// droplist_t
//

droplist_t::
droplist_t ()
: ::xml_schema::type (),
  npc_t_ (::xml_schema::flags (), this)
{
}

droplist_t::
droplist_t (const droplist_t& x,
            ::xml_schema::flags f,
            ::xml_schema::container* c)
: ::xml_schema::type (x, f, c),
  npc_t_ (x.npc_t_, f, this)
{
}

droplist_t::
droplist_t (const ::xercesc::DOMElement& e,
            ::xml_schema::flags f,
            ::xml_schema::container* c)
: ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
  npc_t_ (f, this)
{
  if ((f & ::xml_schema::flags::base) == 0)
  {
    ::xsd::cxx::xml::dom::parser< char > p (e, true, false);
    this->parse (p, f);
  }
}

void droplist_t::
parse (::xsd::cxx::xml::dom::parser< char >& p,
       ::xml_schema::flags f)
{
  for (; p.more_elements (); p.next_element ())
  {
    const ::xercesc::DOMElement& i (p.cur_element ());
    const ::xsd::cxx::xml::qualified_name< char > n (
      ::xsd::cxx::xml::dom::name< char > (i));

    // npc_t
    //
    if (n.name () == "npc_t" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< npc_t_type > r (
        npc_t_traits::create (i, f, this));

      this->npc_t_.push_back (r);
      continue;
    }

    break;
  }
}

droplist_t* droplist_t::
_clone (::xml_schema::flags f,
        ::xml_schema::container* c) const
{
  return new class droplist_t (*this, f, c);
}

droplist_t::
~droplist_t ()
{
}

// npc_t
//

npc_t::
npc_t (const name_type& name)
: ::xml_schema::type (),
  tier_t_ (::xml_schema::flags (), this),
  name_ (name, ::xml_schema::flags (), this),
  num_ (num_default_value (), ::xml_schema::flags (), this)
{
}

npc_t::
npc_t (const npc_t& x,
       ::xml_schema::flags f,
       ::xml_schema::container* c)
: ::xml_schema::type (x, f, c),
  tier_t_ (x.tier_t_, f, this),
  name_ (x.name_, f, this),
  num_ (x.num_, f, this)
{
}

npc_t::
npc_t (const ::xercesc::DOMElement& e,
       ::xml_schema::flags f,
       ::xml_schema::container* c)
: ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
  tier_t_ (f, this),
  name_ (f, this),
  num_ (f, this)
{
  if ((f & ::xml_schema::flags::base) == 0)
  {
    ::xsd::cxx::xml::dom::parser< char > p (e, true, true);
    this->parse (p, f);
  }
}

void npc_t::
parse (::xsd::cxx::xml::dom::parser< char >& p,
       ::xml_schema::flags f)
{
  for (; p.more_elements (); p.next_element ())
  {
    const ::xercesc::DOMElement& i (p.cur_element ());
    const ::xsd::cxx::xml::qualified_name< char > n (
      ::xsd::cxx::xml::dom::name< char > (i));

    // tier_t
    //
    if (n.name () == "tier_t" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< tier_t_type > r (
        tier_t_traits::create (i, f, this));

      this->tier_t_.push_back (r);
      continue;
    }

    break;
  }

  while (p.more_attributes ())
  {
    const ::xercesc::DOMAttr& i (p.next_attribute ());
    const ::xsd::cxx::xml::qualified_name< char > n (
      ::xsd::cxx::xml::dom::name< char > (i));

    if (n.name () == "name" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< name_type > r (
        name_traits::create (i, f, this));

      this->name_.set (r);
      continue;
    }

    if (n.name () == "num" && n.namespace_ ().empty ())
    {
      this->num_.set (num_traits::create (i, f, this));
      continue;
    }
  }

  if (!name_.present ())
  {
    throw ::xsd::cxx::tree::expected_attribute< char > (
      "name",
      "");
  }

  if (!num_.present ())
  {
    this->num_.set (num_default_value ());
  }
}

npc_t* npc_t::
_clone (::xml_schema::flags f,
        ::xml_schema::container* c) const
{
  return new class npc_t (*this, f, c);
}

npc_t::
~npc_t ()
{
}

// tier_t
//

tier_t::
tier_t (const prob_type& prob,
        const type_type& type)
: ::xml_schema::type (),
  item_t_ (::xml_schema::flags (), this),
  prob_ (prob, ::xml_schema::flags (), this),
  type_ (type, ::xml_schema::flags (), this)
{
}

tier_t::
tier_t (const tier_t& x,
        ::xml_schema::flags f,
        ::xml_schema::container* c)
: ::xml_schema::type (x, f, c),
  item_t_ (x.item_t_, f, this),
  prob_ (x.prob_, f, this),
  type_ (x.type_, f, this)
{
}

tier_t::
tier_t (const ::xercesc::DOMElement& e,
        ::xml_schema::flags f,
        ::xml_schema::container* c)
: ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
  item_t_ (f, this),
  prob_ (f, this),
  type_ (f, this)
{
  if ((f & ::xml_schema::flags::base) == 0)
  {
    ::xsd::cxx::xml::dom::parser< char > p (e, true, true);
    this->parse (p, f);
  }
}

void tier_t::
parse (::xsd::cxx::xml::dom::parser< char >& p,
       ::xml_schema::flags f)
{
  for (; p.more_elements (); p.next_element ())
  {
    const ::xercesc::DOMElement& i (p.cur_element ());
    const ::xsd::cxx::xml::qualified_name< char > n (
      ::xsd::cxx::xml::dom::name< char > (i));

    // item_t
    //
    if (n.name () == "item_t" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< item_t_type > r (
        item_t_traits::create (i, f, this));

      this->item_t_.push_back (r);
      continue;
    }

    break;
  }

  while (p.more_attributes ())
  {
    const ::xercesc::DOMAttr& i (p.next_attribute ());
    const ::xsd::cxx::xml::qualified_name< char > n (
      ::xsd::cxx::xml::dom::name< char > (i));

    if (n.name () == "prob" && n.namespace_ ().empty ())
    {
      this->prob_.set (prob_traits::create (i, f, this));
      continue;
    }

    if (n.name () == "type" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< type_type > r (
        type_traits::create (i, f, this));

      this->type_.set (r);
      continue;
    }
  }

  if (!prob_.present ())
  {
    throw ::xsd::cxx::tree::expected_attribute< char > (
      "prob",
      "");
  }

  if (!type_.present ())
  {
    throw ::xsd::cxx::tree::expected_attribute< char > (
      "type",
      "");
  }
}

tier_t* tier_t::
_clone (::xml_schema::flags f,
        ::xml_schema::container* c) const
{
  return new class tier_t (*this, f, c);
}

tier_t::
~tier_t ()
{
}

// item_t
//

item_t::
item_t (const name_type& name,
        const prob_type& prob)
: ::xml_schema::type (),
  name_ (name, ::xml_schema::flags (), this),
  prob_ (prob, ::xml_schema::flags (), this),
  tempattr_ (::xml_schema::flags (), this),
  tempcharge_ (::xml_schema::flags (), this)
{
}

item_t::
item_t (const item_t& x,
        ::xml_schema::flags f,
        ::xml_schema::container* c)
: ::xml_schema::type (x, f, c),
  name_ (x.name_, f, this),
  prob_ (x.prob_, f, this),
  tempattr_ (x.tempattr_, f, this),
  tempcharge_ (x.tempcharge_, f, this)
{
}

item_t::
item_t (const ::xercesc::DOMElement& e,
        ::xml_schema::flags f,
        ::xml_schema::container* c)
: ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
  name_ (f, this),
  prob_ (f, this),
  tempattr_ (f, this),
  tempcharge_ (f, this)
{
  if ((f & ::xml_schema::flags::base) == 0)
  {
    ::xsd::cxx::xml::dom::parser< char > p (e, false, true);
    this->parse (p, f);
  }
}

void item_t::
parse (::xsd::cxx::xml::dom::parser< char >& p,
       ::xml_schema::flags f)
{
  while (p.more_attributes ())
  {
    const ::xercesc::DOMAttr& i (p.next_attribute ());
    const ::xsd::cxx::xml::qualified_name< char > n (
      ::xsd::cxx::xml::dom::name< char > (i));

    if (n.name () == "name" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< name_type > r (
        name_traits::create (i, f, this));

      this->name_.set (r);
      continue;
    }

    if (n.name () == "prob" && n.namespace_ ().empty ())
    {
      this->prob_.set (prob_traits::create (i, f, this));
      continue;
    }

    if (n.name () == "tempattr" && n.namespace_ ().empty ())
    {
      this->tempattr_.set (tempattr_traits::create (i, f, this));
      continue;
    }

    if (n.name () == "tempcharge" && n.namespace_ ().empty ())
    {
      this->tempcharge_.set (tempcharge_traits::create (i, f, this));
      continue;
    }
  }

  if (!name_.present ())
  {
    throw ::xsd::cxx::tree::expected_attribute< char > (
      "name",
      "");
  }

  if (!prob_.present ())
  {
    throw ::xsd::cxx::tree::expected_attribute< char > (
      "prob",
      "");
  }
}

item_t* item_t::
_clone (::xml_schema::flags f,
        ::xml_schema::container* c) const
{
  return new class item_t (*this, f, c);
}

item_t::
~item_t ()
{
}

#include <istream>
#include <xsd/cxx/xml/sax/std-input-source.hxx>
#include <xsd/cxx/tree/error-handler.hxx>

::std::auto_ptr< ::droplist_t >
droplist (const ::std::string& u,
          ::xml_schema::flags f,
          const ::xml_schema::properties& p)
{
  ::xsd::cxx::xml::auto_initializer i (
    (f & ::xml_schema::flags::dont_initialize) == 0,
    (f & ::xml_schema::flags::keep_dom) == 0);

  ::xsd::cxx::tree::error_handler< char > h;

  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::xsd::cxx::xml::dom::parse< char > (
      u, h, p, f));

  h.throw_if_failed< ::xsd::cxx::tree::parsing< char > > ();

  ::std::auto_ptr< ::droplist_t > r (
    ::droplist (
      d, f | ::xml_schema::flags::own_dom, p));

  return r;
}

::std::auto_ptr< ::droplist_t >
droplist (const ::std::string& u,
          ::xml_schema::error_handler& h,
          ::xml_schema::flags f,
          const ::xml_schema::properties& p)
{
  ::xsd::cxx::xml::auto_initializer i (
    (f & ::xml_schema::flags::dont_initialize) == 0,
    (f & ::xml_schema::flags::keep_dom) == 0);

  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::xsd::cxx::xml::dom::parse< char > (
      u, h, p, f));

  if (!d.get ())
    throw ::xsd::cxx::tree::parsing< char > ();

  ::std::auto_ptr< ::droplist_t > r (
    ::droplist (
      d, f | ::xml_schema::flags::own_dom, p));

  return r;
}

::std::auto_ptr< ::droplist_t >
droplist (const ::std::string& u,
          ::xercesc::DOMErrorHandler& h,
          ::xml_schema::flags f,
          const ::xml_schema::properties& p)
{
  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::xsd::cxx::xml::dom::parse< char > (
      u, h, p, f));

  if (!d.get ())
    throw ::xsd::cxx::tree::parsing< char > ();

  ::std::auto_ptr< ::droplist_t > r (
    ::droplist (
      d, f | ::xml_schema::flags::own_dom, p));

  return r;
}

::std::auto_ptr< ::droplist_t >
droplist (::std::istream& is,
          ::xml_schema::flags f,
          const ::xml_schema::properties& p)
{
  ::xsd::cxx::xml::auto_initializer i (
    (f & ::xml_schema::flags::dont_initialize) == 0,
    (f & ::xml_schema::flags::keep_dom) == 0);

  ::xsd::cxx::xml::sax::std_input_source isrc (is);
  return ::droplist (isrc, f, p);
}

::std::auto_ptr< ::droplist_t >
droplist (::std::istream& is,
          ::xml_schema::error_handler& h,
          ::xml_schema::flags f,
          const ::xml_schema::properties& p)
{
  ::xsd::cxx::xml::auto_initializer i (
    (f & ::xml_schema::flags::dont_initialize) == 0,
    (f & ::xml_schema::flags::keep_dom) == 0);

  ::xsd::cxx::xml::sax::std_input_source isrc (is);
  return ::droplist (isrc, h, f, p);
}

::std::auto_ptr< ::droplist_t >
droplist (::std::istream& is,
          ::xercesc::DOMErrorHandler& h,
          ::xml_schema::flags f,
          const ::xml_schema::properties& p)
{
  ::xsd::cxx::xml::sax::std_input_source isrc (is);
  return ::droplist (isrc, h, f, p);
}

::std::auto_ptr< ::droplist_t >
droplist (::std::istream& is,
          const ::std::string& sid,
          ::xml_schema::flags f,
          const ::xml_schema::properties& p)
{
  ::xsd::cxx::xml::auto_initializer i (
    (f & ::xml_schema::flags::dont_initialize) == 0,
    (f & ::xml_schema::flags::keep_dom) == 0);

  ::xsd::cxx::xml::sax::std_input_source isrc (is, sid);
  return ::droplist (isrc, f, p);
}

::std::auto_ptr< ::droplist_t >
droplist (::std::istream& is,
          const ::std::string& sid,
          ::xml_schema::error_handler& h,
          ::xml_schema::flags f,
          const ::xml_schema::properties& p)
{
  ::xsd::cxx::xml::auto_initializer i (
    (f & ::xml_schema::flags::dont_initialize) == 0,
    (f & ::xml_schema::flags::keep_dom) == 0);

  ::xsd::cxx::xml::sax::std_input_source isrc (is, sid);
  return ::droplist (isrc, h, f, p);
}

::std::auto_ptr< ::droplist_t >
droplist (::std::istream& is,
          const ::std::string& sid,
          ::xercesc::DOMErrorHandler& h,
          ::xml_schema::flags f,
          const ::xml_schema::properties& p)
{
  ::xsd::cxx::xml::sax::std_input_source isrc (is, sid);
  return ::droplist (isrc, h, f, p);
}

::std::auto_ptr< ::droplist_t >
droplist (::xercesc::InputSource& i,
          ::xml_schema::flags f,
          const ::xml_schema::properties& p)
{
  ::xsd::cxx::tree::error_handler< char > h;

  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::xsd::cxx::xml::dom::parse< char > (
      i, h, p, f));

  h.throw_if_failed< ::xsd::cxx::tree::parsing< char > > ();

  ::std::auto_ptr< ::droplist_t > r (
    ::droplist (
      d, f | ::xml_schema::flags::own_dom, p));

  return r;
}

::std::auto_ptr< ::droplist_t >
droplist (::xercesc::InputSource& i,
          ::xml_schema::error_handler& h,
          ::xml_schema::flags f,
          const ::xml_schema::properties& p)
{
  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::xsd::cxx::xml::dom::parse< char > (
      i, h, p, f));

  if (!d.get ())
    throw ::xsd::cxx::tree::parsing< char > ();

  ::std::auto_ptr< ::droplist_t > r (
    ::droplist (
      d, f | ::xml_schema::flags::own_dom, p));

  return r;
}

::std::auto_ptr< ::droplist_t >
droplist (::xercesc::InputSource& i,
          ::xercesc::DOMErrorHandler& h,
          ::xml_schema::flags f,
          const ::xml_schema::properties& p)
{
  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::xsd::cxx::xml::dom::parse< char > (
      i, h, p, f));

  if (!d.get ())
    throw ::xsd::cxx::tree::parsing< char > ();

  ::std::auto_ptr< ::droplist_t > r (
    ::droplist (
      d, f | ::xml_schema::flags::own_dom, p));

  return r;
}

::std::auto_ptr< ::droplist_t >
droplist (const ::xercesc::DOMDocument& d,
          ::xml_schema::flags f,
          const ::xml_schema::properties& p)
{
  if (f & ::xml_schema::flags::keep_dom)
  {
    ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > c (
      static_cast< ::xercesc::DOMDocument* > (d.cloneNode (true)));

    ::std::auto_ptr< ::droplist_t > r (
      ::droplist (
        c, f | ::xml_schema::flags::own_dom, p));

    return r;
  }

  const ::xercesc::DOMElement& e (*d.getDocumentElement ());
  const ::xsd::cxx::xml::qualified_name< char > n (
    ::xsd::cxx::xml::dom::name< char > (e));

  if (n.name () == "droplist" &&
      n.namespace_ () == "")
  {
    ::std::auto_ptr< ::droplist_t > r (
      ::xsd::cxx::tree::traits< ::droplist_t, char >::create (
        e, f, 0));
    return r;
  }

  throw ::xsd::cxx::tree::unexpected_element < char > (
    n.name (),
    n.namespace_ (),
    "droplist",
    "");
}

::std::auto_ptr< ::droplist_t >
droplist (::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument >& d,
          ::xml_schema::flags f,
          const ::xml_schema::properties&)
{
  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > c (
    ((f & ::xml_schema::flags::keep_dom) &&
     !(f & ::xml_schema::flags::own_dom))
    ? static_cast< ::xercesc::DOMDocument* > (d->cloneNode (true))
    : 0);

  ::xercesc::DOMDocument& doc (c.get () ? *c : *d);
  const ::xercesc::DOMElement& e (*doc.getDocumentElement ());

  const ::xsd::cxx::xml::qualified_name< char > n (
    ::xsd::cxx::xml::dom::name< char > (e));

  if (f & ::xml_schema::flags::keep_dom)
    doc.setUserData (::xml_schema::dom::tree_node_key,
                     (c.get () ? &c : &d),
                     0);

  if (n.name () == "droplist" &&
      n.namespace_ () == "")
  {
    ::std::auto_ptr< ::droplist_t > r (
      ::xsd::cxx::tree::traits< ::droplist_t, char >::create (
        e, f, 0));
    return r;
  }

  throw ::xsd::cxx::tree::unexpected_element < char > (
    n.name (),
    n.namespace_ (),
    "droplist",
    "");
}

#include <xsd/cxx/post.hxx>

// Begin epilogue.
//
//
// End epilogue.
