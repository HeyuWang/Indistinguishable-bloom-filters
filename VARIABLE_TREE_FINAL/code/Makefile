 
a.exe: bloomfilter.o bloomfiltertree.o csprng.o dataitem.o hmac_sha256.o key128bitGenerator.o sha1.o sha256.o helper.o main.o
	gcc   bloomfilter.o bloomfiltertree.o csprng.o dataitem.o hmac_sha256.o key128bitGenerator.o sha1.o sha256.o helper.o  main.o -o a -lm
main.o:main.c
	gcc -c -O2 main.c
bloomfilter.o: bloomfilter.c
	gcc -c -O2 bloomfilter.c
bloomfiltertree.o:bloomfiltertree.c
	gcc -c -O2  bloomfiltertree.c -lm
csprng.o: csprng.c
	gcc -c -O2 csprng.c
dataitem.o: dataitem.c
	gcc -c -O2 dataitem.c
hmac_sha256.o:hmac_sha256.c
	gcc -c -O2 hmac_sha256.c
key128bitGenerator.o:key128bitGenerator.c
	gcc -c -O2 key128bitGenerator.c
sha1.o: sha1.c
	gcc -c -O2 sha1.c
sha256.o: sha256.c
	gcc -c -O2 sha256.c
helper.o: helper.c
	gcc -c -O2 helper.c
clean:
	-rm -f *.o

	

