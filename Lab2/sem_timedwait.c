#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>

sem_t sem;

static void
sa(int sig)
{
    int sav_errno = errno;
    static const char info_msg[] = "sem_post() from handler\n";
    write(STDOUT_FILENO, info_msg, sizeof info_msg - 1);
    if (sem_post(&sem) == -1) {
        static const char err_msg[] = "sem_post() failed\n";
        write(STDERR_FILENO, err_msg, sizeof err_msg - 1);
        _exit(EXIT_FAILURE);
    }
    errno = sav_errno;
}

int
main(int argc, char *argv[])
{
    struct timespec ts;
    int s;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <alarm-secs> <wait-secs>\n",
            argv[0]);
        exit(EXIT_FAILURE);
    }


    if (sem_init(&sem, 0, 0) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }


    signal(SIGALRM, sa);
    alarm(atoi(argv[1]));


    /* Compute relative interval as current time plus
       number of seconds given argv[2] */

    ts.tv_sec = time (NULL) + atoi(argv[2]);
    ts.tv_nsec = 0;

    printf("main() about to call sem_timedwait()\n");
    while ((s = sem_timedwait(&sem, &ts)) == -1 && errno == EINTR)
        continue;       /* Restart if interrupted by handler */


    /* Check what happened */


    if (s == -1) {
        if (errno == ETIMEDOUT)
            printf("sem_timedwait() timed out\n");
        else
            perror("sem_timedwait");
    } else
        printf("sem_timedwait() succeeded\n");


    exit((s == 0) ? EXIT_SUCCESS : EXIT_FAILURE);
}
