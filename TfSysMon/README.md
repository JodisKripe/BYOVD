# TfSysMon

SysMon.sys was decompiled and opened up in IDA Free <br>
![IDA Free](./media/1.png)

The DriverObject was passed to a different function `sub_17484`. We can assume that all the activity happens within that function. The DeviceName, SymLink and the IOControlRoutine was found from `sub_17848`<br>
![RealDriverEntry](./media/2.png)

|  |  |
|--|--|
| DriverName| \\\\Device\\TfSysMon|
| SymLink| \\\\DosDevices\\TfSysMon |
|IoControlRoutine|sub_17694|

Now we work backwards from ZwTerminateProcess. The call is only made within once function, `sub_1837C`<br>
![ZwTerminateProcess](./media/3.png)

The function is fairly straightforward, it performs the following actions:
- Check if the arguement passed to it is at a valid address
- Obtain handle to a process using part of the payload as the PID (unsigned integer starting from offset +4)
- If opening the process does not end up in an error, (i.e. v6 = 0 --> v7 is boolean false) then try 3 times to terminate the process using the handle recieved from the `ZwOpenProcess` call.<br>

The function is renamed as `TerminateProcess`<br>
![TerminateProcess](./media/4.png)

Only one xref is found, which is within  `sub_177D8` <br>
![Xref TerminateProcess](./media/5.png)

The function has many wrongly tagged data types which need to be fixed, but it clearly handles IOCTL requests based on the xrefs it is called from. <br>
![sub_177D8](./media/6.png)

The IOCTL code and Payload type can be discovered from where the `TerminateProcess` function is being called within `sub_177D8` <br>
![IOCTL](./media/7.png)

|  |  |
|--|--|
|IOCTL Code| 0xB4A00404 |
|Payload| Size more than 24, PID as an unsigned integer starting from the +4 offset |

This is so, since the System Buffer is directly passed to the TerminateProcess function.

Note: From the IOControlRoutine(`sub_17694`), we can see where and how the `sub_177D8` is called and how it is basically as if the `sub_177D8` was the IOControlRoutine function instead of `sub_17694`<br>
![IOControlRoutine](./media/8.png)

# EDRKiller

The following changes can be made in the [EDRKiller Template](../EDRKiller.zip) <br>
![EDRKiller](./media/9.png)

Rest its just about populating it and passing the data to the driver <br>
![EDRKiller](./media/10.png)

Note: The killerBuff can also work with byte padding instead of integer padding, its just about proper offset of +4 and total size more than equal to 24.

The EDRKiller is ready :)