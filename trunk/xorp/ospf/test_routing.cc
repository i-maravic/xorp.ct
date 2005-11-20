// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-
// vim:set sts=4 ts=8:

// Copyright (c) 2001-2004 International Computer Science Institute
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

#ident "$XORP: xorp/ospf/test_routing.cc,v 1.11 2005/11/20 19:16:30 atanu Exp $"

#include "config.h"
#include <map>
#include <list>
#include <set>
#include <deque>

#include "ospf_module.h"

#include "libxorp/test_main.hh"
#include "libxorp/debug.h"
#include "libxorp/xlog.h"
#include "libxorp/callback.hh"
#include "libxorp/tlv.hh"

#include "libxorp/ipv4.hh"
#include "libxorp/ipv6.hh"
#include "libxorp/ipnet.hh"

#include "libxorp/status_codes.h"
#include "libxorp/eventloop.hh"

#include "libproto/spt.hh"

#include "ospf.hh"
#include "debug_io.hh"
#include "delay_queue.hh"
#include "vertex.hh"
#include "area_router.hh"
#include "test_common.hh"

#ifndef	DEBUG_LOGGING
#define DEBUG_LOGGING
#endif /* DEBUG_LOGGING */
#ifndef	DEBUG_PRINT_FUNCTION_NAME
#define DEBUG_PRINT_FUNCTION_NAME
#endif /* DEBUG_PRINT_FUNCTION_NAME */

// Make sure that all tests free up any memory that they use. This will
// allow us to use the leak checker program.

void
p2p(OspfTypes::Version version, RouterLsa *rlsa, OspfTypes::RouterID id,
    uint32_t link_data, uint32_t metric)
{
     RouterLink rl(version);

     rl.set_type(RouterLink::p2p);
     rl.set_metric(metric);

    switch(version) {
    case OspfTypes::V2:
	rl.set_link_id(id);
	rl.set_link_data(link_data);
	break;
    case OspfTypes::V3:
	rl.set_interface_id(id);
	rl.set_neighbour_interface_id(0);
	rl.set_neighbour_router_id(id);
	break;
    }
    rlsa->get_router_links().push_back(rl);
}

void
transit(OspfTypes::Version version, RouterLsa *rlsa, OspfTypes::RouterID id,
	uint32_t link_data, uint32_t metric)
{
     RouterLink rl(version);

     rl.set_type(RouterLink::transit);
     rl.set_metric(metric);

    switch(version) {
    case OspfTypes::V2:
	rl.set_link_id(id);
	rl.set_link_data(link_data);
	break;
    case OspfTypes::V3:
	rl.set_interface_id(id);
	rl.set_neighbour_interface_id(0);
	rl.set_neighbour_router_id(id);
	break;
    }
    rlsa->get_router_links().push_back(rl);
}

void
stub(OspfTypes::Version version, RouterLsa *rlsa, OspfTypes::RouterID id,
     uint32_t link_data, uint32_t metric)
{
     RouterLink rl(version);

    switch(version) {
    case OspfTypes::V2:
	break;
    case OspfTypes::V3:
	return;
	break;
    }

     rl.set_type(RouterLink::stub);
     rl.set_metric(metric);

     switch(version) {
     case OspfTypes::V2:
	 rl.set_link_id(id);
	 rl.set_link_data(link_data);
	break;
     case OspfTypes::V3:
	 rl.set_interface_id(id);
	 rl.set_neighbour_interface_id(0);
	 rl.set_neighbour_router_id(id);
	 break;
     }
     rlsa->get_router_links().push_back(rl);
}

Lsa::LsaRef
create_router_lsa(OspfTypes::Version version, uint32_t link_state_id,
		  uint32_t advertising_router)
{
     RouterLsa *rlsa = new RouterLsa(version);
     Lsa_header& header = rlsa->get_header();

     uint32_t options = compute_options(version, OspfTypes::NORMAL);

     // Set the header fields.
     switch(version) {
     case OspfTypes::V2:
	 header.set_options(options);
	 break;
     case OspfTypes::V3:
	 rlsa->set_options(options);
	 break;
     }

     header.set_link_state_id(link_state_id);
     header.set_advertising_router(advertising_router);

     return Lsa::LsaRef(rlsa);
}

