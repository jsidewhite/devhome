midlrt /winrt -nomidl -out x64\Release\Unmerged -metadata_dir "C:\Windows\System32\WinMetadata" W:\repo\devhome\tools\QuietBackgroundProcesses\DevHome.QuietBackgroundProcesses.ElevatedServer\DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager.idl
midlrt /winrt -nomidl -out x64\Release\Unmerged -metadata_dir "C:\Windows\System32\WinMetadata" W:\repo\devhome\tools\QuietBackgroundProcesses\DevHome.QuietBackgroundProcesses.ElevatedServer\DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.idl
mdmerge "@`"mdmerge.rspfile`""
