// SPDX-License-Identifier: (LGPL-2.1 OR BSD-2-Clause)
/* Copyright (c) 2020 Facebook */
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <bpf/libbpf.h>
#include "xdp_redirect.skel.h"
#include <net/if.h>
static int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args)
{
	return vfprintf(stderr, format, args);
}



int main(int argc, char **argv)
{
	struct xdp_redirect_bpf *skel;
	int err;
	
	/* Set up libbpf errors and debug info callback */
	libbpf_set_print(libbpf_print_fn);
	__u32 key=0;
	__u32 if_count=0;
	
	struct if_nameindex *if_nidxs, *intf;
	
	/* Open BPF application */
	skel = xdp_redirect_bpf__open();
	if (!skel) {
		fprintf(stderr, "Failed to open BPF skeleton\n");
		return 1;
	}
	fprintf(stderr, "Opened BPF skeleton\n");
	
	/* Load & verify BPF programs */
	err = xdp_redirect_bpf__load(skel);
	if (err) {
		fprintf(stderr, "Failed to load and verify BPF skeleton\n");
		goto cleanup;
	}
	fprintf(stderr, "Loaded and verified BPF skeleton\n");
	
	err = bpf_map__update_elem(skel->maps.intf_count, &key, 4, &if_count, 4, 0);
	if (err) {
		fprintf(stderr, "Failed to update BPF map\n");
		goto cleanup;
	}
	fprintf(stderr, "Updated BPF map\n");
	
	/* Attach to every interface */
	if_nidxs = if_nameindex();
    if ( if_nidxs != NULL )
    {
        for (intf = if_nidxs; intf->if_index != 0 || intf->if_name != NULL; intf++)
        {
        	bpf_program__attach_xdp(skel->progs.xdp_redirect, intf->if_index);
        }
        if_freenameindex(if_nidxs);
    }
	
	/*
	err = xdp_redirect_bpf__attach(skel);
	if (err) {
		fprintf(stderr, "Failed to attach BPF skeleton\n");
		goto cleanup;
	}
	*/
	printf("Successfully started! Please run `sudo cat /sys/kernel/debug/tracing/trace_pipe` "
	       "to see output of the BPF programs.\n");

	for (;;) {
		/* trigger our BPF program */
		fprintf(stderr, ".");
		sleep(1);
	}

cleanup:
	xdp_redirect_bpf__destroy(skel);
	return -err;
}
