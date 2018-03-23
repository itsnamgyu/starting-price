default: 
	cd src && $(MAKE) && cp a.out ../20161662.out && $(MAKE) clean
	
.DEFAULT :
	cd src && $(MAKE) $@