Lsa::LsaRef
create_network_lsa(OspfTypes::Version version, uint32_t link_state_id,
		   uint32_t advertising_router, uint32_t mask)
{
    NetworkLsa *nlsa = new NetworkLsa(version);
    Lsa_header& header = nlsa->get_header();

    uint32_t options = compute_options(version, OspfTypes::NORMAL);

    // Set the header fields.
    switch(version) {
    case OspfTypes::V2:
	header.set_options(options);
	nlsa->set_network_mask(mask);
	break;
    case OspfTypes::V3:
	nlsa->set_options(options);
	break;
    }

    header.set_link_state_id(link_state_id);
    header.set_advertising_router(advertising_router);

    return Lsa::LsaRef(nlsa);
}

Lsa::LsaRef
create_external_lsa(OspfTypes::Version version, uint32_t link_state_id,
		    uint32_t advertising_router)
{
    ASExternalLsa *aselsa = new ASExternalLsa(version);
    Lsa_header& header = aselsa->get_header();

    uint32_t options = compute_options(version, OspfTypes::NORMAL);

    // Set the header fields.
    switch(version) {
    case OspfTypes::V2:
	header.set_options(options);
	break;
    case OspfTypes::V3:
	break;
    }

    header.set_link_state_id(link_state_id);
    header.set_advertising_router(advertising_router);

    return Lsa::LsaRef(aselsa);
}

template <typename A> 
bool
verify_routes(TestInfo& info, uint32_t lineno, DebugIO<A>& io, uint32_t routes)
{
    if (routes != io.routing_table_size()) {
	DOUT(info) << "Line number: " << lineno << endl;
	DOUT(info) << "Expecting " << routes << " routes " << "got " <<
	    io.routing_table_size() << endl;
	return false;
    }
    return true;
}

// This is the origin.

Lsa::LsaRef
create_RT6(OspfTypes::Version version)
{
     Lsa::LsaRef lsar = create_router_lsa(version, 6, 6);
     RouterLsa *rlsa = dynamic_cast<RouterLsa *>(lsar.get());
     XLOG_ASSERT(rlsa);

     // Link to RT3
     p2p(version, rlsa, 3, 4, 6);

     // Link to RT5
     p2p(version, rlsa, 5, 6, 6);

     // Link to RT10 XXX need to look at this more carefully.
     p2p(version, rlsa, 10, 11, 7);

     rlsa->encode();

     rlsa->set_self_originating(true);

     return lsar;
}

Lsa::LsaRef
create_RT3(OspfTypes::Version version)
{
     Lsa::LsaRef lsar = create_router_lsa(version, 3, 3);
     RouterLsa *rlsa = dynamic_cast<RouterLsa *>(lsar.get());
     XLOG_ASSERT(rlsa);

     // Link to RT6
     p2p(version, rlsa, 6, 7, 8);

     // Network to N4
     stub(version, rlsa, (4 << 16), 0xffff0000, 2);

     // Network to N3
//      network(version, rlsa, 3, 1);

     rlsa->encode();

     return lsar;
}

// Some of the routers from Figure 2. in RFC 2328. Single area.
// This router is R6.

