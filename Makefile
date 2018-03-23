default: 
	cd src && $(MAKE) && cp a.out ../20161662.out && $(MAKE) clean

20161662.out :
	cd src && $(MAKE) && cp a.out ../20161662.out && $(MAKE) clean
	
.DEFAULT :
	cd src && $(MAKE) $@
