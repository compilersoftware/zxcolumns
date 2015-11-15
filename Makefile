
all: 
	zcc +zxansi -vn -O3 zxcolumns.c -o zxcolumns.bin -lndos
	bin2tap zxcolumns.bin zxcolumns.tap
	rm -f zcc_opt.def

clean:
	rm -f *.bin *.i *.op* *.o *~