template <typename A> 
bool
routing1(TestInfo& info, OspfTypes::Version version)
{
    EventLoop eventloop;
    DebugIO<A> io(info, version, eventloop);
    io.startup();
    
    Ospf<A> ospf(version, eventloop, &io);
    ospf.trace().all(info.verbose());
    ospf.set_router_id(set_id("0.0.0.6"));

    OspfTypes::AreaID area = set_id("128.16.64.16");
    const uint16_t interface_prefix_length = 16;
    const uint16_t interface_mtu = 1500;

    PeerManager<A>& pm = ospf.get_peer_manager();

    // Create an area
    pm.create_area_router(area, OspfTypes::NORMAL);

    // Create a peer associated with this area.
    const string interface = "eth0";
    const string vif = "vif0";

    A src;
    switch(src.ip_version()) {
    case 4:
	src = "192.150.187.78";
	break;
    case 6:
	src = "2001:468:e21:c800:220:edff:fe61:f033";
	break;
    default:
	XLOG_FATAL("Unknown IP version %d", src.ip_version());
	break;
    }

    PeerID peerid = pm.
	create_peer(interface, vif, src, interface_prefix_length,
		    interface_mtu,
		    OspfTypes::BROADCAST,
		    area);

    // Bring the peering up
    if (!pm.set_state_peer(peerid, true)) {
	DOUT(info) << "Failed enable peer\n";
	return false;
    }

    AreaRouter<A> *ar = pm.get_area_router(area);
    XLOG_ASSERT(ar);

    ar->testing_replace_router_lsa(create_RT6(version));

    ar->testing_add_lsa(create_RT3(version));

    if (info.verbose())
	ar->testing_print_link_state_database();
    ar->testing_routing_total_recompute();

    ar->testing_delete_lsa(create_RT3(version));

    // At the time of writing the OSPFv3 routing table computations
    // were not complete, when they are remove this test.
    if (OspfTypes::V2 == version) {
	// At this point there should be a single route in the routing
	// table.
	if (!verify_routes(info, __LINE__, io, 1))
	    return false;
	const uint32_t routes = 1;
	if (routes != io.routing_table_size()) {
	    DOUT(info) << "Expecting " << routes << " routes " << "got " <<
		io.routing_table_size() << endl;
	    return false;
	}
	if (!io.routing_table_verify(IPNet<A>("0.4.0.0/16"),
				    A("0.0.0.7"), 8, false, false)) {
	    DOUT(info) << "Mismatch in routing table\n";
	    return false;
	}
    }

    // Now delete the routes.

    if (info.verbose())
	ar->testing_print_link_state_database();
    ar->testing_routing_total_recompute();

    // Take the peering down
    if (!pm.set_state_peer(peerid, false)) {
	DOUT(info) << "Failed to disable peer\n";
	return false;
    }

    // Delete the peer.
    if (!pm.delete_peer(peerid)) {
	DOUT(info) << "Failed to delete peer\n";
	return false;
    }

    // Delete the area
    if (!pm.destroy_area_router(area)) {
	DOUT(info) << "Failed to delete area\n";
	return false;
    }

    // The routing table should be empty now.
    if (0 != io.routing_table_size()) {
	DOUT(info) << "Expecting no routes " << "got " <<
	    io.routing_table_size() << endl;
	return false;
    }

    return true;
}

