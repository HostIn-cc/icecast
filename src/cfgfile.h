/* Icecast
 *
 * This program is distributed under the GNU General Public License, version 2.
 * A copy of this license is included with this source.
 *
 * Copyright 2010-2022, Karl Heyes <karl@kheyes.plus.com>,
 * Copyright 2000-2004, Jack Moffitt <jack@xiph.org>,
 *                      Michael Smith <msmith@xiph.org>,
 *                      oddsock <oddsock@xiph.org>,
 *                      Karl Heyes <karl@xiph.org>
 *                      and others (see AUTHORS for details).
 */

#ifndef __CFGFILE_H__
#define __CFGFILE_H__

#define CONFIG_EINSANE -1
#define CONFIG_ENOROOT -2
#define CONFIG_EBADROOT -3
#define CONFIG_EPARSE -4

#define MAX_YP_DIRECTORIES 25

#define XMLSTR		(xmlChar *)

struct _mount_proxy;
struct ice_config_tag;
typedef struct _listener_t listener_t;

#include "avl/avl.h"
#include "auth.h"
#include "compat.h"


extern uint64_t config_updated;

typedef struct _redirect_host
{
    struct _redirect_host *next;
    time_t next_update;
    char *server;
    int port;
} redirect_host;


typedef struct access_log
{
    char *name;
    int logid;
    int log_ip;
    int qstr;
    int type;
    int archive;
    int display;
    long size;
    unsigned duration;
    char *exclude_ext;
} access_log;

#define LOG_ACCESS_CLF                  0
#define LOG_ACCESS_CLF_ESC              1

typedef struct error_log
{
    char *name;
    int logid;
    int archive;
    int display;
    long size;
    unsigned duration;
    int level;
} error_log;

typedef struct playlist_log
{
    char *name;
    int logid;
    int archive;
    int display;
    long size;
    unsigned duration;
} playlist_log;

typedef struct error_log preroll_log;


typedef struct ice_config_dir_tag
{
    char *host;
    int touch_interval;
    struct ice_config_dir_tag *next;
} ice_config_dir_t;

typedef struct _config_options {
    char *name;
    char *value;
    struct _config_options *next;
} config_options_t;

typedef struct _mount_proxy {
    char *mountname; /* The mountpoint this proxy is used for */

    char *username; /* Username and password for this mountpoint. If unset, */
    char *password; /* falls back to global source password */

    char *dumpfile; /* Filename to dump this stream to (will be appended). NULL
                       to not dump. */
    char *intro_filename;   /* Send contents of file to client before the stream */

    /* whether to allow matching files to work with http ranges */
    int file_seekable;

    int fallback_when_full; /* switch new listener to fallback source
                               when max listeners reached */
    /* Max bandwidth (kbps)  for this mountpoint only. -1 (default) is not specified */
    int64_t max_bandwidth;

    int max_listeners; /* Max listeners for this mountpoint only. -1 to not
                          limit here (i.e. only use the global limit) */
    char *fallback_mount; /* Fallback mountname */

    int fallback_override; /* When this source arrives, do we steal back
                              clients from the fallback? */
    int ban_client;     /* do we add a client on this to the ban list automatically */
    int no_mount; /* Do we permit direct requests of this mountpoint? (or only
                     indirect, through fallbacks) */
    int so_sndbuf;      /* TCP send buffer size for new clients */
    uint32_t burst_size;
    uint32_t min_queue_size;     /* minimum length of queue */
    uint32_t queue_size_limit;
    uint32_t _refcount;
    int hidden; /* Do we list this on the xsl pages */
    unsigned int source_timeout;  /* source timeout in seconds */
    char *charset;  /* character set if not utf8 */
    int allow_chunked; /* allow chunked transfers */
    int mp3_meta_interval; /* outgoing per-stream metadata interval */
    int max_send_size;
    int filter_theora; /* prevent theora pages getting queued */
    int url_ogg_meta; /* enable to allow updates via url requests for ogg */
    int ogg_passthrough; /* enable to prevent the ogg stream being rebuilt */
    int admin_comments_only; /* enable to only show comments set from the admin page */
    int skip_accesslog;         /* skip logging client to access log */
    int intro_skip_replay;      /* duration to cache IPs, for intro playing */

    int64_t limit_rate;

    /* duration (secs) for mountpoint to be kept reserved after source client exits */
    int wait_time;

    char *auth_type; /* Authentication type */
    struct auth_tag *auth;
    char *cluster_password;
    config_options_t *auth_options; /* Options for this type */
    char *on_connect;
    char *on_disconnect;
    unsigned int max_stream_duration;
    unsigned int max_listener_duration;

    struct access_log      access_log;
    preroll_log     preroll_log;

    char *redirect;
    char *stream_name;
    char *stream_description;
    char *stream_url;
    char *stream_genre;
    char *bitrate;
    char *type;
    char *subtype;
    int yp_public;

    struct _mount_proxy *next;
} mount_proxy;

