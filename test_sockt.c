
1  /* include checkopts1 */
2  /* *INDENT-OFF* */
3  #include  "unp.h"
4  #include  <netinet/tcp.h>   /* for TCP_xxx defines */
5
6  union val {
    7    int       i_val;
    8    long        l_val;
    9    struct linger   linger_val;
    10    struct timeval  timeval_val;
    11
  } val;
12
13  static char *sock_str_flag(union val *, int);
14  static char *sock_str_int(union val *, int);
15  static char *sock_str_linger(union val *, int);
16  static char *sock_str_timeval(union val *, int);
17
18  struct sock_opts {
  19    const char     *opt_str;
  20    int   opt_level;
  21    int   opt_name;
  22    char   *(*opt_val_str)(union val *, int);
  23
} sock_opts[] = {
  24    { "SO_BROADCAST",   SOL_SOCKET, SO_BROADCAST, sock_str_flag },
  25    { "SO_DEBUG",     SOL_SOCKET, SO_DEBUG,   sock_str_flag },
  26    { "SO_DONTROUTE",   SOL_SOCKET, SO_DONTROUTE, sock_str_flag },
  27    { "SO_ERROR",     SOL_SOCKET, SO_ERROR,   sock_str_int },
  28    { "SO_KEEPALIVE",   SOL_SOCKET, SO_KEEPALIVE, sock_str_flag },
  29    { "SO_LINGER",      SOL_SOCKET, SO_LINGER,    sock_str_linger },
  30    { "SO_OOBINLINE",   SOL_SOCKET, SO_OOBINLINE, sock_str_flag },
  31    { "SO_RCVBUF",      SOL_SOCKET, SO_RCVBUF,    sock_str_int },
  32    { "SO_SNDBUF",      SOL_SOCKET, SO_SNDBUF,    sock_str_int },
  33    { "SO_RCVLOWAT",    SOL_SOCKET, SO_RCVLOWAT,  sock_str_int },
  34    { "SO_SNDLOWAT",    SOL_SOCKET, SO_SNDLOWAT,  sock_str_int },
  35    { "SO_RCVTIMEO",    SOL_SOCKET, SO_RCVTIMEO,  sock_str_timeval },
  36    { "SO_SNDTIMEO",    SOL_SOCKET, SO_SNDTIMEO,  sock_str_timeval },
  37    { "SO_REUSEADDR",   SOL_SOCKET, SO_REUSEADDR, sock_str_flag },
  38  #ifdef  SO_REUSEPORT
  39    { "SO_REUSEPORT",   SOL_SOCKET, SO_REUSEPORT, sock_str_flag },
  40  #else
    41    { "SO_REUSEPORT",   0,      0,        NULL },
  42  #endif
  43    { "SO_TYPE",      SOL_SOCKET, SO_TYPE,    sock_str_int },
  44  //  { "SO_USELOOPBACK",   SOL_SOCKET, SO_USELOOPBACK, sock_str_flag },
  45    { "IP_TOS",       IPPROTO_IP, IP_TOS,     sock_str_int },
  46    { "IP_TTL",       IPPROTO_IP, IP_TTL,     sock_str_int },
  47  #ifdef  IPV6_DONTFRAG
  48    { "IPV6_DONTFRAG",    IPPROTO_IPV6, IPV6_DONTFRAG, sock_str_flag },
  49  #else
    50    { "IPV6_DONTFRAG",    0,      0,        NULL },
  51  #endif
  52  #ifdef  IPV6_UNICAST_HOPS
  53    { "IPV6_UNICAST_HOPS",  IPPROTO_IPV6, IPV6_UNICAST_HOPS, sock_str_int },
  54  #else
    55    { "IPV6_UNICAST_HOPS",  0,      0,        NULL },
  56  #endif
  57  #ifdef  IPV6_V6ONLY
  58    { "IPV6_V6ONLY",    IPPROTO_IPV6, IPV6_V6ONLY, sock_str_flag },
  59  #else
    60    { "IPV6_V6ONLY",    0,      0,        NULL },
  61  #endif
  62    { "TCP_MAXSEG",     IPPROTO_TCP, TCP_MAXSEG,   sock_str_int },
  63    { "TCP_NODELAY",    IPPROTO_TCP, TCP_NODELAY,  sock_str_flag },
  64  #ifdef  SCTP_AUTOCLOSE
  65    { "SCTP_AUTOCLOSE",   IPPROTO_SCTP, SCTP_AUTOCLOSE, sock_str_int },
  66  #else
    67    { "SCTP_AUTOCLOSE",   0,      0,        NULL },
  68  #endif
  69  #ifdef  SCTP_MAXBURST
  70    { "SCTP_MAXBURST",    IPPROTO_SCTP, SCTP_MAXBURST, sock_str_int },
  71  #else
    72    { "SCTP_MAXBURST",    0,      0,        NULL },
  73  #endif
  74  #ifdef  SCTP_MAXSEG
  75    { "SCTP_MAXSEG",    IPPROTO_SCTP, SCTP_MAXSEG, sock_str_int },
  76  #else
    77    { "SCTP_MAXSEG",    0,      0,        NULL },
  78  #endif
  79  #ifdef  SCTP_NODELAY
  80    { "SCTP_NODELAY",   IPPROTO_SCTP, SCTP_NODELAY,  sock_str_flag },
  81  #else
    82    { "SCTP_NODELAY",   0,      0,        NULL },
  83  #endif
  84    { NULL,         0,      0,        NULL }
  85
};
86  /* *INDENT-ON* */
87  /* end checkopts1 */
88
89  /* include checkopts2 */
90  int
91  main(int argc, char **argv)
92  {
  93    int         fd;
  94    socklen_t     len;
  95    struct sock_opts  *ptr;
  96
  97    for (ptr = sock_opts; ptr->opt_str != NULL; ptr++) {
    98      printf("%s: ", ptr->opt_str);
    99      if (ptr->opt_val_str == NULL)
      100        printf("(undefined)\n");
    101      else {
      102        switch (ptr->opt_level) {
      103        case SOL_SOCKET:
      104        case IPPROTO_IP:
      105        case IPPROTO_TCP:
        106          fd = Socket(AF_INET, SOCK_STREAM, 0);
        107          break;
        108  #ifdef  IPV6
      109        case IPPROTO_IPV6:
        110          fd = Socket(AF_INET6, SOCK_STREAM, 0);
        111          break;
        112  #endif
        113  #ifdef  IPPROTO_SCTP
      114        case IPPROTO_SCTP:
        115          fd = Socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
        116          break;
        117  #endif
      118        default:
        119          err_quit("Can't create fd for level %d\n", ptr->opt_level);
        120
      }
      121
      122        len = sizeof(val);
      123        if (getsockopt(fd, ptr->opt_level, ptr->opt_name,
                                124                 & val, &len) == -1) {
        125          err_ret("getsockopt error");
        126
      } else {
        127          printf("default = %s\n", (*ptr->opt_val_str)(&val, len));
        128
      }
      129        close(fd);
      130
    }
    131
  }
  132    exit(0);
  133  }
