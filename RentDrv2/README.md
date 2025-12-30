# rentdrv2.sys

rentdrv2.sys was decompiled and opened up in IDA Free<br>
![IDA](./media/0.png)

Upon analysing the DriverEntry function, the device anme and symbolic link names were identified and a function was identified as handling the IRP packets, subsequently named `IRPHandler`.<br>
![Devices](./media/1.png)

| Key | Value |
|---|---|
| Device Name | \\\\Device\\rentdrv2 |
| SymLink| \\\\DosDevices\\rentdrv2 |

I searched through the Imports section of the Driver to look for API that may be misused for provisioning an EDRKiller. It was observed that the rentdrv2.sys driver had ZwTerminateProcess listed as an import and was being used somewhere by the IRPHandler. The intermediary subroutines were hence analysed and named based on their attributes and actions.<br>

![5](./media/2.png)
![6](./media/4.png)

The subroutine TerminateProcessByPID, was aptly named due to the argument, `ProcessID` being eventually passed to the ZwOpenProcess API call which opens a handle to a process and the obtained handle is then passed to the ZwTerminateProcess API call, effectively killing the process with the PID passed as `ProcessID`. <br>
![7](./media/3.png)

The control flow would go from the IRPHandler directly to TerminateProcessByPID only if a few conditions are met.<br>
![4](./media/6.png)
![5](./media/5.png)

Therefore, the IOCTL_CODE has to be `0x22E010`, the first four bytes should have the integer 1, the next 4 should have the PId and there should be atleast an 8 Byte padding to make the payload >=`0x10` thats `16 bytes`.<br>

| Key | Value |
| --- | ----- |
| IOCTL Code | 0x22E010 |
| Payload | { Int Flag{=1}; Int PID; BYTE padding[8]; }  |

# EDRKiller
 In the [EDRKiller template](../EDRKiller.zip), the following can be updated<br> 
 ![First update](./media/7.png)

![Final Loop](./media/8.png)

The EDR Killer is ready :)