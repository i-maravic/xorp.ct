/*
 * Copyright (c) 2001-2004 International Computer Science Institute
 * See LICENSE file for licensing, conditions, and warranties on use.
 *
 * DO NOT EDIT THIS FILE - IT IS PROGRAMMATICALLY GENERATED
 *
 * Generated by 'tgt-gen'.
 */

#ident "$XORP: xorp/xrl/targets/profiler_base.cc,v 1.1 2004/09/21 18:09:13 atanu Exp $"


#include "profiler_base.hh"


XrlProfilerTargetBase::XrlProfilerTargetBase(XrlCmdMap* cmds)
    : _cmds(cmds)
{
    if (_cmds)
	add_handlers();
}

XrlProfilerTargetBase::~XrlProfilerTargetBase()
{
    if (_cmds)
	remove_handlers();
}

bool
XrlProfilerTargetBase::set_command_map(XrlCmdMap* cmds)
{
    if (_cmds == 0 && cmds) {
        _cmds = cmds;
        add_handlers();
        return true;
    }
    if (_cmds && cmds == 0) {
	remove_handlers();
        _cmds = cmds;
        return true;
    }
    return false;
}

const XrlCmdError
XrlProfilerTargetBase::handle_common_0_1_get_target_name(const XrlArgs& xa_inputs, XrlArgs* pxa_outputs)
{
    if (xa_inputs.size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != %u) handling %s",
            0, XORP_UINT_CAST(xa_inputs.size()), "common/0.1/get_target_name");
	return XrlCmdError::BAD_ARGS();
    }

    if (pxa_outputs == 0) {
	XLOG_FATAL("Return list empty");
	return XrlCmdError::BAD_ARGS();
    }

    /* Return value declarations */
    string name;
    try {
	XrlCmdError e = common_0_1_get_target_name(
	    name);
	if (e != XrlCmdError::OKAY()) {
	    XLOG_WARNING("Handling method for %s failed: %s",
            		 "common/0.1/get_target_name", e.str().c_str());
	    return e;
        }
    } catch (const XrlArgs::XrlAtomNotFound& e) {
	XLOG_ERROR("Argument not found");
	return XrlCmdError::BAD_ARGS();
    }

    /* Marshall return values */
    try {
	pxa_outputs->add("name", name);
    } catch (const XrlArgs::XrlAtomFound& ) {
	XLOG_FATAL("Duplicate atom name"); /* XXX Should never happen */
    }
    return XrlCmdError::OKAY();
}

const XrlCmdError
XrlProfilerTargetBase::handle_common_0_1_get_version(const XrlArgs& xa_inputs, XrlArgs* pxa_outputs)
{
    if (xa_inputs.size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != %u) handling %s",
            0, XORP_UINT_CAST(xa_inputs.size()), "common/0.1/get_version");
	return XrlCmdError::BAD_ARGS();
    }

    if (pxa_outputs == 0) {
	XLOG_FATAL("Return list empty");
	return XrlCmdError::BAD_ARGS();
    }

    /* Return value declarations */
    string version;
    try {
	XrlCmdError e = common_0_1_get_version(
	    version);
	if (e != XrlCmdError::OKAY()) {
	    XLOG_WARNING("Handling method for %s failed: %s",
            		 "common/0.1/get_version", e.str().c_str());
	    return e;
        }
    } catch (const XrlArgs::XrlAtomNotFound& e) {
	XLOG_ERROR("Argument not found");
	return XrlCmdError::BAD_ARGS();
    }

    /* Marshall return values */
    try {
	pxa_outputs->add("version", version);
    } catch (const XrlArgs::XrlAtomFound& ) {
	XLOG_FATAL("Duplicate atom name"); /* XXX Should never happen */
    }
    return XrlCmdError::OKAY();
}

const XrlCmdError
XrlProfilerTargetBase::handle_common_0_1_get_status(const XrlArgs& xa_inputs, XrlArgs* pxa_outputs)
{
    if (xa_inputs.size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != %u) handling %s",
            0, XORP_UINT_CAST(xa_inputs.size()), "common/0.1/get_status");
	return XrlCmdError::BAD_ARGS();
    }

    if (pxa_outputs == 0) {
	XLOG_FATAL("Return list empty");
	return XrlCmdError::BAD_ARGS();
    }

    /* Return value declarations */
    uint32_t status;
    string reason;
    try {
	XrlCmdError e = common_0_1_get_status(
	    status,
	    reason);
	if (e != XrlCmdError::OKAY()) {
	    XLOG_WARNING("Handling method for %s failed: %s",
            		 "common/0.1/get_status", e.str().c_str());
	    return e;
        }
    } catch (const XrlArgs::XrlAtomNotFound& e) {
	XLOG_ERROR("Argument not found");
	return XrlCmdError::BAD_ARGS();
    }

    /* Marshall return values */
    try {
	pxa_outputs->add("status", status);
	pxa_outputs->add("reason", reason);
    } catch (const XrlArgs::XrlAtomFound& ) {
	XLOG_FATAL("Duplicate atom name"); /* XXX Should never happen */
    }
    return XrlCmdError::OKAY();
}

