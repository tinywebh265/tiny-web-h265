chmod 755 www
chmod 666 www/*

pkill -9 thttpd
./thttpd -p 8090 -d www -nos