// Attempting to reproduce:
// http://www.xorp.org/bugzilla/show_bug.cgi?id=226
bool
routing2(TestInfo& info)
{
    OspfTypes::Version version = OspfTypes::V2;

    EventLoop eventloop;
    DebugIO<IPv4> io(info, version, eventloop);
    io.startup();

    Ospf<IPv4> ospf(version, eventloop, &io);
    ospf.trace().all(info.verbose());
    OspfTypes::RouterID rid = set_id("10.0.8.161");
    ospf.set_router_id(rid);
    
    OspfTypes::AreaID area = set_id("0.0.0.0");
    const uint16_t interface_prefix_length = 30;
    const uint16_t interface_mtu = 1500;

    PeerManager<IPv4>& pm = ospf.get_peer_manager();

    // Create an area
    pm.create_area_router(area, OspfTypes::NORMAL);

    // Create a peer associated with this area.
    IPv4 src("172.16.1.1");
    const string interface = "eth0";
    const string vif = "vif0";

    PeerID peerid = pm.
	create_peer(interface, vif, src, interface_prefix_length,
		    interface_mtu,
		    OspfTypes::BROADCAST,
		    area);

    // Bring the peering up
    if (!pm.set_state_peer(peerid, true)) {
	DOUT(info) << "Failed enable peer\n";
	return false;
    }

    AreaRouter<IPv4> *ar = pm.get_area_router(area);
    XLOG_ASSERT(ar);

    // Create this router's Router-LSA
    Lsa::LsaRef lsar;
    lsar = create_router_lsa(version, rid, rid);
    RouterLsa *rlsa;
    rlsa = dynamic_cast<RouterLsa *>(lsar.get());
    XLOG_ASSERT(rlsa);
    transit(version, rlsa, set_id("172.16.1.2"), set_id("172.16.1.1"), 1);
    lsar->encode();
    lsar->set_self_originating(true);
    ar->testing_replace_router_lsa(lsar);
    
    // Create the peer's Router-LSA
    OspfTypes::RouterID prid = set_id("172.16.1.2");
    lsar = create_router_lsa(version, prid, prid);
    rlsa = dynamic_cast<RouterLsa *>(lsar.get());
    XLOG_ASSERT(rlsa);
    transit(version, rlsa, set_id("172.16.1.2"), set_id("172.16.1.2"), 1);
    stub(version, rlsa, set_id("172.16.2.1"), 0xffffffff, 1);
    stub(version, rlsa, set_id("172.16.1.100"), 0xffffffff, 1);
    lsar->encode();
    ar->testing_add_lsa(lsar);

    // Create the Network-LSA that acts as the binding glue.
    lsar = create_network_lsa(version, prid, prid, 0xfffffffc);
    NetworkLsa *nlsa;
    nlsa = dynamic_cast<NetworkLsa *>(lsar.get());
    XLOG_ASSERT(nlsa);
    nlsa->get_attached_routers().push_back(prid);
    nlsa->get_attached_routers().push_back(rid);
    lsar->encode();
    ar->testing_add_lsa(lsar);

    /*********************************************************/
    if (info.verbose())
	ar->testing_print_link_state_database();
    ar->testing_routing_total_recompute();

    if (!verify_routes(info, __LINE__, io, 2))
	return false;

    if (!io.routing_table_verify(IPNet<IPv4>("172.16.1.100/32"),
				 IPv4("172.16.1.2"), 2, false, false)) {
	DOUT(info) << "Mismatch in routing table\n";
	return false;
    }

    if (!io.routing_table_verify(IPNet<IPv4>("172.16.2.1/32"),
				 IPv4("172.16.1.2"), 2, false, false)) {
	DOUT(info) << "Mismatch in routing table\n";
	return false;
    }

    /*********************************************************/
    ar->testing_delete_lsa(lsar);
    if (info.verbose())
	ar->testing_print_link_state_database();
    ar->testing_routing_total_recompute();

    if (!verify_routes(info, __LINE__, io, 0))
	return false;

    /*********************************************************/
    // Create the Network-LSA again as the first one has been invalidated.
    lsar = create_network_lsa(version, prid, prid, 0xfffffffc);
    nlsa = dynamic_cast<NetworkLsa *>(lsar.get());
    XLOG_ASSERT(nlsa);
    nlsa->get_attached_routers().push_back(prid);
    nlsa->get_attached_routers().push_back(rid);
    lsar->encode();
    ar->testing_add_lsa(lsar);
    if (info.verbose())
	ar->testing_print_link_state_database();
    ar->testing_routing_total_recompute();

    if (!verify_routes(info, __LINE__, io, 2))
	return false;

    if (!io.routing_table_verify(IPNet<IPv4>("172.16.1.100/32"),
				 IPv4("172.16.1.2"), 2, false, false)) {
	DOUT(info) << "Mismatch in routing table\n";
	return false;
    }

    if (!io.routing_table_verify(IPNet<IPv4>("172.16.2.1/32"),
				 IPv4("172.16.1.2"), 2, false, false)) {
	DOUT(info) << "Mismatch in routing table\n";
	return false;
    }
    /*********************************************************/
    ar->testing_delete_lsa(lsar);
    if (info.verbose())
	ar->testing_print_link_state_database();
    ar->testing_routing_total_recompute();

    if (!verify_routes(info, __LINE__, io, 0))
	return false;

    /*********************************************************/
    // Take the peering down
    if (!pm.set_state_peer(peerid, false)) {
	DOUT(info) << "Failed to disable peer\n";
	return false;
    }

    // Delete the peer.
    if (!pm.delete_peer(peerid)) {
	DOUT(info) << "Failed to delete peer\n";
	return false;
    }

    // Delete the area
    if (!pm.destroy_area_router(area)) {
	DOUT(info) << "Failed to delete area\n";
	return false;
    }

    // The routing table should be empty now.
    if (!verify_routes(info, __LINE__, io, 0))
	return false;

    return true;
}

inline
bool
type_check(TestInfo& info, string message, uint32_t expected, uint32_t actual)
{
    if (expected != actual) {
	DOUT(info) << message << " should be " << expected << " is " << 
	    actual << endl;
	return false;
    }

    return true;
}

/**
 * Read in LSA database files written by print_lsas.cc and run the
 * routing computation.
 */
