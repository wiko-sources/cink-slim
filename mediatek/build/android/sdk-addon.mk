ifeq ($(TARGET_PRODUCT),banyan_addon)

# include MTK SDK toolset version file
include $(MTK_PATH_SOURCE)/frameworks/banyan/TOOLSET_VERSION

# make dependency between banyan_addon/sdk_addon and checkmtkapi
sdk_addon: checkmtkapi

endif