const XrlCmdError
XrlProfilerTargetBase::handle_common_0_1_shutdown(const XrlArgs& xa_inputs, XrlArgs* /* pxa_outputs */)
{
    if (xa_inputs.size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != %u) handling %s",
            0, XORP_UINT_CAST(xa_inputs.size()), "common/0.1/shutdown");
	return XrlCmdError::BAD_ARGS();
    }

    /* Return value declarations */
    try {
	XrlCmdError e = common_0_1_shutdown();
	if (e != XrlCmdError::OKAY()) {
	    XLOG_WARNING("Handling method for %s failed: %s",
            		 "common/0.1/shutdown", e.str().c_str());
	    return e;
        }
    } catch (const XrlArgs::XrlAtomNotFound& e) {
	XLOG_ERROR("Argument not found");
	return XrlCmdError::BAD_ARGS();
    }
    return XrlCmdError::OKAY();
}

const XrlCmdError
XrlProfilerTargetBase::handle_profile_client_0_1_log(const XrlArgs& xa_inputs, XrlArgs* /* pxa_outputs */)
{
    if (xa_inputs.size() != 4) {
	XLOG_ERROR("Wrong number of arguments (%u != %u) handling %s",
            4, XORP_UINT_CAST(xa_inputs.size()), "profile_client/0.1/log");
	return XrlCmdError::BAD_ARGS();
    }

    /* Return value declarations */
    try {
	XrlCmdError e = profile_client_0_1_log(
	    xa_inputs.get_string("pname"),
	    xa_inputs.get_uint32("sec"),
	    xa_inputs.get_uint32("usec"),
	    xa_inputs.get_string("comment"));
	if (e != XrlCmdError::OKAY()) {
	    XLOG_WARNING("Handling method for %s failed: %s",
            		 "profile_client/0.1/log", e.str().c_str());
	    return e;
        }
    } catch (const XrlArgs::XrlAtomNotFound& e) {
	XLOG_ERROR("Argument not found");
	return XrlCmdError::BAD_ARGS();
    }
    return XrlCmdError::OKAY();
}

const XrlCmdError
XrlProfilerTargetBase::handle_profile_client_0_1_finished(const XrlArgs& xa_inputs, XrlArgs* /* pxa_outputs */)
{
    if (xa_inputs.size() != 1) {
	XLOG_ERROR("Wrong number of arguments (%u != %u) handling %s",
            1, XORP_UINT_CAST(xa_inputs.size()), "profile_client/0.1/finished");
	return XrlCmdError::BAD_ARGS();
    }

    /* Return value declarations */
    try {
	XrlCmdError e = profile_client_0_1_finished(
	    xa_inputs.get_string("pname"));
	if (e != XrlCmdError::OKAY()) {
	    XLOG_WARNING("Handling method for %s failed: %s",
            		 "profile_client/0.1/finished", e.str().c_str());
	    return e;
        }
    } catch (const XrlArgs::XrlAtomNotFound& e) {
	XLOG_ERROR("Argument not found");
	return XrlCmdError::BAD_ARGS();
    }
    return XrlCmdError::OKAY();
}

void
XrlProfilerTargetBase::add_handlers()
{
	if (_cmds->add_handler("common/0.1/get_target_name",
	    callback(this, &XrlProfilerTargetBase::handle_common_0_1_get_target_name)) == false) {
	    XLOG_ERROR("Failed to xrl handler finder://%s/%s", "profiler", "common/0.1/get_target_name");
	}
	if (_cmds->add_handler("common/0.1/get_version",
	    callback(this, &XrlProfilerTargetBase::handle_common_0_1_get_version)) == false) {
	    XLOG_ERROR("Failed to xrl handler finder://%s/%s", "profiler", "common/0.1/get_version");
	}
	if (_cmds->add_handler("common/0.1/get_status",
	    callback(this, &XrlProfilerTargetBase::handle_common_0_1_get_status)) == false) {
	    XLOG_ERROR("Failed to xrl handler finder://%s/%s", "profiler", "common/0.1/get_status");
	}
	if (_cmds->add_handler("common/0.1/shutdown",
	    callback(this, &XrlProfilerTargetBase::handle_common_0_1_shutdown)) == false) {
	    XLOG_ERROR("Failed to xrl handler finder://%s/%s", "profiler", "common/0.1/shutdown");
	}
	if (_cmds->add_handler("profile_client/0.1/log",
	    callback(this, &XrlProfilerTargetBase::handle_profile_client_0_1_log)) == false) {
	    XLOG_ERROR("Failed to xrl handler finder://%s/%s", "profiler", "profile_client/0.1/log");
	}
	if (_cmds->add_handler("profile_client/0.1/finished",
	    callback(this, &XrlProfilerTargetBase::handle_profile_client_0_1_finished)) == false) {
	    XLOG_ERROR("Failed to xrl handler finder://%s/%s", "profiler", "profile_client/0.1/finished");
	}
	_cmds->finalize();
}

void
XrlProfilerTargetBase::remove_handlers()
{
	_cmds->remove_handler("common/0.1/get_target_name");
	_cmds->remove_handler("common/0.1/get_version");
	_cmds->remove_handler("common/0.1/get_status");
	_cmds->remove_handler("common/0.1/shutdown");
	_cmds->remove_handler("profile_client/0.1/log");
	_cmds->remove_handler("profile_client/0.1/finished");
}
