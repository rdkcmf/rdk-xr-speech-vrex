##########################################################################
# If not stated otherwise in this file or this component's LICENSE
# file the following copyright and licenses apply:
#
# Copyright 2019 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################
include_HEADERS = xrsv.h xrsv_http/xrsv_http.h xrsv_ws/xrsv_ws.h xrsv_ws_nextgen/xrsv_ws_nextgen.h
lib_LTLIBRARIES = libxrsv.la

AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(srcdir)/xrsv_http -I$(srcdir)/xrsv_ws -I$(srcdir)/xrsv_ws_nextgen

libxrsv_la_SOURCES = xrsv_version.h                                 \
                     xrsv.c                                         \
                     xrsv_utils.c                                   \
                     xrsv_http/xrsv_http.c                          \
                     xrsv_ws/xrsv_ws.c                              \
                     xrsv_ws/xrsv_ws_request.c                      \
                     xrsv_ws/xrsv_ws_action.c                       \
                     xrsv_ws/xrsv_ws_action_phone.c                 \
                     xrsv_ws_nextgen/xrsv_ws_nextgen.c              \
                     xrsv_ws_nextgen/xrsv_ws_nextgen_msgtype.c      \
                     xrsv_ws_nextgen/xrsv_ws_nextgen_tv_control.c  

BUILT_SOURCES = xrsv_version.h                                      \
                xrsv_ws/xrsv_ws_request.c                           \
                xrsv_ws/xrsv_ws_action.c                            \
                xrsv_ws/xrsv_ws_action_phone.c                      \
                xrsv_ws_nextgen/xrsv_ws_nextgen_msgtype.c           \
                xrsv_ws_nextgen/xrsv_ws_nextgen_tv_control.c

CLEANFILES    = xrsv_version.h                                      \
                xrsv_ws/xrsv_ws_request.c                           \
                xrsv_ws/xrsv_ws_action.c                            \
                xrsv_ws/xrsv_ws_action_phone.c                      \
                xrsv_ws_nextgen/xrsv_ws_nextgen_msgtype.c           \
                xrsv_ws_nextgen/xrsv_ws_nextgen_tv_control.c

xrsv_ws/xrsv_ws_request.c: xrsv_ws/xrsv_ws_request.hash
	${STAGING_BINDIR_NATIVE}/gperf --output-file=$(srcdir)/xrsv_ws/xrsv_ws_request.c $(srcdir)/xrsv_ws/xrsv_ws_request.hash
xrsv_ws/xrsv_ws_action.c: xrsv_ws/xrsv_ws_action.hash
	${STAGING_BINDIR_NATIVE}/gperf --output-file=$(srcdir)/xrsv_ws/xrsv_ws_action.c $(srcdir)/xrsv_ws/xrsv_ws_action.hash
xrsv_ws/xrsv_ws_action_phone.c: xrsv_ws/xrsv_ws_action_phone.hash
	${STAGING_BINDIR_NATIVE}/gperf --output-file=$(srcdir)/xrsv_ws/xrsv_ws_action_phone.c $(srcdir)/xrsv_ws/xrsv_ws_action_phone.hash
xrsv_ws_nextgen/xrsv_ws_nextgen_msgtype.c: xrsv_ws_nextgen/xrsv_ws_nextgen_msgtype.hash
	${STAGING_BINDIR_NATIVE}/gperf --output-file=$(srcdir)/xrsv_ws_nextgen/xrsv_ws_nextgen_msgtype.c $(srcdir)/xrsv_ws_nextgen/xrsv_ws_nextgen_msgtype.hash
xrsv_ws_nextgen/xrsv_ws_nextgen_tv_control.c: xrsv_ws_nextgen/xrsv_ws_nextgen_tv_control.hash
	${STAGING_BINDIR_NATIVE}/gperf --output-file=$(srcdir)/xrsv_ws_nextgen/xrsv_ws_nextgen_tv_control.c $(srcdir)/xrsv_ws_nextgen/xrsv_ws_nextgen_tv_control.hash

# Create perfect hash .c file from .hash files
#.hash.c:
#	${STAGING_BINDIR_NATIVE}/gperf --output-file=$@ $<

xrsv_version.h:
	echo "#define XRSV_VER_MAJOR \"1\"" >$@
	echo "#define XRSV_VER_MINOR \"`git -C ${srcdir} rev-list --count 4b825dc642cb6eb9a060e54bf8d69288fbee4904..HEAD``git -C ${srcdir} diff --quiet || echo \"++\"`\"" >>$@
	echo "#define XRSV_BRANCH    \"${GIT_BRANCH}\"" >>$@
	echo "#define XRSV_COMMIT_ID \"`git -C ${srcdir} log --format="%H" -n 1`\"" >>$@
	echo "#define XRSV_VERSION (XRSV_VER_MAJOR \".\" XRSV_VER_MINOR)" >>$@