template <typename A> 
bool
routing3(TestInfo& info, OspfTypes::Version version, string fname)
{
    if (0 == fname.size()) {
	DOUT(info) << "No filename supplied\n";

	return true;
    }

    EventLoop eventloop;
    DebugIO<A> io(info, version, eventloop);
    io.startup();
    
    Ospf<A> ospf(version, eventloop, &io);
    ospf.trace().all(info.verbose());
    Tlv tlv;

    if (!tlv.open(fname, true /* read */)) {
	DOUT(info) << "Failed to open " << fname << endl;
	return false;
    }

    vector<uint8_t> data;
    uint32_t type;

    // Read the version field and check it.
    if (!tlv.read(type, data)) {
	DOUT(info) << "Failed to read " << fname << endl;
	return false;
    }

    if (!type_check(info, "TLV Version", TLV_VERSION, type))
	return false;

    uint32_t tlv_version;
    if (!tlv.get32(data, 0, tlv_version)) {
	return false;
    }

    if (!type_check(info, "Version", TLV_CURRENT_VERSION, tlv_version))
	return false;

    // Read the system info
    if (!tlv.read(type, data)) {
	DOUT(info) << "Failed to read " << fname << endl;
	return false;
    }

    if (!type_check(info, "TLV System info", TLV_SYSTEM_INFO, type))
	return false;

    data.resize(data.size() + 1);
    data[data.size() - 1] = 0;
    
    DOUT(info) << "Built on " << &data[0] << endl;

    // Get OSPF version
    if (!tlv.read(type, data)) {
	DOUT(info) << "Failed to read " << fname << endl;
	return false;
    }
    
    if (!type_check(info, "TLV OSPF Version", TLV_OSPF_VERSION, type))
	return false;

    uint32_t ospf_version;
    if (!tlv.get32(data, 0, ospf_version)) {
	return false;
    }

    if (!type_check(info, "OSPF Version", version, ospf_version))
	return false;

    // OSPF area
    if (!tlv.read(type, data)) {
	DOUT(info) << "Failed to read " << fname << endl;
	return false;
    }
    
    if (!type_check(info, "TLV OSPF Area", TLV_AREA, type))
	return false;
    
    OspfTypes::AreaID area;
    if (!tlv.get32(data, 0, area)) {
	return false;
    }

    PeerManager<A>& pm = ospf.get_peer_manager();
    pm.create_area_router(area, OspfTypes::NORMAL);
    AreaRouter<A> *ar = pm.get_area_router(area);
    XLOG_ASSERT(ar);

    // The first LSA is this routers Router-LSA.
    if (!tlv.read(type, data)) {
	DOUT(info) << "Failed to read " << fname << endl;
	return false;
    }
    
    if (!type_check(info, "TLV LSA", TLV_LSA, type))
	return false;

    LsaDecoder lsa_decoder(version);
    initialise_lsa_decoder(version, lsa_decoder);

    size_t len = data.size();
    Lsa::LsaRef lsar = lsa_decoder.decode(&data[0], len);

    DOUT(info) << lsar->str() << endl;

    ospf.set_router_id(lsar->get_header().get_link_state_id());

    lsar->set_self_originating(true);
    ar->testing_replace_router_lsa(lsar);

    // Keep reading LSAs until we run out or hit a new area.
    for(;;) {
	if (!tlv.read(type, data))
	    break;
	if (TLV_LSA != type)
	    break;
	size_t len = data.size();
	Lsa::LsaRef lsar = lsa_decoder.decode(&data[0], len);
	ar->testing_add_lsa(lsar);
    }

    if (info.verbose())
	ar->testing_print_link_state_database();
    ar->testing_routing_total_recompute();

    return true;
}

/**
 * This is a similar problem to:
 *	http://www.xorp.org/bugzilla/show_bug.cgi?id=295
 *
 * Two Router-LSAs an a Network-LSA, the router under consideration
 * generated the Network-LSA. The other Router-LSA has the e and b
 * bits set so it must be installed in the routing table to help with
 * the routing computation. An AS-External-LSA generated by the other router.
 */
