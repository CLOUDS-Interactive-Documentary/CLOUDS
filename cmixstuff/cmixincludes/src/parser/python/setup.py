from distutils.core import setup, Extension

include = ["../../../include", "../../rtcmix"]

rtcmix_ext = Extension("rtcmix", ["rtcmixmodule.cpp"], include_dirs = include)

setup(name = "rtcmix",
      version = "2.0",
      ext_modules = [rtcmix_ext]
)

