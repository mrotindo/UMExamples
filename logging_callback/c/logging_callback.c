/* Code Disclaimer? */

#include <stdio.h>

#if defined(_MSC_VER)
/* Windows-only includes */
#include <winsock2.h>
#define SLEEP(s) Sleep((s)*1000)
#else
/* Unix-only includes */
#include <stdlib.h>
#include <unistd.h>
#define SLEEP(s) sleep(s)
#endif

#include <lbm/lbm.h>

/* Logging callback */
int lbm_log_msg(int level, const char *message, void *clientd)
{
        printf("LOG Level %d: %s\n", level, message);
        return 0;
}

main()
{
	lbm_context_t *ctx;             /* pointer to context object */
	lbm_context_attr_t * cattr;     /* pointer to context attribute object */
	lbm_topic_t *topic;             /* pointer to topic object */
	lbm_src_topic_attr_t *tattr;    /* pointer to source attribute object */
	lbm_src_t *src;                 /* pointer to source object */
	int err;                        /* return status of lbm functions (true=error) */

#if defined(_MSC_VER)
        /* windows-specific code */
        WSADATA wsadata;
        int wsStat = WSAStartup(MAKEWORD(2,2), &wsadata);
        if (wsStat != 0)
        {
                printf("line %d: wsStat=%d\n",__LINE__,wsStat);
                exit(1);
        }
#endif

        /* Setup logging callback */
        if (lbm_log(lbm_log_msg, NULL) == LBM_FAILURE) {
                fprintf(stderr, "lbm_log: %s\n", lbm_errmsg());
                exit(1);
        }

        err = lbm_context_create(&ctx, NULL, NULL, NULL);
        if (err)
        {
                printf("line %d: %s\n", __LINE__, lbm_errmsg());
                exit(1);
        }

	/* Initializing the source attribute object */
	if (lbm_src_topic_attr_create(&tattr) != 0)
	{
		fprintf(stderr, "lbm_src_topic_attr_create: %s\n", lbm_errmsg());
		exit(1);
	}

	/* Setting late_join */
        if (lbm_src_topic_attr_str_setopt(tattr, "late_join", "1") != 0)
        {
                fprintf(stderr, "lbm_context_attr_str_setopt:late_join: %s\n", lbm_errmsg());
                exit(1);
        }

	/* Allocating the topic */
	err = lbm_src_topic_alloc(&topic, ctx, "test.topic.1", tattr);
	if (err)
	{
		printf("line %d: %s\n", __LINE__, lbm_errmsg());
		exit(1);
	}

	/* Creating the source */
	err = lbm_src_create(&src, ctx, topic, NULL, NULL, NULL);
	if (err)
	{
		printf("line %d: %s\n", __LINE__, lbm_errmsg());
		exit(1);
	}

	/* At the very least, the following core message should print via the logging callback: */
	/* LOG Level 5: Core-5688-539: NOTICE: Source "test.topic.1" has no retention settings (1 message retained max) */

	/* Cleanup */
	lbm_src_delete(src);
	lbm_context_delete(ctx);
}

