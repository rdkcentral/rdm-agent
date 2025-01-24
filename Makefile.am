# Copyright 2023 Comcast Cable Communications Management, LLC
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
#
# SPDX-License-Identifier: Apache-2.0
#


AUTOMAKE_OPTIONS = foreign subdir-objects

rdm_CFLAGS += -fPIC -pthread 

bin_PROGRAMS = rdm

rdm_SOURCES = rdm_main.c                          \
              src/rdm_jsonquery.c                 \
              src/rdm_utils.c                     \
              src/rdm_download.c                  \
              src/rdm_downloadverapp.c            \
              src/rdm_downloadmgr.c               \
              src/rdm_usbinstall.c                \
              src/rdm_curldownload.c              \
              src/rdm_rbus.c                      \
              $(NULL)

if IS_LIBRDKCONFIG_ENABLED
rdm_CFLAGS += $(LIBRDKCONFIG_FLAG)
rdm_LDFLAGS += -lrdkconfig -lsecure_wrapper
endif

rdm_LDFLAGS += -shared -fPIC $(GLIB_LIBS) \
               -lcjson -lrbus -lssl -lcrypto -lcurl -lparsejson -ldwnlutil -lfwutils -lopkg -lrfcapi

rdm_CFLAGS +=  -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/dbus-1.0 \
               -I${PKG_CONFIG_SYSROOT_DIR}$(libdir)/dbus-1.0/include \
               -I${top_srcdir}/include \
               -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/rbus \
               -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/rdk/iarmbus \
               -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/rdk/iarmmgrs/sysmgr \
               -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/rdk/iarmmgrs/rdmmgr \
               -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/rdk/iarmmgrs-hal

if USE_CPC
  rdm_SOURCES += src/rdm-cpc/rdm/codebigUtils.c \
		 src/rdm-cpc/rdm/rdm_openssl.c  \
		 src/rdm-cpc/rdm/rdm_packagemgr.c \
		 src/rdm-cpc/rdm/rdm_downloadutils.c
  rdm_CFLAGS += -DRDM_ENABLE_CODEBIG \
              -I${top_srcdir}/src/rdm-cpc/rdm
  rdm_LDFLAGS += -lsecure_wrapper
else
  rdm_SOURCES += src/codebig/codebigUtils.c \
		 src/rdm_openssl.c \
		 src/rdm_downloadutils.c
  rdm_CFLAGS += -I${top_srcdir}/src/codebig
endif
