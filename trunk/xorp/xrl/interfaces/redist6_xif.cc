/*
 * Copyright (c) 2001-2003 International Computer Science Institute
 * See LICENSE file for licensing, conditions, and warranties on use.
 *
 * DO NOT EDIT THIS FILE - IT IS PROGRAMMATICALLY GENERATED
 *
 * Generated by 'clnt-gen'.
 */

#ident "$XORP$"

#include "redist6_xif.hh"

bool
XrlRedist6V0p1Client::send_add_route6(
	const char*	the_tgt,
	const IPv6Net&	network,
	const IPv6&	nexthop,
	const uint32_t&	global_metric,
	const string&	cookie,
	const AddRoute6CB&	cb
)
{
    Xrl x(the_tgt, "redist6/0.1/add_route6");
    x.args().add("network", network);
    x.args().add("nexthop", nexthop);
    x.args().add("global_metric", global_metric);
    x.args().add("cookie", cookie);
    return _sender->send(x, callback(this, &XrlRedist6V0p1Client::unmarshall_add_route6, cb));
}


/* Unmarshall add_route6 */
void
XrlRedist6V0p1Client::unmarshall_add_route6(
	const XrlError&	e,
	XrlArgs*	a,
	AddRoute6CB		cb
)
{
    if (e != XrlError::OKAY()) {
	cb->dispatch(e);
	return;
    } else if (a && a->size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != 0)", (uint32_t)a->size());
	cb->dispatch(XrlError::BAD_ARGS());
	return;
    }
    cb->dispatch(e);
}

bool
XrlRedist6V0p1Client::send_delete_route6(
	const char*	the_tgt,
	const IPv6Net&	network,
	const string&	cookie,
	const DeleteRoute6CB&	cb
)
{
    Xrl x(the_tgt, "redist6/0.1/delete_route6");
    x.args().add("network", network);
    x.args().add("cookie", cookie);
    return _sender->send(x, callback(this, &XrlRedist6V0p1Client::unmarshall_delete_route6, cb));
}


/* Unmarshall delete_route6 */
void
XrlRedist6V0p1Client::unmarshall_delete_route6(
	const XrlError&	e,
	XrlArgs*	a,
	DeleteRoute6CB		cb
)
{
    if (e != XrlError::OKAY()) {
	cb->dispatch(e);
	return;
    } else if (a && a->size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != 0)", (uint32_t)a->size());
	cb->dispatch(XrlError::BAD_ARGS());
	return;
    }
    cb->dispatch(e);
}

bool
XrlRedist6V0p1Client::send_delete_all_routes6(
	const char*	the_tgt,
	const string&	cookie,
	const DeleteAllRoutes6CB&	cb
)
{
    Xrl x(the_tgt, "redist6/0.1/delete_all_routes6");
    x.args().add("cookie", cookie);
    return _sender->send(x, callback(this, &XrlRedist6V0p1Client::unmarshall_delete_all_routes6, cb));
}


/* Unmarshall delete_all_routes6 */
void
XrlRedist6V0p1Client::unmarshall_delete_all_routes6(
	const XrlError&	e,
	XrlArgs*	a,
	DeleteAllRoutes6CB		cb
)
{
    if (e != XrlError::OKAY()) {
	cb->dispatch(e);
	return;
    } else if (a && a->size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != 0)", (uint32_t)a->size());
	cb->dispatch(XrlError::BAD_ARGS());
	return;
    }
    cb->dispatch(e);
}
