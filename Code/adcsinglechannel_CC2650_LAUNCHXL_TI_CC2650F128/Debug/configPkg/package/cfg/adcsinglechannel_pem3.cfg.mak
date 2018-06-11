# invoke SourceDir generated makefile for adcsinglechannel.pem3
adcsinglechannel.pem3: .libraries,adcsinglechannel.pem3
.libraries,adcsinglechannel.pem3: package/cfg/adcsinglechannel_pem3.xdl
	$(MAKE) -f C:\Users\JANELL~1\workspaceMark3\adcsinglechannel_CC2650_LAUNCHXL_TI_CC2650F128/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\JANELL~1\workspaceMark3\adcsinglechannel_CC2650_LAUNCHXL_TI_CC2650F128/src/makefile.libs clean

