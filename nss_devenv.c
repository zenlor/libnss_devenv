/*
  This file is part of nss-devenv.

  Copyright 2013 Lorenzo Giuliani <lorenzo@frenzart.com>
  Contains portions of nss-myhostname, copyright 2008-2011 Lennart Poettering

  nss-devenv is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  as published by the Free Software Foundation; either version 2.1 of
  the License, or (at your option) any later version.

  nss-devenv is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with nss-devenv; If not, see
  <http://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <nss.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <resolv.h>
#include <arpa/inet.h>

// 127.0.0.2 for local ipv4 address
#define LOCALADDRESS_IPV4 (htonl(0x7F000002))
// ::1 for ipv6
#define LOCALADDRESS_IPV6 &in6addr_loopback

#define ALIGN(a) (((a+sizeof(void*)-1)/sizeof(void*))*sizeof(void*))

static inline size_t PROTO_ADDRESS_SIZE(int proto) {
	assert(proto == AF_INET || proto == AF_INET6);
	return proto == AF_INET6 ? 16 : 4;
}

static enum nss_status _fill_hostent (
		const char *name,
		int af,
		struct hostent *result,
		char *buffer,
		size_t buflen,
		int *errnop,
		int *h_errnop) {

	char *r_addr, *r_name, *r_aliases, *r_addr_list;
	size_t l, idx, ms, alen;

	alen = PROTO_ADDRESS_SIZE(af);

	l = strlen(name);
	ms = ALIGN(l+1)+
		sizeof(char*);

	if (buflen < ms) {
		*errnop = ENOMEM;
		*h_errnop = NO_RECOVERY;
		return NSS_STATUS_TRYAGAIN;
	}

	// hostname
	r_name = buffer;
	memcpy(r_name, name, l+1);

	idx = ALIGN(l+1);

	// empty list of aliases
	r_aliases = buffer + idx;
	*(char**) r_aliases = NULL;
	idx += sizeof(char*);

	// address (local)
	r_addr = buffer+idx;
	if (af == AF_INET) {
		result->h_addrtype = AF_INET;
		*(uint32_t*) r_addr = LOCALADDRESS_IPV4;
	} else {
		result->h_addrtype = AF_INET6;
		memcpy(r_addr, LOCALADDRESS_IPV6, 16);
	}
	idx += ALIGN(alen);

	// address list
	r_addr_list = buffer + idx;
	((char**) r_addr_list)[0] = r_addr;
	((char**) r_addr_list)[1] = NULL;
	idx += 2*sizeof(char*);

	result->h_name = r_name;
	result->h_aliases = (char**) r_aliases;
	result->h_length = alen;
	result->h_addr_list = (char**) r_addr_list;
	return NSS_STATUS_SUCCESS;
}

	enum nss_status _nss_devenv_gethostbyname2_r (const char *name, int af, struct hostent *result,
		char *buffer, size_t buflen, int *errnop,
		int *h_errnop)
{
	if (af == AF_UNSPEC) {
		af = AF_INET;
	}

	if (af != AF_INET && af != AF_INET6) {
		*errnop = EAFNOSUPPORT;
		*h_errnop = NO_DATA;
		return NSS_STATUS_UNAVAIL;
	}

	enum nss_status status = NSS_STATUS_NOTFOUND;

	int len;
	len = strlen(name);

	if (strcmp(".dev", name + len - 4) == 0) {
		status = _fill_hostent(name, af, result, buffer, buflen, errnop, h_errnop);
	}

	return status;
}

enum nss_status _nss_devenv_gethostbyname_r(
		const char *name,
		struct hostent * result,
		char *buffer,
		size_t buflen,
		int *errnop,
		int *h_errnop) {
	return _nss_devenv_gethostbyname2_r(name, AF_UNSPEC, result, buffer, buflen, errnop, h_errnop);
}
