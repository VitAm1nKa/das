/*
 * connection_pipeline.c
 *
 *  Created on: Apr 3, 2021
 *      Author: Silantev
 */

#include "opt.h"
#include "api.h"
#include "sys.h"

#define TCPECHO_THREAD_PRIO (tskIDLE_PRIORITY + 4)

struct netconn *conn, *newconn;
struct netbuf *buf;
void *data;
u16_t len;
volatile err_t err;

void listening()
{
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
						  // receive request data
						  netbuf_data(buf, &data, &len);

						  // write request data
						  netconn_write(newconn, data, len, NETCONN_COPY);

//						  if((len = das_read(data, len)) > 0)
//						  {
//							  netconn_write(newconn, data, len, NETCONN_COPY);
//						  }
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

void start_listening(void)
{
	sys_thread_new("listening_thread", listening, NULL, DEFAULT_THREAD_STACKSIZE, TCPECHO_THREAD_PRIO);
}

