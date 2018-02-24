#include <stdio.h>
#include <string.h>

#include "unitooth.h"

#define USAGE	"ut {connect,listen,find} ...\n"

int main(int argc, char **argv) {

	if (argc-- == 1) {
		printf(USAGE);
		return 1;
	}

	argv = &argv[1];

	/* attempt to dispatch to chosen command */
	if (!strcmp("listen", argv[0]))
		return run_listen(argc, argv);
	else if (!strcmp("connect", argv[0]))
		return run_connect(argc, argv);
	else if (!strcmp("find", argv[0]))
		return run_find(argc, argv);
	else {
		printf("invalid command '%s'\n", argv[0]);
		printf(USAGE);
		return 1;
	}

	/* shouldn't get here */
	return 1;
}
