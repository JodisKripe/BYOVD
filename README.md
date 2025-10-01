# BYOVD
Bring Your Own Vulnerable Driver PoCs developed to learn about the process.

# Testing Kernel Drivers
## Create and start Service
> sc.exe create SysMon type= kernel start= demand binPath= 'D:\BYOVD\TfSysMon\SysMon.sys' DisplayName= 'SysMonTF'

> sc.exe start SysMon

## Stop and Remove the Service
> sc.exe stop SysMon

> sc.exe delete SysMon

## Or just use OSRLoader :)
https://www.osronline.com/article.cfm%5Earticle=157.htm

# DriverCommunication.zip
Boilerplate Visual Studio Template (BdApiUtil) to prepare fresh EDRKillers