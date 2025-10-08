# BYOVD
Bring Your Own Vulnerable Driver PoCs developed to learn about the process.

# EDR Killer Drivers Added
- [X] BdApiUtil (BdApiUtil.sys)
- [X] K7RKScan (K7RKScan.sys)
- [X] KsApi64 (ksapi64.sys)
- [X] TfSysMon (SysMon.sys)
- [X] TrueSight (truesight.sys)
- [X] Viragt64 (viragt64.sys)
- [X] wsftprm (wsftprm.sys)

# To Do
- [X] Create IAT Parser to check for ZwTerminateProcess in kernel drivers
- - [ ] Fix Formatting Issues
- [X] Create function to handle to IOCTL_CODE blast based on process name

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