// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-
// vim:set sts=4 ts=8:

// Copyright (c) 2001-2005 International Computer Science Institute
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

// $XORP: xorp/devnotes/template.hh,v 1.5 2005/03/25 02:52:59 pavlin Exp $

#ifndef __OSPF_TRACE_HH__
#define __OSPF_TRACE_HH__

/**
 * All the trace variables in one place.
 */
struct Trace {
    // Enable all the error tracing be default.
    Trace() : _input_errors(true)
	{}
    bool _input_errors;
};

#endif // __OSPF_TRACE_HH__
