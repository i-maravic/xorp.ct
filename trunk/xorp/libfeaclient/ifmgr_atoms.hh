// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-

// Copyright (c) 2001-2003 International Computer Science Institute
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software")
// to deal in the Software without restriction, subject to the conditions
// listed in the XORP LICENSE file. These conditions include: you must
// preserve this copyright notice, and you cannot mention the copyright
// holders in advertising related to the Software without their permission.
// The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
// notice is a summary of the XORP LICENSE file; the license in that file is
// legally binding.

// $XORP: xorp/devnotes/template.hh,v 1.2 2003/01/16 19:08:48 mjh Exp $

#ifndef __LIBFEACLIENT_IFMGR_ATOMS_HH__
#define __LIBFEACLIENT_IFMGR_ATOMS_HH__

#include "libxorp/xorp.h"

#include <map>
#include <string>

#include "libxorp/ipv4.hh"
#include "libxorp/ipv6.hh"
#include "libxorp/mac.hh"

class IfMgrIfAtom;
class IfMgrVifAtom;
class IfMgrIPv4Atom;
class IfMgrIPv6Atom;

class IfMgrIfTree {
public:
    typedef map<const string, IfMgrIfAtom> IfMap;

public:
    inline const IfMap& interfaces() const		{ return _ifs; }
    inline IfMap& interfaces()				{ return _ifs; }

    const IfMgrIfAtom* find_if(const string& ifname) const;

    IfMgrIfAtom* find_if(const string& ifname);

    const IfMgrVifAtom* find_vif(const string& ifname,
				 const string& vifname) const;

    IfMgrVifAtom* find_vif(const string& ifname,
			   const string& vifname);

    const IfMgrIPv4Atom* find_addr(const string& ifname,
				   const string& vifname,
				   const IPv4	 addr) const;

    IfMgrIPv4Atom* find_addr(const string& ifname,
			     const string& vifname,
			     const IPv4	   addr);

    const IfMgrIPv6Atom* find_addr(const string& ifname,
				   const string& vifname,
				   const IPv6&	 addr) const;

    IfMgrIPv6Atom* find_addr(const string& ifname,
			     const string& vifname,
			     const IPv6&   addr);

    bool operator==(const IfMgrIfTree& o) const;

protected:
    IfMap _ifs;
};


/**
 * @short Interface configuration atom.
 *
 * Represents an interface in XORP's model of forwarding h/w.
 */
class IfMgrIfAtom {
public:
    typedef map<const string, IfMgrVifAtom> VifMap;

public:
    inline IfMgrIfAtom(const string& name);

    inline const string& name() const			{ return _name; }

    inline bool		enabled() const			{ return _en; }
    inline void		set_enabled(bool en)		{ _en = en; }

    inline uint32_t	mtu_bytes() const		{ return _mtu; }
    inline void		set_mtu_bytes(uint32_t mtu) 	{ _mtu = mtu; }

    inline const Mac&	mac() const			{ return _mac; }
    inline void		set_mac(const Mac& mac)		{ _mac = mac; }

    inline const VifMap& vifs() const			{ return _vifs; }
    inline VifMap& vifs()				{ return _vifs; }

    bool operator==(const IfMgrIfAtom& o) const;

private:
    IfMgrIfAtom();		// not implemented

protected:
    string	_name;

    bool	_en;		// enabled
    uint32_t	_mtu;		// mtu in bytes
    Mac		_mac;		// MAC address

    VifMap	_vifs;		// map of vifname-vif
};


/**
 * @short Virtual Interface configuration atom.
 *
 * Represents a virtual interface in XORP's model of forwarding h/w.
 */
class IfMgrVifAtom {
public:
    typedef map<const IPv4, IfMgrIPv4Atom> V4Map;
    typedef map<const IPv6, IfMgrIPv6Atom> V6Map;

public:
    inline IfMgrVifAtom(IfMgrIfAtom* parent, const string& name);

    inline const string&      name() const		{ return _name; }
    inline const IfMgrIfAtom* parent() const		{ return _parent; }

    inline bool		enabled() const			{ return _en; }
    inline void		set_enabled(bool en)		{ _en = en; }

    inline bool		multicast_capable() const 	{ return _mcap; }
    inline void		set_multicast_capable(bool cap)	{ _mcap = cap; }

    inline bool		broadcast_capable() const 	{ return _bcap; }
    inline void		set_broadcast_capable(bool cap)	{ _bcap = cap; }

    inline bool		p2p_capable() const		{ return _p2pcap; }
    inline void		set_p2p_capable(bool cap)	{ _p2pcap = cap; }

    inline bool		loopback() const		{ return _loopback; }
    inline void		set_loopback(bool l)		{ _loopback = l; }

    inline uint32_t	pif_index() const		{ return _pif; }
    inline uint32_t	set_pif_index(uint32_t i) 	{ return _pif = i; }

    inline const V4Map&	ipv4addrs() const		{ return _v4addrs; }
    inline V4Map&	ipv4addrs() 			{ return _v4addrs; }

    inline const V6Map&	ipv6addrs() const		{ return _v6addrs; }
    inline V6Map&	ipv6addrs() 			{ return _v6addrs; }

    bool 		operator==(const IfMgrVifAtom& o) const;

private:
    IfMgrVifAtom();		// not implemented

protected:
    IfMgrIfAtom* _parent;
    string	 _name;

    bool	_en;		// enabled
    bool	_mcap;		// multicast capable
    bool	_bcap;		// broadcast capable
    bool	_p2pcap;	// point-to-point capable
    bool	_loopback;	// true if loopback Vif
    uint32_t	_pif;		// physical interface index

    V4Map	_v4addrs;
    V6Map	_v6addrs;
};


