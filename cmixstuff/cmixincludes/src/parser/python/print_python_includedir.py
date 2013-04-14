import distutils.sysconfig
print distutils.sysconfig.get_config_vars("INCLUDEPY")[0]

