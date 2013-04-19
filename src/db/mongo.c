#include <regex.h>
#include <stdlib.h>
#include <string.h>

#include "db/types/mongo_host.h"
#include "logging.h"

#define USER_REGEX "([-.A-Za-z0-9_:]+)"
#define PASS_REGEX "([^@,]+)"
#define AUTH_REGEX "(" USER_REGEX ":" PASS_REGEX "@)?"

#define HOST_REGEX "([-.A-Za-z0-9_]+)"
#define PORT_REGEX "(:([0-9]+))?"
#define NODE_REGEX "((" HOST_REGEX PORT_REGEX ",?)+)"

#define PATH_REGEX "(/([-A-Za-z0-9_]+))?"

#define MONGODB_URI_MATCHER "^" AUTH_REGEX NODE_REGEX PATH_REGEX "$"

static int count_delimiters(char const* string, char d)
{
    int i, n = 0;
    if (string)
        for (i = 0; string[i]; i++)
            if (string[i] == d)
                n++;
    return n;
}

static void free_strings(char** strings)
{
    int i;
    for (i = 0; strings[i]; i++)
        free(strings[i]);
    free(strings);
}

static char** split(char const* string, char d)
{
    int i, len, n = count_delimiters(string, d);
    char** strings = calloc(n + 2, sizeof(char *));
    char* current;

    if (strings) {
        current = (char*)string;
        for (i = 0; i < n + 1; i++) {
            len = 0;
            if (current)
                while (current[len] && current[len] != d)
                    len++;
            strings[i] = malloc(len+1);
            if (!strings[i]) {
                free_strings(strings);
                strings = 0;
                break;
            }
            if (current)
                strncpy(strings[i], current, len);
            strings[i][len] = 0;
            if (current) {
                while (current[len] && current[len] != d)
                    len++;
                current += len + 1;
            }
        }
    }

    if (!strings)
        log_error("Couldn't allocate space for strings");
    return strings;
}

static struct mongo_host* build_host(char* hosturi, char* uname, char* pwd, char* db, struct mongo_host* next)
{
    struct mongo_host* host = malloc(sizeof(struct mongo_host));
    char** host_port = split(hosturi, ':');

    if (host && host_port) {
        host->host = malloc(strlen(host_port[0]) + 1);
        host->username = uname ? malloc(strlen(uname) + 1) : 0;
        host->password = pwd ? malloc(strlen(pwd) + 1) : 0;
        host->database = db ? malloc(strlen(db) + 1) : 0;
        if (!host->host ||
            (uname && !host->username) ||
            (pwd && !host->password) ||
            (db && !host->database)) {
            log_error("Failed to allocate space for host data\n");
            free(host->host);
            free(host->username);
            free(host->password);
            free(host->database);
            free(host);
            host = 0;
        }
        else {
            strcpy(host->host, host_port[0]);
            host->port = host_port[1] ? atoi(host_port[1]) : 27017;
            if (uname)
                strcpy(host->username, uname);
            if (pwd)
                strcpy(host->password, pwd);
            if (db)
                strcpy(host->database, db);
            host->next = next;
        }
        free_strings(host_port);
    }
    else
        log_error("Failed to allocate host structure\n");

    return host;
}

static char* get_match(char* s, regmatch_t match)
{
    char* m = 0;

    if (match.rm_so < match.rm_eo && (m = malloc((match.rm_eo - match.rm_so) + 1))) {
        strncpy(m, s + match.rm_so, match.rm_eo - match.rm_so);
        m[match.rm_eo - match.rm_so] = 0;
    }

    return m;
}

static struct mongo_host* parse_hosts(char* uri_without_proto)
{
    regex_t matcher;
    int match_count;
    char error[64];
    regmatch_t* matches = 0;
    int i, host_count;
    char* uname, *pwd, *hoststring, *db;
    char** hosturis;
    struct mongo_host* hosts = 0;

    if (regcomp(&matcher, MONGODB_URI_MATCHER, REG_EXTENDED)) {
        log_error("Error compiling regex\n");
        return 0;
    }

    match_count = 1 + matcher.re_nsub;
    matches = calloc(match_count, sizeof(regmatch_t));
    if (!matches) {
        regfree(&matcher);
        return 0;
    }

    if (!regexec(&matcher, uri_without_proto, match_count, matches, 0)) {
        uname = get_match(uri_without_proto, matches[2]);
        pwd = get_match(uri_without_proto, matches[3]);
        db = get_match(uri_without_proto, matches[10]);

        if (hoststring = get_match(uri_without_proto, matches[4])) {
            hosturis = split(hoststring, ',');
            free(hoststring);
        }
        else {
            log_error("Failed to find host\n");
            regfree(&matcher);
            free(matches);
            free(uname);
            free(pwd);
            free(db);
            free(hoststring);
            return 0;
        }

        if ((uname || pwd) && !db) {
            log_error("Username or password without db\n");
            regfree(&matcher);
            free(matches);
            free_strings(hosturis);
            free(uname);
            free(pwd);
            free(db);
            return 0;
        }

        for (host_count = 0; hosturis[host_count]; host_count++)
            ;

        for (i = host_count-1; i >= 0; i--)
            if (!(hosts = build_host(hosturis[i], uname, pwd, db, hosts))) {
                regfree(&matcher);
                free(matches);
                free_strings(hosturis);
                free(uname);
                free(pwd);
                free(db);
                return 0;
            }

        regfree(&matcher);
        free(matches);
        free_strings(hosturis);
        free(uname);
        free(pwd);
        free(db);
        return hosts;
    }
    else {
        log_error("Failed to match mongodb URI in '%s'\n", uri_without_proto);
        regfree(&matcher);
        free(matches);
        return 0;
    }
}

struct mongo_host* parse_mongo_uri(char const* uri)
{
    if (strstr(uri, "mongodb://") != uri || strchr(uri, '?'))
        return 0;

    return parse_hosts((char*)uri + 10);
}

void free_hosts(struct mongo_host* hosts)
{
    if (hosts) {
        free(hosts->host);
        free(hosts->username);
        free(hosts->password);
        free(hosts->database);
        free_hosts(hosts->next);
    }
    free(hosts);
}
