# BYOVD
Bring Your Own Vulnerable Driver PoCs developed to learn about the process.

# EDR Killer Drivers Added
- [X] [BdApiUtil](./BdApiUtil/README.md) (BdApiUtil.sys)
- [X] [K7RKScan](./K7RKScan/README.md) (K7RKScan.sys)
- [X] KsApi64 (ksapi64.sys)
- [X] [TfSysMon](./TfSysMon/README.md) (SysMon.sys)
- [X] TrueSight (truesight.sys)
- [X] Viragt64 (viragt64.sys)
- [X] [wsftprm](./wsftprm/README.md) (wsftprm.sys)
- [X] Blackout (gmer.sys) {revoked cert}
- [X] NSecKrnl  (NSecKrnl.sys)
- [X] RentDrv2  (rentdrv2.sys)
- [X] mhyProt   (mhyprot2.sys) {revoked cert}

# To Do
- [X] Create IAT Parser to check for ZwTerminateProcess in kernel drivers
- - [X] Move ZwTerminateProcessHunter to a standalone repository and add as submodule
- [X] Create function to handle to IOCTL_CODE blast based on process name
- [ ] Develop Writeups for all EDRKillers

# Testing Kernel Drivers
## Create and start Service
> sc.exe create SysMon type= kernel start= demand binPath= 'D:\BYOVD\TfSysMon\SysMon.sys' DisplayName= 'SysMonTF'

> sc.exe start SysMon

## Stop and Remove the Service
> sc.exe stop SysMon

> sc.exe delete SysMon

## Or just use OSRLoader :)
https://www.osronline.com/article.cfm%5Earticle=157.htm

# EDRKiller.zip
Boilerplate Visual Studio Template to prepare fresh EDRKillers