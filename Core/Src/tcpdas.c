/*
 * tcp_das.c
 *
 *  Created on: Jun 3, 2020
 *      Author: Silantev
 */

#include "opt.h"
#include "tcpdas.h"

#if LWIP_NETCONN
#include "api.h"
#include "sys.h"
#include "das.h"

#define TCPDAS_THREAD_PRIO (tskIDLE_PRIORITY + 4)

static void tcpdas_thread(void *arg)
{
	struct netconn *conn, *newconn;
	struct netbuf *buf;
	void *data;
	u16_t len;
	volatile err_t err;

	// LWIP_UNUSED_ARG(arg);

	conn = netconn_new(NETCONN_TCP);

	if(conn != NULL)
	{
		if((err = netconn_bind(conn, NULL, 8082)) == ERR_OK)
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

						  if((len = das_read(data, len)) > 0)
						  {
							  netconn_write(newconn, data, len, NETCONN_COPY);
						  }
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

void tcpdas_init(void)
{
	sys_thread_new("tcpdas_thread", tcpdas_thread, NULL, DEFAULT_THREAD_STACKSIZE, TCPDAS_THREAD_PRIO);
}
#endif

