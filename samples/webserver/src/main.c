#include "mongoose.h"
#include <stdio.h>
#define SERVER_ADDR "http://0.0.0.0:8000"
static const char *s_http_addr = SERVER_ADDR;
static void fn(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    
    if (mg_match(hm->uri, mg_str("/"), NULL) || mg_match(hm->uri, mg_str("/index.html"), NULL)) {
      mg_printf(c,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Transfer-Encoding: chunked\r\n\r\n");
      mg_http_printf_chunk(c,
        "<!DOCTYPE html>"
        "<html>"
        "<head><title>Webserver Demo</title></head>"
        "<body>"
        "<h1>Hello, World!</h1>"
        "</body>"
        "</html>");
      mg_http_printf_chunk(c, "");
      
    } else {
      mg_http_reply(c, 404, "", "Not Found\n");
    }
  } else if (ev == MG_EV_WS_MSG) {
    // Handle WebSocket messages (echo back)
    struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;
    mg_ws_send(c, wm->data.buf, wm->data.len, WEBSOCKET_OP_TEXT);
  }
}
int main(void) {
  mg_log_set(MG_LL_ERROR);                    // Set log level
  setvbuf(stdout, NULL, _IONBF, 0);           // Disable stdout buffering
  
  // Display startup banner
  printf("\n**********************************************\n");
  printf("Web server starting...\n");
  printf("Web server listening on %s\n", s_http_addr);
  printf("**********************************************\n");
  printf("To connect:\n");
  printf("1. Find this device's IP: net iface\n");
  printf("2. Open your web browser\n");
  printf("3. Navigate to: http://<IP>:8000\n");
  printf("**********************************************\n");
  fflush(stdout);
  
  struct mg_mgr mgr;                          // Event manager
  mg_mgr_init(&mgr);                          // Initialize event manager
  mg_http_listen(&mgr, s_http_addr, fn, NULL); // Create HTTP listener
  printf("Server running at %s\n", s_http_addr);
  for (;;) mg_mgr_poll(&mgr, 1000);           // Infinite event loop
  
  mg_mgr_free(&mgr);
  return 0;
}
