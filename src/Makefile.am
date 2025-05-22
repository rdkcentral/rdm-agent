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

rdm_SOURCES = ../rdm_main.c                          \
              rdm_jsonquery.c                 \
              rdm_utils.c                     \
              rdm_download.c                  \
              rdm_downloadverapp.c            \
              rdm_downloadmgr.c               \
              rdm_usbinstall.c                \
              rdm_curldownload.c              \
	            rdm_packagemgr.c                \
              rdm_rbus.c                      \
              $(NULL)

if IS_LIBRDKCONFIG_ENABLED
rdm_CFLAGS += $(LIBRDKCONFIG_FLAG)
rdm_LDFLAGS += -lrdkconfig -lsecure_wrapper
endif

rdm_LDFLAGS += -shared -fPIC $(GLIB_LIBS) \
               -lcjson -lrbus -lssl -lcrypto -lcurl -lparsejson -ldwnlutil -lfwutils -lrfcapi

rdm_CFLAGS +=  -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/dbus-1.0 \
               -I${PKG_CONFIG_SYSROOT_DIR}$(libdir)/dbus-1.0/include \
               -I${top_srcdir}/include \
               -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/rbus \
               -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/rdk/iarmbus \
               -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/rdk/iarmmgrs/sysmgr \
               -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/rdk/iarmmgrs/rdmmgr \
               -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/rdk/iarmmgrs-hal

if USE_CPC
  SUBDIRS = rdm-cpc
  rdm_SOURCES += rdm-cpc/rdm/codebigUtils.c \
		 rdm-cpc/rdm/rdm_openssl.c  \
		 rdm-cpc/rdm/rdm_downloadutils.c
  rdm_CFLAGS += -DRDM_ENABLE_CODEBIG \
              -I${top_srcdir}/src/rdm-cpc/rdm
  rdm_LDFLAGS += -lsecure_wrapper
else
  rdm_SOURCES += codebig/codebigUtils.c \
		 rdm_openssl.c \
		 rdm_downloadutils.c
  rdm_CFLAGS += -I${top_srcdir}/src/codebig
endif

if IARMBUS_ENABLE
  rdm_CFLAGS += -DIARMBUS_SUPPORT
endif
