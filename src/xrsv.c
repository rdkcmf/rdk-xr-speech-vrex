/*
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
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <xrsv.h>
#include <xrsv_version.h>

void xrsv_version(const char **name, const char **version, const char **branch, const char **commit_id) {
   if(name != NULL) {
      *name = "xr-speech-vrex";
   }
   if(version != NULL) {
      *version = XRSV_VERSION;
   }
   if(branch != NULL) {
      *branch = XRSV_BRANCH;
   }
   if(commit_id != NULL) {
      *commit_id = XRSV_COMMIT_ID;
   }
}
