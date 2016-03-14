# UDP-Broadcast
#注意
##udp广播时，自保留一个网卡，把其他的网卡禁用，否则不知道他是使用的哪一个网卡导致发送接受不到广播，或者绑定本机ip
###_server_add.sin_addr.s_addr = htons(INADDR_ANY); // 改成本地的ip 或者禁用其他的网卡