bool
routing4(TestInfo& info)
{
    OspfTypes::Version version = OspfTypes::V2;

    EventLoop eventloop;
    DebugIO<IPv4> io(info, version, eventloop);
    io.startup();

    Ospf<IPv4> ospf(version, eventloop, &io);
    ospf.trace().all(info.verbose());
    OspfTypes::AreaID area = set_id("0.0.0.0");

    PeerManager<IPv4>& pm = ospf.get_peer_manager();
    pm.create_area_router(area, OspfTypes::NORMAL);
    AreaRouter<IPv4> *ar = pm.get_area_router(area);
    XLOG_ASSERT(ar);

    OspfTypes::RouterID rid = set_id("10.0.1.1");
    ospf.set_router_id(rid);

    // Create this router's Router-LSA
    Lsa::LsaRef lsar;
    lsar = create_router_lsa(version, rid, rid);
    RouterLsa *rlsa;
    rlsa = dynamic_cast<RouterLsa *>(lsar.get());
    XLOG_ASSERT(rlsa);
    transit(version, rlsa, rid, rid, 1 /* metric */);
    lsar->encode();
    lsar->set_self_originating(true);
    ar->testing_replace_router_lsa(lsar);

    // Create the peer's Router-LSA
    OspfTypes::RouterID prid = set_id("10.0.1.6");
    lsar = create_router_lsa(version, prid, prid);
    rlsa = dynamic_cast<RouterLsa *>(lsar.get());
    XLOG_ASSERT(rlsa);
    transit(version, rlsa, rid, prid, 1);
    rlsa->set_e_bit(true);
    rlsa->set_b_bit(true);
    lsar->encode();
    ar->testing_add_lsa(lsar);

    // Create the Network-LSA that acts as the binding glue.
    lsar = create_network_lsa(version, rid, rid, 0xffff0000);
    NetworkLsa *nlsa;
    nlsa = dynamic_cast<NetworkLsa *>(lsar.get());
    XLOG_ASSERT(nlsa);
    nlsa->get_attached_routers().push_back(rid);
    nlsa->get_attached_routers().push_back(prid);
    lsar->encode();
    ar->testing_add_lsa(lsar);

    // Create the AS-External-LSA from the peer.
    lsar = create_external_lsa(version, set_id("10.20.0.0"), prid);
    ASExternalLsa *aselsa;
    aselsa = dynamic_cast<ASExternalLsa *>(lsar.get());
    XLOG_ASSERT(aselsa);
    aselsa->set_network_mask(0xffff0000);
    aselsa->set_metric(1);
    aselsa->set_forwarding_address_ipv4(IPv4("10.0.1.6"));
    lsar->encode();
    ar->testing_add_lsa(lsar);

    /*********************************************************/    

    if (info.verbose())
	ar->testing_print_link_state_database();
    ar->testing_routing_total_recompute();

    if (!verify_routes(info, __LINE__, io, 1))
	return false;

#if	0
    // This route made it to the routing table but not to the RIB
    if (!io.routing_table_verify(IPNet<IPv4>("10.0.1.6/32"),
				 IPv4("10.0.1.6"), 1, false, false)) {
	DOUT(info) << "Mismatch in routing table\n";
	return false;
    }
#endif

    if (!io.routing_table_verify(IPNet<IPv4>("10.20.0.0/16"),
				 IPv4("10.0.1.6"), 2, false, false)) {
	DOUT(info) << "Mismatch in routing table\n";
	return false;
    }
    
    return true;
}

int
main(int argc, char **argv)
{
    XorpUnexpectedHandler x(xorp_unexpected_handler);

    TestMain t(argc, argv);

    string test =
	t.get_optional_args("-t", "--test", "run only the specified test");
    string fname =t.get_optional_args("-f", "--filename", "lsa database");
    t.complete_args_parsing();

    struct test {
	string test_name;
	XorpCallback1<bool, TestInfo&>::RefPtr cb;
    } tests[] = {
	{"r1V2", callback(routing1<IPv4>, OspfTypes::V2)},
	{"r1V3", callback(routing1<IPv6>, OspfTypes::V3)},
	{"r2", callback(routing2)},
	{"r3V2", callback(routing3<IPv4>, OspfTypes::V2, fname)},
	{"r3V3", callback(routing3<IPv6>, OspfTypes::V3, fname)},
	{"r4", callback(routing4)},
    };

    try {
	if (test.empty()) {
	    for (size_t i = 0; i < sizeof(tests) / sizeof(struct test); i++)
		t.run(tests[i].test_name, tests[i].cb);
	} else {
	    for (size_t i = 0; i < sizeof(tests) / sizeof(struct test); i++)
		if (test == tests[i].test_name) {
		    t.run(tests[i].test_name, tests[i].cb);
		    return t.exit();
		}
	    t.failed("No test with name " + test + " found\n");
	}
    } catch(...) {
	xorp_catch_standard_exceptions();
    }

    return t.exit();
}
