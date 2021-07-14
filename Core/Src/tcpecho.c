/*
 * tcpecho.c
 *
 *  Created on: Jun 3, 2020
 *      Author: Silantev
 */

#include "opt.h"

#if LWIP_NETCONN
#include "api.h"
#include "sys.h"

#define TCPECHO_THREAD_PRIO (tskIDLE_PRIORITY + 4)

static void tcpecho_thread(void *arg)
{
	struct netconn *conn, *newconn;
	struct netbuf *buf;
	void *data;
	u16_t len;
	volatile err_t err;

	LWIP_UNUSED_ARG(arg);

	conn = netconn_new(NETCONN_TCP);

	if(conn != NULL)
	{
	  if((err = netconn_bind(conn, NULL, 8081)) == ERR_OK)
	  {
		  netconn_listen(conn);

		  while(1)
		  {
			  if((err = netconn_accept(conn, &newconn)) == ERR_OK)
			  {
				  while((err = netconn_recv(newconn, &buf)) == ERR_OK)
				  {
					  do
					  {
						  netbuf_data(buf, &data, &len);
						  netconn_write(newconn, data, len, NETCONN_COPY);
					  }
					  while (netbuf_next(buf) >= 0);

					  netbuf_delete(buf);
				  }

				  netconn_close(newconn);
				  netconn_delete(newconn);
			  }
		  }
	  }
	  else
	  {
		  netconn_delete(newconn);
	  }
	}
}

void tcpecho_init(void)
{
	sys_thread_new("tcpecho_thread", tcpecho_thread, NULL, DEFAULT_THREAD_STACKSIZE, TCPECHO_THREAD_PRIO);
}
#endif
