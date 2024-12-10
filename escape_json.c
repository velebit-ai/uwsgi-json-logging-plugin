/*
  uWSGI plugin that creates custom json-escaped logging variables.

  build plugin with `uwsgi --build-plugin <filename.c>`
  and use it with `uwsgi --plugin <filename_plugin.so> ...`
*/
#include <uwsgi.h>

// Inspired by
// https://github.com/unbit/uwsgi/blob/52d858af7148a7084628d1241a7597441bb84f95/core/logging.c#L1986-L2031
#define lf_json(x, y) \
  static ssize_t uwsgi_lf_json_ ## x(struct wsgi_request *wsgi_req, char **buf) { \
    long pos = offsetof(struct wsgi_request, y); \
    long pos_len = offsetof(struct wsgi_request, y ## _len); \
    char **var = (char **) (((char *) wsgi_req) + pos); \
    uint16_t *varlen = (uint16_t *) (((char *) wsgi_req) + pos_len); \
    *buf = uwsgi_malloc((*varlen * 2) + 1); \
    escape_json(*var, *varlen, *buf); \
    return strlen(*buf); \
  }

#define r_logchunk(x) \
  uwsgi_register_logchunk("json_" #x, uwsgi_lf_json_ ## x, 1)

lf_json(uri, uri)
lf_json(method, method)
lf_json(user, remote_user)
lf_json(addr, remote_addr)
lf_json(host, host)
lf_json(proto, protocol)
lf_json(uagent, user_agent)
lf_json(referer, referer)

static void register_logchunks() {
  r_logchunk(uri);
  r_logchunk(method);
  r_logchunk(user);
  r_logchunk(addr);
  r_logchunk(host);
  r_logchunk(proto);
  r_logchunk(uagent);
  r_logchunk(referer);
}

struct uwsgi_plugin escape_json_plugin = {
  .name = "escape_json",
  .on_load = register_logchunks,
};
