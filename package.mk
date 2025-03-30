PKG_NAME	:= allocators
PKG_SOURCE	:= https://github.com/AnatolyRybchych/allocators
PKG_BRANCH	:= v0.0.0
PKG_VERSION	:= v0.0.0
PKG_DEPENDS :=

PKG_INCLUDE_DIR	:= $(PKG_DIR)/include

include $(PKG_MGR)/package.mk

define Package/Build
endef

$(eval $(call Package))