/**
 * @short IPv4 configuration atom.
 *
 * Represents an address associated with a virtual interface in XORP's model
 * of forwarding h/w.
 */
class IfMgrIPv4Atom {
public:
    inline IfMgrIPv4Atom(IfMgrVifAtom* parent, const IPv4& addr)
	: _parent(parent), _addr(addr)
    {}

    inline IPv4			addr() const		{ return _addr; }
    inline const IfMgrVifAtom*	parent() const		{ return _parent; }

    inline uint32_t	prefix() const			{ return _prefix; }
    inline void		set_prefix(uint32_t p)		{ _prefix = p; }

    inline bool		enabled() const			{ return _en; }
    inline void		set_enabled(bool en)		{ _en = en; }

    inline bool		multicast_capable() const 	{ return _mcap; }
    inline void		set_multicast_capable(bool cap)	{ _mcap = cap; }

    inline bool		loopback() const		{ return _loop; }
    inline void		set_loopback(bool l) 		{ _loop = l; }

    inline bool		has_broadcast() const		{ return _bcast; }
    inline void		remove_broadcast()		{ _bcast = false; }
    inline void		set_broadcast_addr(const IPv4 baddr);
    inline IPv4		broadcast_addr() const;

    inline bool		has_endpoint() const		{ return _p2p; }
    inline void		remove_endpoint()		{ _p2p = false; }
    inline void		set_endpoint_addr(const IPv4 endpoint);
    inline IPv4		endpoint_addr() const;

    bool 		operator==(const IfMgrIPv4Atom& o) const;

private:
    IfMgrIPv4Atom();		// not implemented

protected:
    IfMgrVifAtom* _parent;
    IPv4	  _addr;
    uint32_t	  _prefix;	// network prefix

    bool	  _en;		// enabled
    bool	  _mcap;	// multicast capable
    bool	  _loop;	// Is a loopback address
    bool	  _bcast;	// _oaddr refers to a broadcast addr
    bool	  _p2p;		// _oaddr refers to a point-to-point address

    IPv4	  _oaddr;	// Other address [bcast|p2p]
};


/**
 * @short IPv6 configuration atom.
 *
 * Represents an address associated with a virtual interface in XORP's model
 * of forwarding h/w.
 */

class IfMgrIPv6Atom {
public:
    inline IfMgrIPv6Atom(IfMgrVifAtom* parent, const IPv6& addr)
	: _parent(parent), _addr(addr)
    {}

    inline const IPv6&         addr() const		{ return _addr; }
    inline const IfMgrVifAtom* parent() const		{ return _parent; }

    inline bool		enabled() const			{ return _en; }
    inline void		set_enabled(bool en)		{ _en = en; }

    inline uint32_t	prefix() const			{ return _prefix; }
    inline void		set_prefix(uint32_t p)		{ _prefix = p; }

    inline bool		multicast_capable() const 	{ return _mcap; }
    inline void		set_multicast_capable(bool cap)	{ _mcap = cap; }

    inline bool		loopback() const		{ return _loop; }
    inline void		set_loopback(bool l) 		{ _loop = l; }

    inline bool		has_endpoint() const		{ return _p2p; }
    inline void		remove_endpoint()		{ _p2p = false; }
    inline void		set_endpoint_addr(const IPv6& endpoint);
    inline const IPv6&	endpoint_addr() const;

    bool 		operator==(const IfMgrIPv6Atom& o) const;

private:
    IfMgrIPv6Atom();		// not implemented

protected:
    IfMgrVifAtom* _parent;
    IPv6	  _addr;
    uint32_t	  _prefix;

    bool	_en;		// enabled
    bool	_mcap;		// multicast capable
    bool	_loop;		// Is a loopback address
    bool	_p2p;		// _oaddr refers to a point-to-point address

    IPv6	_oaddr;
};


// ----------------------------------------------------------------------------
// Inline IfMgrIfAtom methods

inline
IfMgrIfAtom::IfMgrIfAtom(const string& name)
    : _name(name)
{}

// ----------------------------------------------------------------------------
// Inline IfMgrVifAtom methods

inline
IfMgrVifAtom::IfMgrVifAtom(IfMgrIfAtom* parent, const string& name)
    : _parent(parent), _name(name)
{}

// ----------------------------------------------------------------------------
// Inline IfMgrIPv4Atom methods

inline void
IfMgrIPv4Atom::set_broadcast_addr(const IPv4 oaddr)
{
    if (oaddr == IPv4::ZERO()) {
	_bcast = false;
    } else {
	_bcast = true;
	_p2p = false;
	_oaddr = oaddr;
    }
}

inline IPv4
IfMgrIPv4Atom::broadcast_addr() const
{
    return _bcast ? _oaddr : IPv4::ZERO();
}

inline void
IfMgrIPv4Atom::set_endpoint_addr(const IPv4 oaddr)
{
    if (oaddr == IPv4::ZERO()) {
	_p2p = false;
    } else {
	_p2p = true;
	_bcast = false;
	_oaddr = oaddr;
    }
}

inline IPv4
IfMgrIPv4Atom::endpoint_addr() const
{
    return _p2p ? _oaddr : IPv4::ZERO();
}


// ----------------------------------------------------------------------------
// Inline IfMgrIPv6Atom methods

inline void
IfMgrIPv6Atom::set_endpoint_addr(const IPv6& oaddr)
{
    if (oaddr == IPv6::ZERO()) {
	_p2p = false;
    } else {
	_p2p = true;
	_oaddr = oaddr;
    }
}

inline const IPv6&
IfMgrIPv6Atom::endpoint_addr() const
{
    return _p2p ? _oaddr : IPv6::ZERO();
}

#endif // __LIBFEACLIENT_IFMGR_ATOMS_HH__