134  /* end checkopts2 */
135
136  /* include checkopts3 */
137  static char strres[128];
138
139  static char *
140  sock_str_flag(union val *ptr, int len)
141  {
  142  /* *INDENT-OFF* */
  143    if (len != sizeof(int))
    144      snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", len);
  145    else
    146      snprintf(strres, sizeof(strres),
    147           "%s", (ptr->i_val == 0) ? "off" : "on");
  148    return (strres);
  149  /* *INDENT-ON* */
  150  }
151  /* end checkopts3 */
152
153  static char *
154  sock_str_int(union val *ptr, int len)
155  {
  156    if (len != sizeof(int))
    157      snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", len);
  158    else
    159      snprintf(strres, sizeof(strres), "%d", ptr->i_val);
  160    return (strres);
  161  }
162
163  static char *
164  sock_str_linger(union val *ptr, int len)
165  {
  166    struct linger *lptr = &ptr->linger_val;
  167
  168    if (len != sizeof(struct linger))
    169      snprintf(strres, sizeof(strres),
    170           "size (%d) not sizeof(struct linger)", len);
  171    else
    172      snprintf(strres, sizeof(strres), "l_onoff = %d, l_linger = %d",
    173           lptr->l_onoff, lptr->l_linger);
  174    return (strres);
  175  }
176
177  static char *
178  sock_str_timeval(union val *ptr, int len)
179  {
  180    struct timeval  *tvptr = &ptr->timeval_val;
  181
  182    if (len != sizeof(struct timeval))
    183      snprintf(strres, sizeof(strres),
    184           "size (%d) not sizeof(struct timeval)", len);
  185    else
    186      snprintf(strres, sizeof(strres), "%d sec, %d usec",
    187           tvptr->tv_sec, tvptr->tv_usec);
  188    return (strres);
  189  }
root@wl - Lenovo - B590: / myworkspace / unixnetwork / unpv13e / sockopt# man getsockopt
