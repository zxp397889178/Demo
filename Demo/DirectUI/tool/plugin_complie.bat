@echo off 

call PluginCacheBuilder.exe "--plugin-dll-path=%cd%\PluginFramework_u.dll" "--plugin-cache-path=%cd%\Plugins\RegCache.db" "--plugin-config-path=%cd%\Plugins\LoadModules.xml"