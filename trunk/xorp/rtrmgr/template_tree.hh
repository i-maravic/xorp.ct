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

// $XORP: xorp/rtrmgr/template_tree.hh,v 1.5 2003/08/01 23:07:29 pavlin Exp $

#ifndef __RTRMGR_TEMPLATE_TREE_HH__
#define __RTRMGR_TEMPLATE_TREE_HH__

#include <map>
#include <list>
#include <set>
#include "config.h"
#include "libxorp/xorp.h"
#include "libxorp/ipvxnet.hh"
#include "libxorp/mac.hh"
#include "module_manager.hh"
#include "xorp_client.hh"
#include "xrldb.hh"
#include "path_segment.hh"

class TemplateTreeNode;
class ModuleCommand;

class TemplateTree {
public:
    TemplateTree(const string& xorp_root_dir,
		 const string& config_template_dir,
		 const string& xrl_dir);
    ~TemplateTree();
    void extend_path(string segment, bool is_tag);
    void pop_path();
    void push_path(int type, char *initializer);
    void add_untyped_node(string segment, bool is_tag);
    void add_node(const string &segment, int type, char *initializer);
    TemplateTreeNode *find_node(const list<string>& path_segments);
    string path_as_string();
    void add_cmd(char *cmd);
    void add_cmd_action(string cmd, const list<string> &action);
    void display_tree();
    void register_module(const string &name, ModuleCommand *mc);
    ModuleCommand *find_module(const string &name);
    bool check_variable_name(const string& s) const;
    const XRLdb& xrldb() const {return _xrldb;}
    TemplateTreeNode *root() const {return _root;}
    const string& xorp_root_dir() const { return _xorp_root_dir; }

private:
    TemplateTreeNode *new_node(TemplateTreeNode *parent,
			       const string &path, 
			       const string &varname,
			       int type, 
			       const string &initializer);
    TemplateTreeNode	*_root;
    TemplateTreeNode	*_current_node;
    map <string, ModuleCommand *> _registered_modules;
    list<PathSegment>	_path_segments;
    list<int>		_segment_lengths;
    XRLdb		_xrldb;
    ModuleManager	*mmgr;
    string		_xorp_root_dir;	// The root of the XORP tree
};

#endif // __RTRMGR_TEMPLATE_TREE_HH__
