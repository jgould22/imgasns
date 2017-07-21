solaris:
	gcc -Wall asn1.h asn1.c recUtils.h recUtils.c lineUtils.h lineUtils.c -lm -I. -L. -lpnm -o asn1
linux:
	gcc -Wall asn1.h asn1.c recUtils.h recUtils.c lineUtils.h lineUtils.c libpnm.h libpnm.c -lm -I. -L. -o asn1
clean:
	rm asn1