typedef struct _aliases {
    char *source;
    char *destination;
    int port;
    char *bind_address;
    struct _aliases *next;
}aliases;


struct xforward_entry
{
    char *ip;
    struct xforward_entry *next;
};


struct _listener_t
{
    struct _listener_t *next;
    int refcount;
    int port;
    char *bind_address;
    char *shoutcast_mount;
    int qlen;
    int shoutcast_compat;
    int so_sndbuf;
    int so_mss;
};


#define RELAY_RUNNING                   1
#define RELAY_CLEANUP                   (1<<1)
#define RELAY_ON_DEMAND                 (1<<2)
#define RELAY_ICY_META                  (1<<3)
#define RELAY_FROM_MASTER               (1<<4)
#define RELAY_SLAVE                     (1<<5)
#define RELAY_IN_LIST                   (1<<6)

typedef struct _relay_server_host
{
    struct _relay_server_host *next;
    char *ip;
    char *bind;
    char *mount;
    int priority;
    int port;
    int timeout;
    int skip;
} relay_server_host;


typedef struct _relay_server
{
    struct _relay_server *new_details;
    struct source_tag *source;
    time_t updated;
    int interval;
    int run_on;
    unsigned char type;
    unsigned char flags;
    char *localmount;
    relay_server_host *hosts, *in_use;
    char *username;
    char *password;
} relay_server;


typedef struct
{
    char *hostname;
    int  port;
    char *username;
    char *password;
} ice_master_details;


typedef struct ice_config_tag
{
    char *config_filename;

    char *gitversion;
    char *location;
    char *admin;

    int client_limit;
    int source_limit;
    unsigned int queue_size_limit;
    int min_queue_size;
    int workers_count;
    uint32_t burst_size;
    int client_timeout;
    int header_timeout;
    int source_timeout;
    int ice_login;
    int64_t max_bandwidth;
    int max_listeners;
    int fileserve;
    int on_demand; /* global setting for all relays */

    char *shoutcast_mount;
    char *source_password;
    char *admin_username;
    char *admin_password;
    char *relay_username;
    char *relay_password;

    int inactivity_timeout;
    int touch_interval;
    ice_config_dir_t *dir_list;

    char *hostname;
    int port;
    char *mimetypes_fn;

    listener_t *listen_sock;
    unsigned int listen_sock_count;

    char *master_server;
    int master_server_port;
    int master_update_interval;
    char *master_bind;
    char *master_username;
    char *master_password;
    int master_relay_auth;
    int master_relay_retry;
    int master_ssl_port;
    int master_redirect;
    int master_run_on;
    int max_redirects;
    struct _redirect_host *redirect_hosts;
    struct xforward_entry *xforward;

    relay_server *relays;

    mount_proxy *mounts;
    avl_tree *mounts_tree;

    char *server_id;
    char *base_dir;
    char *log_dir;
    char *pidfile;
    char *banfile;
    char *allowfile;
    char *agentfile;
    char *cert_file;
    char *key_file;
    char *ca_file;
    char *cipher_list;
    char *webroot_dir;
    char *adminroot_dir;
    struct _aliases *aliases;
    unsigned slaves_count;

    struct access_log      access_log;
    struct error_log       error_log;
    struct playlist_log    playlist_log;
    preroll_log     preroll_log;

    int chroot;
    int chuid;
    char *user;
    char *group;
    char *yp_url[MAX_YP_DIRECTORIES];
    int    yp_url_timeout[MAX_YP_DIRECTORIES];
    int    yp_touch_interval[MAX_YP_DIRECTORIES];
    int num_yp_directories;
} ice_config_t;

typedef struct {
    rwlock_t config_lock;
    spin_t mount_lock;
} ice_config_locks;

void config_initialize(void);
void config_shutdown(void);

int config_parse_file(const char *filename, ice_config_t *configuration);
int config_initial_parse_file(const char *filename);
int config_parse_cmdline(int arg, char **argv);
void config_set_config (ice_config_t *new_config, ice_config_t *old_config);
listener_t *config_clear_listener (listener_t *listener);
relay_server *config_clear_relay (relay_server *relay);
void config_clear(ice_config_t *config);
void config_clear_mount (mount_proxy *mountinfo, int log);
#define config_release_mount(x) config_clear_mount(x,0)
int config_mount_ref (mount_proxy *mountinfo, int inc);
mount_proxy *config_lock_mount (ice_config_t *config, const char *mount);
mount_proxy *config_find_mount (ice_config_t *config, const char *mount);
void config_xml_parse_failure (void*x,  const xmlError *error);
int config_qsizing_conv_a2n (const char *str, uint32_t *p);

int config_rehash(void);

ice_config_locks *config_locks(void);

ice_config_t *config_get_config(void);
ice_config_t *config_grab_config(void);
void config_release_config(void);

/* To be used ONLY in one-time startup code */
ice_config_t *config_get_config_unlocked(void);

#endif  /* __CFGFILE_H__ */
